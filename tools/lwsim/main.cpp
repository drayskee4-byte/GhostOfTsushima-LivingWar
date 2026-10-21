// lwsim - run the Living War core without the game.
//
//   lwsim demo      [--data DIR]                 the milestone: one commander who remembers Jin
//   lwsim simulate  [--data DIR] [--days N] [--seed S] [--slot NAME]
//   lwsim encounter --id ID --outcome O [--method M] [--burn N --cut N --arrow N] [--men N] [--data DIR] [--slot NAME]
//   lwsim advance   --days N [--data DIR] [--slot NAME]
//   lwsim show      --id ID [--data DIR] [--slot NAME]
//   lwsim list      [--region R] [--data DIR] [--slot NAME]
//   lwsim export    --out FILE.html [--data DIR] [--slot NAME]
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <string>

#include "lw/living_war.hpp"
#include "mock_adapter.hpp"

using namespace lw;

namespace {
std::map<std::string, std::string> parseArgs(int argc, char** argv, int from) {
    std::map<std::string, std::string> a;
    for (int i = from; i < argc; ++i) {
        std::string k = argv[i];
        if (k.rfind("--", 0) == 0) {
            std::string v = (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0) ? argv[++i] : "1";
            a[k.substr(2)] = v;
        }
    }
    return a;
}
std::string get(const std::map<std::string, std::string>& a, const std::string& k, const std::string& d) {
    auto it = a.find(k);
    return it == a.end() ? d : it->second;
}

void printProfile(const CombatProfile& p) {
    std::printf("    %s  (%s, stance: %s)\n", p.title.c_str(), toString(p.archetype), toString(p.stance));
    std::printf("    health x%.2f  damage x%.2f  aggression %.2f  alertness %.2f  flee<%.0f%%  retinue %d\n",
                p.healthMult, p.damageMult, p.aggression, p.alertness, p.fleeAtHealth * 100, p.retinueTotal());
    if (!p.resist.empty()) {
        std::printf("    resists:");
        for (auto& kv : p.resist) std::printf(" %s %.0f%%", toString(kv.first), kv.second * 100);
        std::printf("\n");
    }
    if (!p.tactics.empty()) {
        std::printf("    tactics:");
        for (auto& t : p.tactics) std::printf(" %s", t.c_str());
        std::printf("\n");
    }
    if (!p.panicTriggers.empty()) {
        std::printf("    panics at:");
        for (auto& t : p.panicTriggers) std::printf(" %s", t.c_str());
        std::printf("\n");
    }
    for (auto& r : p.retinue) std::printf("      + %d %s (%s)\n", r.count, toString(r.archetype), r.role.c_str());
    std::printf("    dialogue: %s\n", p.tauntKey.c_str());
}

void printCommander(const Commander& c, const Config& cfg) {
    std::printf("%s [%s] %s\n", c.fullTitle().c_str(), c.id.c_str(), c.alive ? (c.presumedDead ? "(presumed dead)" : "") : "(DEAD)");
    std::printf("  %s from %s, posted to %s. Hate %d, fear %d. Goal: %s\n", toString(c.archetype), c.origin.c_str(),
                c.region.c_str(), c.hate, c.fearOfJin, c.goal.text.empty() ? "-" : c.goal.text.c_str());
    std::printf("  Record: met Jin %d, beaten %d, beat him %d, fled %d, cheated death %d, men lost %d\n",
                c.stats.encounters, c.stats.defeatsByJin, c.stats.victoriesOverJin, c.stats.timesFled,
                c.stats.timesCheatedDeath, c.stats.menLostToJin);
    for (auto& s : c.scars) std::printf("  scar: %s\n", s.text.c_str());
    for (auto& a : c.adaptations) std::printf("  learned: %s %d - %s\n", a.id.c_str(), a.level, a.reason.c_str());
    for (auto& m : c.memories) std::printf("  remembers (%s %d): %s\n", toString(m.emotion), m.intensity, m.text.c_str());
    std::printf("  profile:\n");
    printProfile(buildProfile(c, cfg));
}

int demo(const std::string& data) {
    std::printf("=== Living War prototype: ONE MONGOL COMMANDER WHO REMEMBERS JIN ===\n\n");
    std::filesystem::remove_all(std::filesystem::path(data) / "saves" / "demo");
    MockAdapter game;
    CommanderId id;
    {
        LivingWar lw(&game, data);
        lw.start("demo", 1274, 1);
        std::printf("1. The game spawns a camp leader in Izuhara. Living War gives him an identity:\n");
        auto prof = lw.onLeaderSpawned(1, "izuhara", 2);
        if (!prof) { std::printf("no commander offered\n"); return 1; }
        id = prof->commander;
        printProfile(*prof);

        std::printf("\n2. Jin attacks the camp openly with fire: burns him, kills six of his men, cuts him down.\n");
        lw.onPlayerDetected(1);
        for (int i = 0; i < 6; ++i) lw.onSoldierKilled(1);
        lw.onLeaderDamaged(1, Wound::Burn, 60, Method::Fire);
        lw.onLeaderDamaged(1, Wound::Cut, 40, Method::Melee);
        lw.world().cfg.cheatDeathAlways = true;   // demo only: force the survival roll so the story can continue
        lw.onLeaderKilled(1, Method::Fire);
        lw.world().cfg.cheatDeathAlways = false;
        const Commander& c = *lw.world().find(id);
        std::printf("   -> alive=%s presumedDead=%s, recovers on day %d\n", c.alive ? "yes" : "no",
                    c.presumedDead ? "yes" : "no", c.recoverUntil);
        std::printf("\n3. The world is saved to disk and the game is 'closed'.\n");
        lw.save();
    }
    {
        MockAdapter game2;
        LivingWar lw(&game2, data);
        lw.start("demo");
        std::printf("4. A new session loads the save. %s\n", lw.statusLine().c_str());
        std::printf("\n5. Days pass while Jin explores (simulated in-game clock).\n");
        for (int d = 0; d <= 10; ++d) lw.onGameMinutes(static_cast<int64_t>(d) * 1440);
        std::printf("   %s\n", lw.statusLine().c_str());

        std::printf("\n6. The game spawns another leader in Izuhara. Who shows up?\n");
        auto prof = lw.onLeaderSpawned(2, "izuhara", 2);
        bool same = prof && prof->commander == id;
        std::printf("   -> %s\n", same ? "HE CAME BACK." : "someone else (plan not ready yet)");
        if (prof) printProfile(*prof);

        std::printf("\n7. This time Jin comes from the shadows; the commander flees.\n");
        lw.onLeaderDamaged(2, Wound::Cut, 30, Method::Stealth);
        lw.onLeaderFled(2);
        const Commander& c = *lw.world().find(id);
        std::printf("\n8. Third meeting - his profile now reflects both defeats:\n");
        for (int d = 11; d <= 16; ++d) lw.onGameMinutes(static_cast<int64_t>(d) * 1440);
        printCommander(c, lw.world().cfg);

        std::string html = (std::filesystem::path(data) / "hierarchy_demo.html").string();
        lw.exportHierarchy(html);
        std::printf("\nHierarchy written to %s\n", html.c_str());
        return same ? 0 : 2;
    }
}

int simulate(const std::map<std::string, std::string>& a, const std::string& data) {
    int days = std::atoi(get(a, "days", "30").c_str());
    uint64_t seed = std::strtoull(get(a, "seed", "1274").c_str(), nullptr, 10);
    std::string slot = get(a, "slot", "sim");
    MockAdapter game;
    game.quiet = true;
    LivingWar lw(&game, data);
    lw.start(slot, seed, std::atoi(get(a, "act", "1").c_str()));
    Rng jin(seed ^ 0x6A696E);   // the simulated player's choices
    static const Method methods[] = {Method::Stealth, Method::Standoff, Method::Melee, Method::Ranged, Method::Fire, Method::Explosive};
    ActorHandle h = 1;
    int64_t minutes = static_cast<int64_t>(lw.world().day) * 1440;
    for (int d = 0; d < days; ++d) {
        int fights = jin.range(0, 3);
        for (int f = 0; f < fights; ++f, ++h) {
            std::vector<std::string> open;
            for (auto& r : lw.world().regions) if (r.second.unlocked) open.push_back(r.first);
            std::string region = jin.pick(open);
            auto p = lw.onLeaderSpawned(h, region, 3);
            if (!p) continue;
            if (jin.chance(70)) lw.onPlayerDetected(h);
            Method m = methods[jin.range(0, 5)];
            Wound w = m == Method::Fire ? Wound::Burn : m == Method::Ranged ? Wound::Arrow : m == Method::Explosive ? Wound::Blast : Wound::Cut;
            lw.onLeaderDamaged(h, w, jin.range(10, 80), m);
            int men = jin.range(0, 7);
            for (int i = 0; i < men; ++i) lw.onSoldierKilled(h);
            int roll = jin.range(1, 100);
            if (roll <= 62) lw.onLeaderKilled(h, m);
            else if (roll <= 80) lw.onLeaderFled(h);
            else if (roll <= 86) lw.onLeaderSpared(h);
            else if (roll <= 94) lw.onPlayerKilled(h);
            else lw.onPlayerEscaped(h);
            if (jin.chance(12)) lw.onCampLiberated("camp near " + region, region, men + 3);
        }
        minutes += 1440;
        lw.onGameMinutes(minutes);
    }
    std::printf("%s\n\nLast 25 events:\n", lw.statusLine().c_str());
    auto& ev = lw.world().events;
    for (size_t i = ev.size() > 25 ? ev.size() - 25 : 0; i < ev.size(); ++i)
        std::printf("  day %3d  %s\n", ev[i].day, ev[i].text.c_str());
    std::string html = get(a, "out", (std::filesystem::path(data) / ("hierarchy_" + slot + ".html")).string());
    lw.exportHierarchy(html);
    std::printf("\nHierarchy written to %s\n", html.c_str());
    return 0;
}
} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: lwsim demo|simulate|encounter|advance|show|list|export [options]\n");
        return 1;
    }
    std::string cmd = argv[1];
    auto a = parseArgs(argc, argv, 2);
    std::string data = get(a, "data", "LivingWarData");
    Log::setLevel(a.count("verbose") ? LogLevel::Debug : LogLevel::Warn);
    Log::setFile((std::filesystem::path(data) / "lwsim.log").string());
    std::filesystem::create_directories(data);

    if (cmd == "demo") return demo(data);
    if (cmd == "simulate") return simulate(a, data);

    MockAdapter game;
    game.quiet = true;
    LivingWar lw(&game, data);
    lw.start(get(a, "slot", "sim"));
    if (cmd == "list") {
        std::string region = get(a, "region", "");
        for (auto& kv : lw.world().commanders) {
            auto& c = kv.second;
            if (c.storyProtected || !c.alive || (!region.empty() && c.region != region)) continue;
            std::printf("%s  %-10s %-34s hate %3d  met Jin %d\n", c.id.c_str(), c.region.c_str(), c.fullTitle().c_str(), c.hate, c.stats.encounters);
        }
        return 0;
    }
    if (cmd == "show") {
        const Commander* c = lw.world().find(get(a, "id", ""));
        if (!c) { std::printf("no such commander\n"); return 1; }
        printCommander(*c, lw.world().cfg);
        return 0;
    }
    if (cmd == "advance") {
        lw.nemesis().advanceDays(std::atoi(get(a, "days", "1").c_str()));
        lw.save();
        std::printf("%s\n", lw.statusLine().c_str());
        return 0;
    }
    if (cmd == "encounter") {
        EncounterReport r;
        r.commander = get(a, "id", "");
        r.outcome = fromString<Outcome>(get(a, "outcome", "commander_killed"));
        r.method = fromString<Method>(get(a, "method", "melee"));
        for (const char* w : {"burn", "cut", "arrow", "blast", "poison", "blunt"})
            if (a.count(w)) r.damageTaken[fromString<Wound>(w)] = std::atoi(a.at(w).c_str());
        r.subordinatesKilled = std::atoi(get(a, "men", "0").c_str());
        r.detectedJin = !a.count("unseen");
        auto res = lw.nemesis().resolve(r);
        for (auto& l : res.lines) std::printf("%s\n", l.c_str());
        lw.save();
        if (const Commander* c = lw.world().find(r.commander)) printCommander(*c, lw.world().cfg);
        return 0;
    }
    if (cmd == "export") {
        std::string out = get(a, "out", "hierarchy.html");
        bool ok = lw.exportHierarchy(out);
        std::printf("%s %s\n", ok ? "wrote" : "FAILED to write", out.c_str());
        return ok ? 0 : 1;
    }
    std::printf("unknown command %s\n", cmd.c_str());
    return 1;
}
