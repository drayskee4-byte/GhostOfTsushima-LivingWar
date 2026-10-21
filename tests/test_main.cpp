// Living War core tests. Run: lw_tests  (exit code 0 = all passed)
#include <cstdio>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "lw/living_war.hpp"
#include "mock_adapter.hpp"

using namespace lw;
namespace fs = std::filesystem;

static int g_fail = 0, g_pass = 0;
#define CHECK(cond) do { if (cond) ++g_pass; else { ++g_fail; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

struct TestCase { const char* name; std::function<void()> fn; };
static std::vector<TestCase>& tests() { static std::vector<TestCase> t; return t; }
struct Reg { Reg(const char* n, std::function<void()> f) { tests().push_back({n, std::move(f)}); } };
#define TEST(name) static void name(); static Reg reg_##name(#name, name); static void name()

static std::string tmpDir(const char* name) {
    auto p = fs::temp_directory_path() / ("lw_test_" + std::string(name));
    fs::remove_all(p);
    fs::create_directories(p);
    return p.string();
}

static Commander& anyArban(World& w, const std::string& region = "izuhara") {
    for (auto& kv : w.commanders) if (kv.second.alive && kv.second.region == region && kv.second.rank == Rank::Arban) return kv.second;
    throw std::runtime_error("no arban");
}

// ------------------------------------------------------------------ json
TEST(json_roundtrip) {
    Json j = Json::parse(R"({"a":1,"b":[true,false,null],"c":{"d":"x\"y\né"},"e":-2.5})");
    CHECK(j.i("a") == 1);
    CHECK(j["b"].size() == 3 && j["b"].at(0).asBool());
    CHECK(j["c"].str("d") == "x\"y\n\xc3\xa9");
    CHECK(j.num("e") == -2.5);
    Json k = Json::parse(j.dump(2));
    CHECK(k.dump() == j.dump());
    bool threw = false;
    try { Json::parse("{\"a\":}"); } catch (const std::exception&) { threw = true; }
    CHECK(threw);
}

TEST(json_copy_on_write) {
    Json a; a["x"] = 1;
    Json b = a; b["x"] = 2;
    CHECK(a.i("x") == 1 && b.i("x") == 2);
}

// ------------------------------------------------------------------ seeding / hierarchy
TEST(seed_builds_hierarchy) {
    World w; Nemesis n(w); n.seed(7, 1);
    CHECK(w.regions.size() == 4);
    CHECK(w.regions["izuhara"].unlocked && !w.regions["kamiagata"].unlocked);
    for (auto& r : w.regions) {
        CHECK(n.seatsUsed(r.first, Rank::Tumen) == 1);
        CHECK(n.seatsUsed(r.first, Rank::Mingghan) == 2);
        CHECK(n.seatsUsed(r.first, Rank::Arban) == 6);
    }
    // every simulated officer's chain reaches a story figure
    for (auto& kv : w.commanders) {
        if (kv.second.storyProtected) continue;
        auto chain = n.chainOfCommand(kv.first);
        CHECK(chain.back()->storyProtected);
    }
    CHECK(w.find("khotun_khan") && w.find("khotun_khan")->storyProtected);
}

TEST(seed_is_deterministic) {
    World a, b; Nemesis na(a), nb(b);
    na.seed(99); nb.seed(99);
    CHECK(a.toJson().dump() == b.toJson().dump());
}

// ------------------------------------------------------------------ cheating death
TEST(cheat_death_survives_with_scar_and_plan) {
    World w; Nemesis n(w); n.seed(3);
    w.cfg.cheatDeathAlways = true;
    Commander& c = anyArban(w);
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderKilled; r.method = Method::Fire;
    r.damageTaken[Wound::Burn] = 70;
    auto res = n.resolve(r);
    CHECK(res.cheatedDeath && !res.died);
    CHECK(c.alive && c.presumedDead);
    CHECK(c.scars.size() == 1 && c.scars[0].wound == Wound::Burn);
    CHECK(c.stats.timesCheatedDeath == 1);
    CHECK(c.hasAdaptation("fire_ward") || c.hasAdaptation("fire_fear"));
    CHECK(c.epithet == "the Burned");
    CHECK(c.goal.type == GoalType::Avenge);
    CHECK(!res.plansCreated.empty());
}

