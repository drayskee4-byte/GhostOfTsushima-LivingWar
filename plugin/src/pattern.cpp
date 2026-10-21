#include "pattern.hpp"

#include <windows.h>

#include <cctype>
#include <sstream>

namespace lwp {

ModuleImage mainModule() {
    ModuleImage m;
    auto base = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    m.base = base;
    m.size = nt->OptionalHeader.SizeOfImage;
    m.timestamp = nt->FileHeader.TimeDateStamp;
    return m;
}

bool parsePattern(const std::string& text, std::vector<int>& out) {
    out.clear();
    std::istringstream in(text);
    std::string tok;
    while (in >> tok) {
        if (tok == "?" || tok == "??" || tok == "*" || tok == "**") { out.push_back(-1); continue; }
        if (tok.size() != 2 || !std::isxdigit(static_cast<unsigned char>(tok[0])) || !std::isxdigit(static_cast<unsigned char>(tok[1]))) return false;
        out.push_back(std::stoi(tok, nullptr, 16));
    }
    return !out.empty();
}

std::optional<uintptr_t> scan(const ModuleImage& m, const std::vector<int>& pat) {
    if (pat.empty()) return std::nullopt;
    // Walk committed, readable regions only, so a guard page can't crash the game.
    uintptr_t addr = m.base, end = m.base + m.size;
    while (addr < end) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof mbi)) break;
        uintptr_t rStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        uintptr_t rEnd = rStart + mbi.RegionSize;
        bool readable = mbi.State == MEM_COMMIT && !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
                        (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE));
        if (readable && rEnd - rStart >= pat.size()) {
            auto* p = reinterpret_cast<const uint8_t*>(rStart);
            size_t n = (rEnd - rStart) - pat.size();
            for (size_t i = 0; i <= n; ++i) {
                size_t k = 0;
                for (; k < pat.size(); ++k) if (pat[k] >= 0 && p[i + k] != pat[k]) break;
                if (k == pat.size()) return rStart + i;
            }
        }
        addr = rEnd;
    }
    return std::nullopt;
}

std::optional<uintptr_t> resolve(const ModuleImage& m, const Signature& s) {
    std::vector<int> pat;
    if (s.pattern.empty() || !parsePattern(s.pattern, pat)) return std::nullopt;
    auto hit = scan(m, pat);
    if (!hit) return std::nullopt;
    if (s.b == 0) return *hit + s.a;
    int32_t disp = *reinterpret_cast<const int32_t*>(*hit + s.a);
    return *hit + s.b + disp;
}

} // namespace lwp
