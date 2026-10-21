#include "recon.hpp"

#include <windows.h>
#include <intrin.h>

#include <MinHook.h>

#include <atomic>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "lw/util.hpp"

#pragma intrinsic(_ReturnAddress)

namespace lwp {

namespace {
// Character layout, verified by disassembly on build 0x6A3AAFBD (docs/RE_NOTES.md).
constexpr size_t kMaxHealth = 0x1970;
constexpr size_t kHealth = 0x1974;
constexpr size_t kArchetype = 0x3E84;

using SetFloatFn = void(__fastcall*)(void*, float);
SetFloatFn g_origSetMax = nullptr;
SetFloatFn g_origSetHealth = nullptr;
ReconAddresses g_a;
std::string g_csv;
std::atomic<bool> g_on{false};
std::atomic<uint64_t> g_dropped{0}, g_total{0};

enum Kind : uint8_t { SetMax = 0, Damage = 1, Death = 2, Heal = 3 };
struct Sample {
    uint64_t tick;
    uintptr_t ent;
    uint32_t vtRva;
    int32_t arch;
    float maxHp, oldHp, newHp;
    uint32_t caller;
    uint8_t kind;
};
SRWLOCK g_lock = SRWLOCK_INIT;
std::vector<Sample> g_buf;

uintptr_t playerPtr() {
    return g_a.playerGlobal ? *reinterpret_cast<uintptr_t*>(g_a.playerGlobal) : 0;
}

void record(void* c, Kind k, float oldHp, float newHp, void* ret) {
    g_total++;
    if (!TryAcquireSRWLockExclusive(&g_lock)) { g_dropped++; return; }   // never block the game
    if (g_buf.size() < 20000) {
        auto p = static_cast<uint8_t*>(c);
        uintptr_t vt = *reinterpret_cast<uintptr_t*>(p);
        g_buf.push_back(Sample{GetTickCount64(), reinterpret_cast<uintptr_t>(c),
                               static_cast<uint32_t>(vt - g_a.base), *reinterpret_cast<int32_t*>(p + kArchetype),
                               *reinterpret_cast<float*>(p + kMaxHealth), oldHp, newHp,
                               static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ret) - g_a.base), static_cast<uint8_t>(k)});
    } else {
        g_dropped++;
    }
    ReleaseSRWLockExclusive(&g_lock);
}

void __fastcall hkSetMax(void* c, float v) {
    g_origSetMax(c, v);
    if (g_on && c) record(c, SetMax, 0.f, v, _ReturnAddress());
}

void __fastcall hkSetHealth(void* c, float v) {
    float old = c ? *reinterpret_cast<float*>(static_cast<uint8_t*>(c) + kHealth) : 0.f;
    g_origSetHealth(c, v);
    if (!g_on || !c || v == old) return;
    Kind k = v > old ? Heal : (v <= 0.f && old > 0.f ? Death : Damage);
    if (k == Heal) return;   // heals are noise for the census
    record(c, k, old, v, _ReturnAddress());
}

const char* kindName(uint8_t k) {
    switch (k) { case SetMax: return "spawn/maxhp"; case Damage: return "damage"; case Death: return "death"; default: return "heal"; }
}
} // namespace

bool reconStart(const ReconAddresses& a, const std::string& csvPath) {
    g_a = a;
    g_csv = csvPath;
    if (!a.setMaxHealth || !a.setHealth) { lw::Log::warn("recon: health functions not resolved - recon off"); return false; }
    if (MH_Initialize() != MH_OK) { lw::Log::error("recon: MinHook init failed"); return false; }
    if (MH_CreateHook(reinterpret_cast<void*>(a.setMaxHealth), reinterpret_cast<void*>(&hkSetMax), reinterpret_cast<void**>(&g_origSetMax)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void*>(a.setHealth), reinterpret_cast<void*>(&hkSetHealth), reinterpret_cast<void**>(&g_origSetHealth)) != MH_OK) {
        lw::Log::error("recon: could not create hooks");
        MH_Uninitialize();
        return false;
    }
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) { lw::Log::error("recon: could not enable hooks"); MH_Uninitialize(); return false; }
    {
        std::ofstream f(g_csv, std::ios::app);
        f << "ms,kind,entity,is_player,vtable_rva,archetype,max_hp,old_hp,new_hp,caller_rva,note\n";
    }
    g_on = true;
    lw::Log::info("recon: watching SetMaxHealth/SetHealth (read-only) -> " + g_csv);
    return true;
}

void reconFlush() {
    std::vector<Sample> take;
    AcquireSRWLockExclusive(&g_lock);
    take.swap(g_buf);
    ReleaseSRWLockExclusive(&g_lock);
    if (take.empty()) return;
    std::ofstream f(g_csv, std::ios::app);
    uintptr_t player = playerPtr();
    char line[256];
    for (auto& s : take) {
        std::snprintf(line, sizeof line, "%llu,%s,%llX,%d,%X,%d,%.1f,%.1f,%.1f,%X,\n", static_cast<unsigned long long>(s.tick),
                      kindName(s.kind), static_cast<unsigned long long>(s.ent), s.ent == player ? 1 : 0, s.vtRva, s.arch,
                      s.maxHp, s.oldHp, s.newHp, s.caller);
        f << line;
    }
}

void reconMark(const std::string& note) {
    reconFlush();
    std::ofstream f(g_csv, std::ios::app);
    f << GetTickCount64() << ",MARK,,,,,,,,," << note << "\n";
}

void reconStop() {
    if (!g_on.exchange(false)) return;
    MH_DisableHook(MH_ALL_HOOKS);
    reconFlush();
    // Leave MinHook initialised: the game may still be inside a trampoline during shutdown.
}

std::string reconStatus() {
    return lw::fmt("recon: %llu events seen, %llu dropped", static_cast<unsigned long long>(g_total.load()),
                   static_cast<unsigned long long>(g_dropped.load()));
}

} // namespace lwp