TEST(final_methods_always_kill) {
    World w; Nemesis n(w); n.seed(3);
    w.cfg.cheatDeathAlways = true;
    w.cfg.finalMethods = {Method::Stealth};
    Commander& c = anyArban(w);
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderKilled; r.method = Method::Stealth;
    auto res = n.resolve(r);
    CHECK(res.died && !c.alive);
    CHECK(!w.seats.empty());   // succession scheduled
}

TEST(cheat_death_rate_matches_formula) {
    // rank 1: 35% base * 1.0 (melee) -> expect roughly 35% over many trials
    int survived = 0, trials = 2000;
    for (int i = 0; i < trials; ++i) {
        World w; Nemesis n(w); n.seed(1000 + i);
        Commander& c = anyArban(w);
        EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderKilled; r.method = Method::Melee;
        if (n.resolve(r).cheatedDeath) ++survived;
    }
    double rate = 100.0 * survived / trials;
    std::printf("    rank-1 melee survival: %.1f%% (expected ~35%%)\n", rate);
    CHECK(rate > 30 && rate < 40);
}

// ------------------------------------------------------------------ adaptations drive the profile
TEST(stealth_defeat_makes_paranoid_profile) {
    World w; Nemesis n(w); n.seed(5);
    Commander& c = anyArban(w);
    auto before = buildProfile(c, w.cfg);
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderFled; r.method = Method::Stealth; r.detectedJin = false;
    n.resolve(r);
    CHECK(c.hasAdaptation("paranoid"));
    auto after = buildProfile(c, w.cfg);
    CHECK(after.alertness > before.alertness);
    bool neverAlone = false; for (auto& t : after.tactics) neverAlone |= t == "never_alone";
    CHECK(neverAlone);
    CHECK(after.retinueTotal() > before.retinueTotal());
}

TEST(arrows_bring_shields) {
    World w; Nemesis n(w); n.seed(5);
    Commander& c = anyArban(w);
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderFled; r.method = Method::Ranged;
    r.damageTaken[Wound::Arrow] = 40;
    n.resolve(r);
    auto p = buildProfile(c, w.cfg);
    CHECK(p.resist[Wound::Arrow] > 0);
    bool shields = false; for (auto& s : p.retinue) shields |= s.archetype == Archetype::Shield;
    CHECK(shields);
}

TEST(burned_coward_fears_fire) {
    World w; Nemesis n(w); n.seed(5);
    Commander& c = anyArban(w);
    c.facet(Facet::Courage) = 10;
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderFled; r.method = Method::Fire;
    r.damageTaken[Wound::Burn] = 50;
    n.resolve(r);
    CHECK(c.fears.count("fire") == 1);
    auto p = buildProfile(c, w.cfg);
    CHECK(!p.panicTriggers.empty() && p.panicTriggers[0] == "fire");
}

TEST(victory_emboldens_and_raises_rank) {
    World w; Nemesis n(w); n.seed(5);
    Commander& c = anyArban(w);
    c.goal = {};
    c.facet(Facet::Ambition) = 100;   // 1.5x XP
    // free a Jaghun seat so promotion is possible
    for (auto& kv : w.commanders) if (kv.second.region == c.region && kv.second.rank == Rank::Jaghun) { n.kill(kv.second, "test", false); break; }
    w.seats.clear();
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::JinKilled; r.method = Method::Melee;
    auto res = n.resolve(r);
    CHECK(c.hasAdaptation("emboldened"));
    CHECK(c.stats.victoriesOverJin == 1);
    CHECK(c.rank == Rank::Jaghun);   // 3 xp * 1.5 = 5 >= 2 needed at rank 1
    CHECK(res.rankedUp);
}

