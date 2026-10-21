// Living War plugin main loop.
//
// What works today (no game hooks needed):
//   - loads/saves the persistent world in <game>/plugins/LivingWar/
//   - advances the world while you play (real-time stand-in for the game clock)
//   - CHRONICLE MODE: you tell Living War about fights with hotkeys, and it runs the full
//     nemesis loop (identities, memories, scars, revenge, promotions, succession)
//   - F9 writes and opens the hierarchy page
//   - probes GotSignatures.ini and logs which hooks this game build supports
//   - OBSERVER (0.3.0): recognises camp/patrol leaders as they spawn (the game gives them x1.75
//     health), gives each a Living War identity, follows the fight, and records how it ended
// What doesn't work yet: changing enemies in-game (health, retinue, tactics).
#include "plugin.hpp"

#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "got_adapter.hpp"
#include "ini.hpp"
#include "recon.hpp"
#include "lw/living_war.hpp"

namespace fs = std::filesystem;

namespace lwp {

namespace {
std::atomic<bool> g_stop{false};

struct Keys {
    int engage = VK_F7, region = VK_F8, hierarchy = VK_F9, save = VK_F10;
    int killBlade = VK_NUMPAD1, killStealth = VK_NUMPAD2, killFire = VK_NUMPAD3, killArrows = VK_NUMPAD4;
    int heFled = VK_NUMPAD5, spared = VK_NUMPAD6, jinDied = VK_NUMPAD7, jinEscaped = VK_NUMPAD8, menKilled = VK_NUMPAD0;
};

class Edge {   // fires once per key press
public:
    bool pressed(int vk) {
        bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
        bool was = state_[vk];
        state_[vk] = down;
        return down && !was;
    }
private:
    std::map<int, bool> state_;
};

bool gameHasFocus() {
    DWORD pid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &pid);
    return pid == GetCurrentProcessId();
}

std::string moduleDir(HMODULE m) {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(m, buf, MAX_PATH);
    return fs::path(buf).parent_path().string();
}

const char* kRegions[] = {"izuhara", "toyotama", "kamiagata", "iki"};
} // namespace

void requestStop() { g_stop = true; }

DWORD WINAPI pluginThread(LPVOID module) {
    const std::string pluginDir = moduleDir(static_cast<HMODULE>(module));
    const std::string dataDir = (fs::path(pluginDir) / "LivingWar").string();
    fs::create_directories(dataDir);
    lw::Log::setFile((fs::path(dataDir) / "LivingWar.log").string());

    Ini ini;
    ini.load((fs::path(pluginDir) / "LivingWar.ini").string());
    lw::Log::setLevel(ini.getBool("General", "DebugLog", false) ? lw::LogLevel::Debug : lw::LogLevel::Info);
    lw::Log::info("==== Living War 0.3.1 starting ====");
    if (!ini.getBool("General", "Enabled", true)) { lw::Log::info("disabled in LivingWar.ini"); return 0; }

    Sleep(static_cast<DWORD>(ini.getInt("General", "StartDelaySeconds", 10) * 1000));   // let the game finish booting

    auto adapter = std::make_unique<GotAdapter>((fs::path(dataDir) / "feed.txt").string());
    lw::Log::info(adapter->probe((fs::path(pluginDir) / "GotSignatures.ini").string()));
    uint32_t expected = 0;
    try { expected = static_cast<uint32_t>(std::stoul(ini.get("General", "ExpectedBuildTimestamp", "0"), nullptr, 0)); } catch (...) {}
    const bool buildOk = expected != 0 && expected == adapter->image().timestamp;
    if (!buildOk)
        lw::Log::warn(lw::fmt("game build 0x%08X is not the tested build 0x%08X - hooks stay off, Chronicle mode only", adapter->image().timestamp, expected));

    bool recon = false;
    if (buildOk && ini.getBool("Recon", "Enabled", true)) {
        ReconAddresses ra;
        ra.base = adapter->image().base;
        ra.setMaxHealth = adapter->addr("lw_setmaxhealth");
        ra.setHealth = adapter->addr("lw_sethealth");
        ra.playerGlobal = adapter->addr("pplayer");
        try { ra.leaderRatio = std::stof(ini.get("Observer", "LeaderHealthRatio", "1.75")); } catch (...) {}
        {
            std::string lt = ini.get("Observer", "LeaderTypes", "35"), cur;
            for (char ch : lt + ",") {
                if (ch == ',' || ch == ' ') { if (!cur.empty()) { try { ra.leaderTypes.push_back(std::stoi(cur)); } catch (...) {} cur.clear(); } }
                else cur += ch;
            }
        }
        recon = reconStart(ra, (fs::path(dataDir) / "census.csv").string());
    }
    const int markKey = ini.getInt("Recon", "Mark", VK_F11);

    lw::LivingWar war(adapter.get(), dataDir);
    war.start(ini.get("General", "SaveSlot", "default"), static_cast<uint64_t>(ini.getInt("General", "WorldSeed", 1274)),
              ini.getInt("General", "CampaignAct", 1));
    war.setAct(ini.getInt("General", "CampaignAct", 1));
    lw::Log::info(war.statusLine());

    Keys k;
    k.engage = ini.getInt("Keys", "Engage", k.engage);
    k.region = ini.getInt("Keys", "CycleRegion", k.region);
    k.hierarchy = ini.getInt("Keys", "Hierarchy", k.hierarchy);
    k.save = ini.getInt("Keys", "Save", k.save);
    const bool chronicle = ini.getBool("Chronicle", "Enabled", true);
    const int realMinutesPerDay = std::max(1, ini.getInt("World", "RealMinutesPerWorldDay", 30));
    int regionIdx = 0;
    {
        std::string r = ini.get("Chronicle", "StartRegion", "izuhara");
        for (int i = 0; i < 4; ++i) if (r == kRegions[i]) regionIdx = i;
    }

    const bool observer = recon && ini.getBool("Observer", "Enabled", true);
    const int leaderMaxRank = ini.getInt("Observer", "MaxRank", 2);
    std::map<lw::ActorHandle, ULONGLONG> lastHit;   // when each tracked leader was last hit by Jin
    if (observer) lw::Log::info("observer: automatic leader tracking ON");

    Edge edge;
    lw::ActorHandle engaged = 0, nextHandle = 1;
    ULONGLONG playMs = 0, last = GetTickCount64();
    const int64_t baseMinutes = static_cast<int64_t>(war.world().day) * 1440;
    std::string feed = (fs::path(dataDir) / "feed.txt").string();
    adapter->notify("Living War", "loaded - " + war.statusLine());

    while (!g_stop) {
        Sleep(50);
        ULONGLONG now = GetTickCount64();
        bool focus = gameHasFocus();
        if (focus) playMs += now - last;   // the world only moves while you're actually playing
        last = now;
        war.onGameMinutes(baseMinutes + static_cast<int64_t>(playMs / 60000ULL) * 1440 / realMinutesPerDay);
        if (recon) {
            static ULONGLONG lastFlush = 0, lastStatus = 0;
            if (now - lastFlush > 1000) { reconFlush(); lastFlush = now; }
            if (now - lastStatus > 60000) { lw::Log::info(reconStatus()); lastStatus = now; }
        }
        if (observer) {
            for (auto& e : observerTakeEvents()) {
                lw::ActorHandle h = static_cast<lw::ActorHandle>(e.entity);
                switch (e.kind) {
                case GameEvent::LeaderSpawned:
                    if (!war.isTracked(h)) {
                        auto p = war.onLeaderSpawned(h, kRegions[regionIdx], leaderMaxRank);
                        if (p) lw::Log::info(lw::fmt("observer: leader type %d (%.0f hp) is %s", e.type, e.maxHp, p->title.c_str()));
                    }
                    break;
                case GameEvent::LeaderDamaged:
                    if (war.isTracked(h)) {
                        if (!lastHit.count(h)) adapter->notify("You face " + war.trackedName(h), "the fight is on");
                        lastHit[h] = now;
                        war.onPlayerDetected(h);
                        war.onLeaderDamaged(h, lw::Wound::Cut, static_cast<int>(e.oldHp - e.newHp), lw::Method::Melee);
                    }
                    break;
                case GameEvent::LeaderDied:
                    if (war.isTracked(h)) {
                        // One blow from full health = an assassination; otherwise a fight.
                        bool oneShot = e.oldHp >= e.maxHp * 0.99f;
                        lw::Method m = oneShot ? lw::Method::Stealth : lw::Method::Melee;
                        if (!oneShot) war.onPlayerDetected(h);
                        war.onLeaderDamaged(h, lw::Wound::Cut, static_cast<int>(e.oldHp), m);
                        std::string who = war.trackedName(h);
                        war.onLeaderKilled(h, m);
                        lastHit.erase(h);
                        lw::Log::info("observer: " + who + (oneShot ? " assassinated" : " cut down"));
                    }
                    break;
                case GameEvent::LeaderGone:
                    if (war.isTracked(h)) {
                        if (war.wasEngaged(h)) war.onEncounterEnded(h); else war.onLeaderReleased(h);
                        lastHit.erase(h);
                    }
                    break;
                case GameEvent::SoldierDied: {
                    // Credit the death to the leader Jin was fighting most recently (within a minute).
                    lw::ActorHandle best = 0; ULONGLONG bestT = 0;
                    for (auto& kv : lastHit) if (now - kv.second < 60000 && kv.second > bestT) { best = kv.first; bestT = kv.second; }
                    if (best) war.onSoldierKilled(best);
                    break;
                }
                case GameEvent::PlayerDied: {
                    lw::ActorHandle best = 0; ULONGLONG bestT = 0;
                    for (auto& kv : lastHit) if (now - kv.second < 60000 && kv.second > bestT) { best = kv.first; bestT = kv.second; }
                    war.onPlayerKilled(best);
                    lastHit.clear();
                    lw::Log::info("observer: Jin fell" + std::string(best ? " - credited to the leader he was fighting" : ""));
                    break;
                }
                }
            }
        }
        if (!focus) continue;
        if (recon && edge.pressed(markKey)) { reconMark("leader killed (player mark)"); adapter->notify("Recon", "marked"); }

        if (edge.pressed(k.hierarchy)) {
            std::string html = (fs::path(dataDir) / "hierarchy.html").string();
            if (war.exportHierarchy(html)) ShellExecuteA(nullptr, "open", html.c_str(), nullptr, nullptr, SW_SHOWNOACTIVATE);
        }
        if (edge.pressed(k.save)) { war.save(); adapter->notify("Living War", "saved"); }
        if (!chronicle) continue;

        if (edge.pressed(k.region)) {
            regionIdx = (regionIdx + 1) % 4;
            adapter->notify("Chronicle", std::string("region set to ") + kRegions[regionIdx]);
        }
        if (edge.pressed(k.engage)) {
            if (engaged) war.onEncounterEnded(engaged);
            engaged = nextHandle++;
            auto p = war.onLeaderSpawned(engaged, kRegions[regionIdx], ini.getInt("Chronicle", "MaxRank", 3));
            if (p) {
                war.onPlayerDetected(engaged);
                std::string tactics;
                for (auto& t : p->tactics) tactics += t + " ";
                adapter->notify("You face " + p->title, std::string("stance ") + lw::toString(p->stance) +
                                ", " + std::to_string(p->retinueTotal()) + " men. " + tactics);
            } else {
                engaged = 0;
            }
        }
        if (!engaged) continue;
        auto end = [&](auto fn) { fn(); engaged = 0; };
        if (edge.pressed(k.menKilled)) { for (int i = 0; i < 5; ++i) war.onSoldierKilled(engaged); adapter->notify("Chronicle", "+5 of his men killed"); }
        else if (edge.pressed(k.killBlade)) end([&] { war.onLeaderDamaged(engaged, lw::Wound::Cut, 50, lw::Method::Melee); war.onLeaderKilled(engaged, lw::Method::Melee); });
        else if (edge.pressed(k.killStealth)) end([&] { war.onLeaderKilled(engaged, lw::Method::Stealth); });
        else if (edge.pressed(k.killFire)) end([&] { war.onLeaderDamaged(engaged, lw::Wound::Burn, 60, lw::Method::Fire); war.onLeaderKilled(engaged, lw::Method::Fire); });
        else if (edge.pressed(k.killArrows)) end([&] { war.onLeaderDamaged(engaged, lw::Wound::Arrow, 50, lw::Method::Ranged); war.onLeaderKilled(engaged, lw::Method::Ranged); });
        else if (edge.pressed(k.heFled)) end([&] { war.onLeaderFled(engaged); });
        else if (edge.pressed(k.spared)) end([&] { war.onLeaderSpared(engaged); });
        else if (edge.pressed(k.jinDied)) end([&] { war.onPlayerKilled(engaged); });
        else if (edge.pressed(k.jinEscaped)) end([&] { war.onPlayerEscaped(engaged); });
    }
    if (recon) reconStop();
    war.save();
    lw::Log::info("==== Living War stopped ====");
    return 0;
}

} // namespace lwp