TEST(xp_follows_enrage_formula) {
    World w; Nemesis n(w); n.seed(5);
    Commander& c = anyArban(w);
    c.rank = Rank::Tumen; c.rankXp = 0; c.goal = {}; c.facet(Facet::Ambition) = 50;   // mult 1.0
    n.gainXp(c, 3, "test");
    CHECK(c.rankXp == 3);
    c.goal.type = GoalType::Rise;  // x1.5 -> round(4.5)=5 (banker's? lround -> 5)
    n.gainXp(c, 3, "test");
    CHECK(c.rankXp == 8);          // capped at xpNeed(4)=8
    c.goal.type = GoalType::Survive;
    int before = c.rankXp;
    n.gainXp(c, 3, "test");
    CHECK(c.rankXp == before);     // "Settle": no xp
}

// ------------------------------------------------------------------ death ripples, succession
TEST(subordinate_avenges_superior) {
    World w; Nemesis n(w); n.seed(11);
    Commander* boss = nullptr;
    for (auto& kv : w.commanders) if (kv.second.region == "izuhara" && kv.second.rank == Rank::Jaghun) { boss = &kv.second; break; }
    auto subs = n.subordinates(boss->id);
    CHECK(!subs.empty());
    Commander& sub = *w.find(subs[0]->id);
    sub.facet(Facet::Loyalty) = 90;
    sub.goal = {};
    w.cfg.cheatDeathBase = 0; w.cfg.cheatDeathPerRank = 0;
    EncounterReport r; r.commander = boss->id; r.outcome = Outcome::CommanderKilled; r.method = Method::Melee;
    n.resolve(r);
    CHECK(!boss->alive);
    CHECK(sub.hate >= w.cfg.hateVengefulAt);
    CHECK(sub.goal.type == GoalType::Avenge);
    bool plan = false; for (auto& p : w.plans) plan |= p.leader == sub.id && p.kind == "revenge_ambush";
    CHECK(plan);
}

TEST(succession_fills_the_seat) {
    World w; Nemesis n(w); n.seed(11);
    Commander* gen = nullptr;
    for (auto& kv : w.commanders) if (kv.second.region == "toyotama" && kv.second.rank == Rank::Tumen) { gen = &kv.second; break; }
    CommanderId genId = gen->id;
    n.kill(*gen, "test", true);
    CHECK(n.seatsUsed("toyotama", Rank::Tumen) == 1);   // reserved while pending
    n.advanceDays(w.cfg.successionMaxDays + 4);
    int living = 0; CommanderId heir;
    for (auto& kv : w.commanders) if (kv.second.alive && kv.second.region == "toyotama" && kv.second.rank == Rank::Tumen) { ++living; heir = kv.first; }
    CHECK(living == 1);
    CHECK(heir != genId);
    // nobody still reports to the dead general
    for (auto& kv : w.commanders) if (kv.second.alive) CHECK(kv.second.superior != genId);
}

// ------------------------------------------------------------------ world
TEST(camp_liberation_triggers_retaliation) {
    World w; Nemesis n(w); n.seed(13);
    int before = w.regions["izuhara"].control;
    n.onCampLiberated("izuhara", "Komatsu Forge", 12);
    CHECK(w.regions["izuhara"].control < before);
    bool plan = false; for (auto& p : w.plans) plan |= p.kind == "retaliation" && p.region == "izuhara";
    CHECK(plan);
}

TEST(world_keeps_moving_without_jin) {
    World w; Nemesis n(w); n.seed(17);
    w.cfg.rivalryClashPerDayPct = 50;
    size_t ev = w.events.size();
    n.advanceDays(60);
    CHECK(w.day == 60);
    CHECK(w.events.size() > ev);
    for (auto& kv : w.commanders) if (kv.second.storyProtected) CHECK(kv.second.alive);
}

TEST(set_act_unlocks_regions) {
    World w; Nemesis n(w); n.seed(1, 1);
    CHECK(!w.regions["toyotama"].unlocked);
    n.setAct(2);
    CHECK(w.regions["toyotama"].unlocked && w.regions["iki"].unlocked && !w.regions["kamiagata"].unlocked);
}

// ------------------------------------------------------------------ persistence & facade
TEST(save_load_roundtrip_and_backup_recovery) {
    std::string dir = tmpDir("save");
    World w; Nemesis n(w); n.seed(21);
    w.cfg.cheatDeathAlways = true;
    Commander& c = anyArban(w);
    EncounterReport r; r.commander = c.id; r.outcome = Outcome::CommanderKilled; r.method = Method::Fire; r.damageTaken[Wound::Burn] = 50;
    n.resolve(r);
    SaveManager sm(dir);
    CHECK(sm.save(w, "slot1"));
    CHECK(sm.save(w, "slot1"));   // creates .bak1
    auto loaded = sm.load("slot1");
    CHECK(loaded.has_value());
    CHECK(loaded->toJson().dump() == w.toJson().dump());
    // RNG continues identically after load
    CHECK(loaded->rng.range(0, 1000000) == w.rng.range(0, 1000000));
    // corrupt main file -> backup used
    SaveManager::writeAtomic(sm.pathFor("slot1"), "{ this is not json");
    auto rec = sm.load("slot1");
    CHECK(rec.has_value() && rec->find(c.id) && rec->find(c.id)->stats.timesCheatedDeath == 1);
}

TEST(facade_commander_remembers_jin) {
    // The Phase 7 milestone, end to end through the game-facing API.
    std::string dir = tmpDir("facade");
    MockAdapter game; game.quiet = true;
    CommanderId id;
    {
        LivingWar lw(&game, dir);
        lw.start("m1", 42);
        auto p = lw.onLeaderSpawned(1, "izuhara", 2);
        CHECK(p.has_value());
        id = p->commander;
        CHECK(game.applied.count(1) == 1);   // profile was pushed to the game
        lw.onLeaderDamaged(1, Wound::Burn, 50, Method::Fire);
        for (int i = 0; i < 6; ++i) lw.onSoldierKilled(1);
        lw.world().cfg.cheatDeathAlways = true;
        lw.onLeaderKilled(1, Method::Fire);
    }
    {
        MockAdapter g2; g2.quiet = true;
        LivingWar lw(&g2, dir);
        lw.start("m1");
        const Commander* c = lw.world().find(id);
        CHECK(c && c->alive && c->stats.timesCheatedDeath == 1 && c->stats.menLostToJin == 6);
        for (int d = 0; d <= 12; ++d) lw.onGameMinutes(static_cast<int64_t>(d) * 1440);
        auto p = lw.onLeaderSpawned(7, "izuhara", 2);
        CHECK(p.has_value() && p->commander == id);           // he comes back for Jin
        CHECK(p->resist.count(Wound::Burn) || !p->panicTriggers.empty());   // and he remembers the fire
        CHECK(!g2.notifications.empty());                      // the game is told he remembers
    }
}

TEST(hierarchy_html_renders) {
    World w; Nemesis n(w); n.seed(2);
    std::string html = renderHierarchyHtml(w);
    CHECK(html.find("/*DATA*/") == std::string::npos);
    CHECK(html.find("khotun_khan") != std::string::npos);
    CHECK(html.find("</script>") != std::string::npos);
}

TEST(config_file_overrides) {
    Json j = Json::parse(R"({"cheatDeathBase": 90, "finalMethods": ["standoff"], "followersByRank": [0,1,2,3,4,5]})");
    Config c = Config::fromJson(j);
    CHECK(c.cheatDeathBase == 90 && c.finalMethods.size() == 1 && c.finalMethods[0] == Method::Standoff);
    CHECK(c.followersByRank[5] == 5);
    CHECK(Config::fromJson(c.toJson()).toJson().dump() == c.toJson().dump());
}

int main() {
    for (auto& t : tests()) {
        int before = g_fail;
        t.fn();
        std::printf("%s %s\n", g_fail == before ? "[ ok ]" : "[FAIL]", t.name);
    }
    std::printf("\n%d checks passed, %d failed\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
