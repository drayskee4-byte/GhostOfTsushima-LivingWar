#include "lw/world.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "lw/names.hpp"

namespace lw {

// ======================================================================= serialisation
Json PendingSeat::toJson() const {
    Json j; j["fallen"] = fallen; j["fallenName"] = fallenName; j["region"] = region; j["rank"] = toString(rank);
    j["superior"] = superior; j["fillOn"] = fillOn; j["killedByJin"] = killedByJin; return j;
}
PendingSeat PendingSeat::fromJson(const Json& j) {
    PendingSeat s; s.fallen = j.str("fallen"); s.fallenName = j.str("fallenName"); s.region = j.str("region");
    s.rank = fromString<Rank>(j.str("rank")); s.superior = j.str("superior"); s.fillOn = j.i("fillOn");
    s.killedByJin = j.flag("killedByJin"); return s;
}

Commander* World::find(const CommanderId& id) {
    auto it = commanders.find(id);
    return it == commanders.end() ? nullptr : &it->second;
}
const Commander* World::find(const CommanderId& id) const {
    auto it = commanders.find(id);
    return it == commanders.end() ? nullptr : &it->second;
}

Json World::toJson() const {
    Json j;
    j["schemaVersion"] = schemaVersion;
    j["config"] = cfg.toJson();
    j["rngSeed"] = std::to_string(rng.seed());
    j["rngState"] = rng.state();
    j["day"] = day; j["act"] = act; j["nextId"] = nextId;
    Json cs = Json::array(); for (auto& kv : commanders) cs.push(kv.second.toJson()); j["commanders"] = cs;
    Json rs = Json::array(); for (auto& kv : regions) rs.push(kv.second.toJson()); j["regions"] = rs;
    Json ps = Json::array(); for (auto& p : plans) ps.push(p.toJson()); j["plans"] = ps;
    Json es = Json::array(); for (auto& e : events) es.push(e.toJson()); j["events"] = es;
    Json ss = Json::array(); for (auto& s : seats) ss.push(s.toJson()); j["seats"] = ss;
    return j;
}

World World::fromJson(const Json& j) {
    World w;
    w.schemaVersion = j.i("schemaVersion", kSchemaVersion);
    w.cfg = Config::fromJson(j["config"]);
    w.rng.reseed(std::stoull(j.str("rngSeed", "1")));
    if (j.has("rngState") && !w.rng.setState(j.str("rngState"))) Log::warn("save: RNG state unreadable, reseeded");
    w.day = j.i("day"); w.act = j.i("act", 1); w.nextId = j.i("nextId", 1);
    for (auto& c : j["commanders"].elements()) { auto x = Commander::fromJson(c); x.active = false; w.commanders[x.id] = x; }
    for (auto& r : j["regions"].elements()) { auto x = Region::fromJson(r); w.regions[x.id] = x; }
    for (auto& p : j["plans"].elements()) w.plans.push_back(Plan::fromJson(p));
    for (auto& e : j["events"].elements()) w.events.push_back(WorldEvent::fromJson(e));
    for (auto& s : j["seats"].elements()) w.seats.push_back(PendingSeat::fromJson(s));
    return w;
}

// ======================================================================= helpers
namespace {
struct RegionDef { const char* id; const char* name; int act; };
const RegionDef kRegions[] = {
    {"izuhara", "Izuhara", 1}, {"toyotama", "Toyotama", 2}, {"kamiagata", "Kamiagata", 3}, {"iki", "Iki Island", 2},
};
const char* kBodyParts[] = {"face", "jaw", "left eye", "neck", "chest", "left arm", "right arm", "hands", "back", "leg"};

Wound dominantWound(const EncounterReport& r) {
    Wound best = Wound::None; int amt = 0;
    for (auto& kv : r.damageTaken) if (kv.second > amt) { amt = kv.second; best = kv.first; }
    if (best == Wound::None) {
        switch (r.method) {
            case Method::Fire: return Wound::Burn;
            case Method::Ranged: return Wound::Arrow;
            case Method::Explosive: return Wound::Blast;
            case Method::Poison: return Wound::Poison;
            default: return Wound::Cut;
        }
    }
    return best;
}
int dmg(const EncounterReport& r, Wound w) {
    auto it = r.damageTaken.find(w);
    return it == r.damageTaken.end() ? 0 : it->second;
}
std::string scarText(Wound w, const std::string& where) {
    switch (w) {
        case Wound::Burn: return "burn scars across his " + where;
        case Wound::Arrow: return "an arrow wound in his " + where;
        case Wound::Blast: return "shrapnel scars on his " + where;
        case Wound::Poison: return "a sickly pallor and a ruined voice";
        case Wound::Blunt: return "a crooked, badly-set " + where;
        default: return "a long katana scar across his " + where;
    }
}
const char* methodPhrase(Method m) {
    switch (m) {
        case Method::Stealth: return "from the shadows";
        case Method::Standoff: return "in a standoff";
        case Method::Fire: return "with fire";
        case Method::Ranged: return "with arrows";
        case Method::Explosive: return "with black powder";
        case Method::Poison: return "with poison";
        case Method::GhostWeapons: return "with the Ghost's tricks";
        case Method::Mounted: return "from horseback";
        case Method::Melee: return "blade to blade";
        default: return "in battle";
    }
}
} // namespace

std::string Nemesis::newId() {
    char b[16];
    std::snprintf(b, sizeof b, "c%04d", w_.nextId++);
    return b;
}
std::string Nemesis::regionName(const std::string& id) const {
    auto it = w_.regions.find(id);
    return it == w_.regions.end() ? id : it->second.name;
}

void Nemesis::history(Commander& c, const std::string& text) {
    c.history.push_back({w_.day, text});
    if (c.history.size() > 60) c.history.erase(c.history.begin());
}

void Nemesis::remember(Commander& c, const std::string& kind, Emotion e, int intensity, const std::string& text,
                       bool longTerm) {
    c.memories.push_back({w_.day, kind, e, clampi(intensity, 0, 100), longTerm, text});
    if (c.memories.size() > 40) {
        // forget the faintest short-term memory first
        auto it = std::min_element(c.memories.begin(), c.memories.end(), [](const Memory& a, const Memory& b) {
            return (a.longTerm ? 1000 : 0) + a.intensity < (b.longTerm ? 1000 : 0) + b.intensity;
        });
        c.memories.erase(it);
    }
}

void Nemesis::event(const std::string& kind, const std::string& region, std::vector<CommanderId> who,
                    const std::string& text) {
    w_.events.push_back({w_.day, kind, region, std::move(who), text});
    if (w_.events.size() > 500) w_.events.erase(w_.events.begin());
    Log::info("[day " + std::to_string(w_.day) + "] " + text);
}

bool Nemesis::adapt(Commander& c, const std::string& id, const std::string& reason) {
    for (auto& a : c.adaptations) {
        if (a.id == id) {
            if (a.level >= 3) return false;
            ++a.level;
            history(c, "hardened further: " + reason);
            return true;
        }
    }
    c.adaptations.push_back({id, 1, w_.day, reason});
    history(c, "learned from it: " + reason);
    return true;
}

void Nemesis::tie(Commander& a, Commander& b, TieType ab, TieType ba, int strength, const std::string& note) {
    if (!a.tieTo(b.id)) a.ties.push_back({b.id, ab, strength, note});
    if (!b.tieTo(a.id)) b.ties.push_back({a.id, ba, strength, note});
}

// ======================================================================= creation
void Nemesis::rollPersonality(Commander& c, Archetype a) {
    auto& r = w_.rng;
    for (auto& f : c.facets) f = (r.range(10, 90) + r.range(10, 90)) / 2;   // gentle bell curve
    if (a == Archetype::Brute) { c.facet(Facet::Aggression) += 15; c.facet(Facet::Cunning) -= 10; }
    if (a == Archetype::Archer) { c.facet(Facet::Cunning) += 10; c.facet(Facet::Discipline) += 5; }
    if (a == Archetype::Shield) { c.facet(Facet::Discipline) += 15; }
    if (a == Archetype::Shaman) { c.facet(Facet::Cruelty) += 10; c.facet(Facet::Cunning) += 10; }
    for (auto& f : c.facets) f = clampi(f, 0, 100);

    static const std::vector<std::string> strengths = {"ironhide", "marksman", "unyielding", "tactician", "leader", "tracker", "horseman"};
    static const std::vector<std::string> weaknesses = {"hothead", "drunkard", "arrogant", "coward", "superstitious"};
    static const std::vector<std::string> fears = {"fire", "assassination", "the_ghost"};
    int ns = r.range(0, 100) < 55 ? 1 : (r.range(0, 100) < 25 ? 2 : 0);
    for (int i = 0; i < ns; ++i) {
        std::string s = r.pick(strengths);
        if (s == "marksman" && a != Archetype::Archer && r.chance(60)) s = "ironhide";
        c.strengths.insert(s);
    }
    if (r.chance(55)) c.weaknesses.insert(r.pick(weaknesses));
    if (c.facet(Facet::Aggression) > 75) c.weaknesses.insert("hothead");
    if (c.facet(Facet::Courage) < 22) c.weaknesses.insert("coward");
    if (c.facet(Facet::Pride) > 80) c.weaknesses.insert("arrogant");
    if (r.chance(15)) c.fears.insert(r.pick(fears));
    if (c.weaknesses.count("superstitious") && r.chance(50)) c.fears.insert("the_ghost");

    // Starting ambition. [enRAGE] goal types Rise / Settle / Prove ...
    if (c.facet(Facet::Ambition) > 70) c.goal = {GoalType::Rise, "", w_.day, "means to climb the ranks of the Khan's army"};
    else if (c.facet(Facet::Courage) < 25) c.goal = {GoalType::Survive, "", w_.day, "wants to survive this war and go home"};
}

Commander& Nemesis::create(const std::string& region, Rank rank, const CommanderId& superior,
                           std::optional<Archetype> arch) {
    auto& r = w_.rng;
    Commander c;
    c.id = newId();
    static const std::vector<Archetype> pool = {Archetype::Swordsman, Archetype::Swordsman, Archetype::Spearman,
                                                Archetype::Shield, Archetype::Brute, Archetype::Archer};
    Archetype a = arch ? *arch : (region == "iki" && r.chance(25) ? Archetype::Shaman : r.pick(pool));
    std::set<std::string> taken;
    for (auto& kv : w_.commanders) if (kv.second.alive) taken.insert(kv.second.name);
    Identity id = makeIdentity(r, a, taken);
    c.name = id.name; c.origin = id.origin; c.appearance = id.appearance;
    c.archetype = a; c.rank = rank; c.region = region; c.superior = superior;
    c.renown = 10 * static_cast<int>(rank) + r.range(0, 9);
    rollPersonality(c, a);
    history(c, "serves as " + rankTitle(rank) + " in " + regionName(region) + ", from " + c.origin);
    auto& ref = w_.commanders[c.id] = c;
    if (auto* sup = w_.find(superior)) tie(ref, *sup, TieType::Superior, TieType::Subordinate, 30, "chain of command");
    return ref;
}

void Nemesis::seed(uint64_t seed, int act) {
    w_ = World();
    w_.rng.reseed(seed);
    w_.act = act;
    for (auto& d : kRegions) w_.regions[d.id] = Region{d.id, d.name, act >= d.act, d.id == std::string("iki") ? 90 : 85, 0, 0};

    // Story figures sit above the simulated hierarchy and are never touched by it.
    Commander khan; khan.id = "khotun_khan"; khan.name = "Khotun Khan"; khan.rank = Rank::Noyan;
    khan.storyProtected = true; khan.region = "tsushima"; khan.archetype = Archetype::Swordsman;
    w_.commanders[khan.id] = khan;
    Commander eagle; eagle.id = "the_eagle"; eagle.name = "The Eagle"; eagle.rank = Rank::Noyan;
    eagle.storyProtected = true; eagle.region = "iki"; eagle.archetype = Archetype::Shaman;
    w_.commanders[eagle.id] = eagle;

    auto& noyan = create("izuhara", Rank::Noyan, khan.id);
    history(noyan, "chosen by Khotun Khan to hold Tsushima");
    for (auto& d : kRegions) {
        CommanderId top = d.id == std::string("iki") ? CommanderId("the_eagle") : noyan.id;
        auto& gen = create(d.id, Rank::Tumen, top);
        std::vector<CommanderId> ming, jag;
        for (int i = 0; i < 2; ++i) ming.push_back(create(d.id, Rank::Mingghan, gen.id).id);
        for (int i = 0; i < 4; ++i) jag.push_back(create(d.id, Rank::Jaghun, ming[i % 2]).id);
        for (int i = 0; i < 6; ++i) create(d.id, Rank::Arban, jag[i % 4]);
    }
    // Rivalries, brotherhoods and kin between peers.
    std::vector<CommanderId> ids;
    for (auto& kv : w_.commanders) if (!kv.second.storyProtected) ids.push_back(kv.first);
    for (size_t i = 0; i < ids.size(); ++i) {
        for (size_t k = i + 1; k < ids.size(); ++k) {
            auto& a = w_.commanders[ids[i]];
            auto& b = w_.commanders[ids[k]];
            if (a.region != b.region) continue;
            int rd = std::abs(static_cast<int>(a.rank) - static_cast<int>(b.rank));
            if (rd == 0 && w_.rng.chance(18)) tie(a, b, TieType::Rival, TieType::Rival, -40, "both want the same seat");
            else if (rd <= 1 && w_.rng.chance(6)) tie(a, b, TieType::BloodBrother, TieType::BloodBrother, 70, "rode together on the steppe");
            else if (w_.rng.chance(3)) tie(a, b, TieType::Kin, TieType::Kin, 80, "brothers");
        }
    }
    event("seed", "", {}, "The Mongol occupation of Tsushima begins. " + std::to_string(ids.size()) + " officers hold the island.");
}

void Nemesis::setAct(int act) {
    w_.act = act;
    for (auto& d : kRegions) {
        auto& r = w_.regions[d.id];
        bool was = r.unlocked;
        r.unlocked = act >= d.act;
        if (!was && r.unlocked) event("region", d.id, {}, std::string(d.name) + " is now open to the Ghost.");
    }
}

// ======================================================================= ranks
void Nemesis::gainXp(Commander& c, int amount, const std::string& reason) {
    // [enRAGE] GainRankXp: no XP for the dead, the capped or those who want out ("Settle").
    if (!c.alive || amount <= 0 || static_cast<int>(c.rank) >= kMaxRank || c.storyProtected) return;
    if (c.goal.type == GoalType::Survive) return;
    double mult = 0.5 + c.facet(Facet::Ambition) / 100.0;
    if (c.goal.type == GoalType::Rise) mult *= w_.cfg.riseGoalXpMult;
    int gain = std::max(1, static_cast<int>(std::lround(amount * mult)));
    c.rankXp = std::min(c.rankXp + gain, xpNeed(c.rank));
    Log::debug(c.name + " +" + std::to_string(gain) + " xp (" + reason + ")");
    tryRankUp(c, reason);
}

int Nemesis::seatsUsed(const std::string& region, Rank rank) const {
    int n = 0;
    for (auto& kv : w_.commanders) {
        auto& c = kv.second;
        if (!c.alive || c.storyProtected || c.rank != rank) continue;
        if (rank == Rank::Noyan || c.region == region) ++n;
    }
    for (auto& s : w_.seats) if (s.rank == rank && (rank == Rank::Noyan || s.region == region)) ++n;  // reserved
    return n;
}
bool Nemesis::seatFree(const std::string& region, Rank rank) const {
    int cap = w_.cfg.seatsPerRegion[std::min<size_t>(static_cast<int>(rank), w_.cfg.seatsPerRegion.size() - 1)];
    return seatsUsed(region, rank) < cap;
}

bool Nemesis::tryRankUp(Commander& c, const std::string& reason) {
    if (static_cast<int>(c.rank) >= kMaxRank || c.rankXp < xpNeed(c.rank)) return false;
    if (c.goal.type == GoalType::Survive || c.presumedDead || !c.alive) return false;
    Rank next = static_cast<Rank>(static_cast<int>(c.rank) + 1);
    if (!seatFree(c.region, next)) return false;   // waits for a seat - see rivalries
    c.rankXp -= xpNeed(c.rank);                    // [enRAGE] TryRankUp carries the remainder
    c.rank = next;
    c.renown += 10;
    // Re-attach above his old superior if he now matches him.
    if (auto* sup = w_.find(c.superior)) {
        if (static_cast<int>(sup->rank) <= static_cast<int>(c.rank) && !sup->storyProtected) c.superior = sup->superior;
    }
    history(c, "rose to " + rankTitle(c.rank) + " - " + reason);
    remember(c, "promoted", Emotion::Pride, 45, "made a name for himself: " + rankTitle(c.rank));   // [enRAGE]
    event("promotion", c.region, {c.id}, c.name + " is getting a name for himself: now " + rankTitle(c.rank) + ".");
    return true;
}

// ======================================================================= encounters
void Nemesis::learnFrom(Commander& c, const EncounterReport& r, EncounterResult& out) {
    auto learned = [&](const std::string& id, const std::string& why) {
        if (adapt(c, id, why)) out.newAdaptations.push_back(id);
    };
    const bool brave = c.facet(Facet::Courage) >= 50;
    if (r.method == Method::Stealth || !r.detectedJin) {
        learned("paranoid", "never saw Jin coming");
        if (!brave) c.fears.insert("assassination");
        if (c.epithet.empty()) c.epithet = epithetFor("paranoid", 1);
    }
    if (dmg(r, Wound::Burn) >= 20 || r.method == Method::Fire) {
        if (brave) learned("fire_ward", "was burned and swore it would not happen again");
        else { learned("fire_fear", "was burned and now dreads the flame"); c.fears.insert("fire"); }
        c.epithet = epithetFor("burned", 1);
    }
    if (dmg(r, Wound::Arrow) >= 25 || r.method == Method::Ranged) {
        learned("shield_wall", "was cut down by Jin's arrows");
    }
    if (r.method == Method::Standoff) learned("standoff_wary", "was humbled in a standoff");
    if (dmg(r, Wound::Blast) >= 20 || r.method == Method::Explosive) learned("spread_out", "lost men to black powder");
    if (dmg(r, Wound::Poison) >= 10 || r.method == Method::Poison) learned("poison_wary", "survived Jin's poison");
    if (r.method == Method::Mounted) learned("horse_wary", "was ridden down");
    if (r.method == Method::GhostWeapons && c.facet(Facet::Courage) < 40) c.fears.insert("the_ghost");
}

void Nemesis::addPlan(const std::string& kind, Commander& leader, int minDays, int maxDays, int strength,
                      const std::string& text, EncounterResult* out) {
    for (auto& p : w_.plans) if (!p.done && p.leader == leader.id && p.kind == kind) return;  // one at a time
    Plan p;
    p.id = "p" + std::to_string(w_.nextId++);
    p.kind = kind; p.leader = leader.id; p.region = leader.region;
    p.readyOn = std::max(w_.day, leader.presumedDead ? leader.recoverUntil : w_.day) + w_.rng.range(minDays, maxDays);
    p.expiresOn = p.readyOn + w_.cfg.planLifetimeDays;
    p.strength = strength; p.text = text;
    w_.plans.push_back(p);
    history(leader, text);
    if (out) out->plansCreated.push_back(p.id);
}

EncounterResult Nemesis::resolve(const EncounterReport& r) {
    EncounterResult out;
    Commander* cp = w_.find(r.commander);
    if (!cp) { Log::warn("resolve: unknown commander " + r.commander); return out; }
    Commander& c = *cp;
    if (!c.alive) { Log::warn("resolve: " + c.name + " is already dead"); return out; }
    const auto& cfg = w_.cfg;
    const Rank rankBefore = c.rank;
    c.active = false;
    c.stats.encounters++;
    c.lastSeenByJin = w_.day;
    std::string region = r.region.empty() ? c.region : r.region;
    if (w_.regions.count(region)) w_.regions[region].pressure += 5;
    std::string where = r.place.empty() ? regionName(region) : r.place;

    // Close out any plan he was executing.
    for (auto& p : w_.plans) if (!p.done && p.leader == c.id && p.readyOn <= w_.day) p.done = true;

    // Men lost - the "camp massacre" grudge.
    if (r.subordinatesKilled > 0) {
        c.stats.menLostToJin += r.subordinatesKilled;
        c.hate = clampi(c.hate + std::min(cfg.hateMenLostCap, r.subordinatesKilled * cfg.hatePerManLost), 0, 100);
        if (r.subordinatesKilled >= 5 || r.campLiberated) {
            remember(c, "massacre", Emotion::Rage, 80, fmt("watched the Ghost cut down %d of his men at %s", r.subordinatesKilled, where.c_str()), true);
            c.goal = {GoalType::Avenge, "jin", w_.day, "make the Ghost pay for the men he butchered"};
            adapt(c, "vengeful", "survived the slaughter of his men");
        }
    }

    switch (r.outcome) {
    case Outcome::CommanderKilled: {
        c.stats.defeatsByJin++;
        bool final = std::find(cfg.finalMethods.begin(), cfg.finalMethods.end(), r.method) != cfg.finalMethods.end();
        // [enRAGE] cheat-death chance = min(max, base + perRank * (rank-1))
        int chance = std::min(cfg.cheatDeathMax, cfg.cheatDeathBase + cfg.cheatDeathPerRank * (static_cast<int>(c.rank) - 1));
        auto ms = cfg.methodSurvival.find(r.method);
        if (ms != cfg.methodSurvival.end()) chance = static_cast<int>(std::lround(chance * ms->second));
        bool survives = !final && (cfg.cheatDeathAlways || w_.rng.chance(chance));
        if (c.storyProtected) survives = true;
        if (!survives) {
            out.died = true;
            kill(c, std::string("slain by Jin Sakai ") + methodPhrase(r.method) + " at " + where, true, r.method);
            out.lines.push_back(c.fullTitle() + " is dead.");
            break;
        }
        out.cheatedDeath = true;
        Wound w = dominantWound(r);
        std::string part = kBodyParts[w_.rng.range(0, 9)];
        c.scars.push_back({w_.day, w, part, true, scarText(w, part)});
        c.stats.timesCheatedDeath++;
        c.presumedDead = true;
        c.recoverUntil = w_.day + w_.rng.range(cfg.recoverMinDays, cfg.recoverMaxDays);
        c.hate = clampi(c.hate + cfg.hateDefeated + 10, 0, 100);
        c.fearOfJin = clampi(c.fearOfJin + cfg.fearDefeated, 0, 100);
        remember(c, "cheated_death", Emotion::Rage, 90,
                 std::string("left for dead by Jin ") + methodPhrase(r.method) + " at " + where + " - carries " + scarText(w, part), true);
        history(c, std::string("cut down by Jin ") + methodPhrase(r.method) + " at " + where + ", but did not die");
        learnFrom(c, r, out);
        if (c.epithet.empty() || c.stats.timesCheatedDeath >= 2) c.epithet = epithetFor("cheated_death", c.stats.timesCheatedDeath);
        c.goal = {GoalType::Avenge, "jin", w_.day, "settle it with the Ghost, for good"};
        gainXp(c, cfg.xpCheatedDeath, "cheated death - carries your scar");   // [enRAGE]
        int strength = 1 + static_cast<int>(c.rank) / 2;
        addPlan(c.hate >= cfg.hateVengefulAt ? "revenge_ambush" : "return", c, 0, 2, strength,
                "swore to find the Ghost again once his wounds close", &out);
        out.lines.push_back(c.name + " survived. He will be back.");
        event("cheated_death", region, {c.id}, "Rumour has it " + c.name + " survived his wounds at " + where + ".");
        break;
    }
    case Outcome::CommanderFled: {
        c.stats.timesFled++;
        c.hate = clampi(c.hate + cfg.hateFled, 0, 100);
        c.fearOfJin = clampi(c.fearOfJin + cfg.fearFled, 0, 100);
        learnFrom(c, r, out);
        gainXp(c, cfg.xpSurvived, "lived to fight again");
        if (c.facet(Facet::Pride) > 55) {
            remember(c, "fled_shame", Emotion::Shame, 75, "ran from the Ghost at " + where + " in front of his men", true);
            c.goal = {GoalType::RestoreHonor, "jin", w_.day, "wipe out the shame of running from the Ghost"};
            adapt(c, "ambusher", "will not face Jin openly again");
            addPlan("revenge_ambush", c, cfg.revengeMinDays, cfg.revengeMaxDays, 1 + static_cast<int>(c.rank) / 2,
                    "plans an ambush to restore his honour", &out);
            out.lines.push_back(c.name + " is humiliated and plotting an ambush.");
        } else {
            remember(c, "fled_fear", Emotion::Fear, 70, "barely escaped the Ghost at " + where);
            if (c.fearOfJin >= 60 && c.goal.type != GoalType::Avenge)
                c.goal = {GoalType::Survive, "", w_.day, "get away from the Ghost - out of this war"};
            if (c.stats.timesFled >= 2 && c.epithet.empty()) c.epithet = epithetFor("fled", 2);
            out.lines.push_back(c.name + " escaped, shaken.");
        }
        history(c, "fled from Jin at " + where);
        break;
    }
    case Outcome::CommanderSpared: {
        c.stats.timesSpared++;
        c.fearOfJin = clampi(c.fearOfJin + cfg.fearSpared, 0, 100);
        if (c.facet(Facet::Pride) > 60) {
            c.hate = clampi(c.hate + 10, 0, 100);
            remember(c, "spared_shame", Emotion::Shame, 80, "was spared by the Ghost at " + where + " - a mercy he cannot forgive", true);
            c.goal = {GoalType::RestoreHonor, "jin", w_.day, "prove the Ghost's mercy was a mistake"};
            addPlan("revenge_ambush", c, cfg.revengeMinDays, cfg.revengeMaxDays, 1, "means to repay the Ghost's mercy in blood", &out);
        } else {
            c.hate = clampi(c.hate + cfg.hateSpared, 0, 100);
            remember(c, "spared_respect", Emotion::Respect, 60, "was spared by the Ghost at " + where, true);
            if (c.epithet.empty()) c.epithet = epithetFor("spared", 1);
        }
        history(c, "spared by Jin at " + where);
        out.lines.push_back(c.name + " was spared.");
        break;
    }
    case Outcome::JinKilled: {
        c.stats.victoriesOverJin++;
        c.fearOfJin = clampi(c.fearOfJin + cfg.fearKilledJin, 0, 100);
        c.renown += 15;
        c.hate = clampi(c.hate - 10, 0, 100);
        remember(c, "slew_jin", Emotion::Triumph, 90, "struck down the Ghost at " + where, true);
        history(c, "defeated Jin Sakai at " + where);
        adapt(c, "emboldened", "has beaten the Ghost and believes he can do it again");
        c.epithet = epithetFor("slew_jin", c.stats.victoriesOverJin);
        gainXp(c, cfg.xpKilledJin, "killed you");   // [enRAGE]
        if (c.goal.type == GoalType::Avenge || c.goal.type == GoalType::RestoreHonor)
            c.goal = {GoalType::Rise, "", w_.day, "ride his victory over the Ghost to higher command"};
        event("victory", region, {c.id}, c.name + " boasts that he has beaten the Ghost.");
        out.lines.push_back(c.name + " defeated Jin and grows bolder.");
        break;
    }
    case Outcome::JinFled: {
        c.fearOfJin = clampi(c.fearOfJin - 10, 0, 100);
        c.renown += 5;
        remember(c, "jin_fled", Emotion::Pride, 60, "drove the Ghost off at " + where);
        history(c, "drove Jin off at " + where);
        gainXp(c, cfg.xpJinFled, "drove off the Ghost");
        out.lines.push_back(c.name + " drove Jin away.");
        break;
    }
    case Outcome::Inconclusive:
        remember(c, "glimpsed", Emotion::Fear, 30, "crossed paths with the Ghost at " + where);
        break;
    }
    out.rankedUp = c.alive && c.rank != rankBefore;
    if (out.rankedUp) out.lines.push_back(c.name + " rose to " + rankTitle(c.rank) + ".");
    return out;
}

void Nemesis::kill(Commander& c, const std::string& cause, bool byJin, Method m) {
    if (!c.alive || c.storyProtected) return;
    c.alive = false;
    c.presumedDead = false;
    c.active = false;
    c.fate = cause;
    c.diedOn = w_.day;
    history(c, cause);
    for (auto& p : w_.plans) if (p.leader == c.id) p.done = true;
    event(byJin ? "death_jin" : "death", c.region, {c.id}, c.fullTitle() + " has died - " + cause + ".");

    // Ripple through his ties: subordinates seek revenge, brothers and kin grieve, rivals rejoice.
    for (auto& kv : w_.commanders) {
        Commander& o = kv.second;
        if (!o.alive || o.storyProtected || o.id == c.id) continue;
        const Tie* t = o.tieTo(c.id);
        if (!t) continue;
        int add = 0; Emotion e = Emotion::Grief; std::string text;
        switch (t->type) {
            case TieType::Superior: case TieType::Mentor:      // c was o's superior
                add = byJin ? w_.cfg.hateSuperiorKilled * o.facet(Facet::Loyalty) / 60 : 0;
                text = "lost his commander " + c.name; break;
            case TieType::Kin:
                add = byJin ? w_.cfg.hateKinKilled : 0; text = "lost his kinsman " + c.name; break;
            case TieType::BloodBrother:
                add = byJin ? w_.cfg.hateBrotherKilled : 0; text = "lost his blood brother " + c.name; break;
            case TieType::Rival:
                e = Emotion::Pride; text = "outlived his rival " + c.name; break;
            default:
                add = byJin ? 8 : 0; text = "lost " + c.name; break;
        }
        remember(o, "loss", e, e == Emotion::Grief ? 70 : 40, text + (byJin ? " to the Ghost" : ""), add > 20);
        if (add <= 0) continue;
        o.hate = clampi(o.hate + add, 0, 100);
        if (o.hate >= w_.cfg.hateVengefulAt && o.goal.type != GoalType::Survive) {
            o.goal = {GoalType::Avenge, "jin", w_.day, "avenge " + c.name};
            adapt(o, "vengeful", "swore vengeance for " + c.name);
            addPlan("revenge_ambush", o, w_.cfg.revengeMinDays, w_.cfg.revengeMaxDays, 1 + static_cast<int>(o.rank) / 2,
                    "hunts the Ghost to avenge " + c.name);
            event("vow", o.region, {o.id, c.id}, o.name + " has sworn to avenge " + c.name + ".");
        }
    }
    if (w_.cfg.successionMaxDays > 0) scheduleSeat(c, byJin);
    (void)m;
}

// ======================================================================= succession
void Nemesis::scheduleSeat(const Commander& fallen, bool byJin) {
    PendingSeat s;
    s.fallen = fallen.id; s.fallenName = fallen.name; s.region = fallen.region; s.rank = fallen.rank;
    s.superior = fallen.superior; s.killedByJin = byJin;
    s.fillOn = w_.day + w_.rng.range(w_.cfg.successionMinDays, w_.cfg.successionMaxDays);
    w_.seats.push_back(s);
}

void Nemesis::fillSeat(const PendingSeat& s) {
    // Candidates: one rank below, same region, prefer the fallen man's own subordinates.
    std::vector<Commander*> cands;
    Commander* kin = nullptr;
    for (auto& kv : w_.commanders) {
        Commander& o = kv.second;
        if (!o.available(w_.day) || o.storyProtected || o.goal.type == GoalType::Survive) continue;
        if (static_cast<int>(o.rank) != static_cast<int>(s.rank) - 1) continue;
        if (s.rank != Rank::Noyan && o.region != s.region) continue;
        bool movedToday = false;   // one step up the ladder per day
        for (auto& m : o.memories) movedToday |= (m.kind == "succeeded" || m.kind == "promoted") && m.day == w_.day;
        if (movedToday) continue;
        cands.push_back(&o);
        if (const Tie* t = o.tieTo(s.fallen); t && t->type == TieType::Kin) kin = &o;
    }
    auto score = [&](const Commander* o) {
        int sc = o->rankXp * 10 + o->renown + o->facet(Facet::Ambition) / 2;
        if (o->superior == s.fallen) sc += 15;
        return sc;
    };
    Commander* heir = nullptr;
    if (kin && w_.rng.chance(w_.cfg.successionKinChance)) heir = kin;   // [enRAGE] SuccessionKinChance
    if (!heir && !cands.empty()) {
        std::sort(cands.begin(), cands.end(), [&](auto* a, auto* b) { return score(a) > score(b); });
        heir = cands[0];
        if (cands.size() > 1 && cands[1]->tieTo(heir->id) && cands[1]->tieTo(heir->id)->type == TieType::Rival) {
            Commander& loser = *cands[1];
            loser.tieTo(heir->id)->strength -= 20;
            remember(loser, "passed_over", Emotion::Rage, 70, "was passed over for " + heir->name);
        }
    }
    Rank oldRank = heir ? heir->rank : Rank::Arban;
    if (!heir) {
        // Nobody fit to step up: the Khan sends a replacement from the mainland.
        heir = &create(s.region, s.rank, s.superior);
        history(*heir, "arrived from the mainland to replace " + s.fallenName);
    } else {
        heir->rank = s.rank;
        heir->rankXp = 0;
        heir->superior = s.superior;
        heir->renown += 10;
        history(*heir, "took " + s.fallenName + "'s place as " + rankTitle(s.rank));
        remember(*heir, "succeeded", Emotion::Pride, 55, "took command after " + s.fallenName + " fell");
        if (s.killedByJin && heir->facet(Facet::Loyalty) > 50) heir->hate = clampi(heir->hate + 15, 0, 100);
        // His own old seat now needs filling too (cascade), unless it was the bottom rung.
        if (static_cast<int>(oldRank) >= kMinRank) {
            PendingSeat next = s;
            next.fallen = heir->id; next.fallenName = heir->name; next.rank = oldRank;
            next.superior = heir->id; next.killedByJin = false;
            next.fillOn = w_.day + w_.rng.range(1, 3);
            if (oldRank == Rank::Arban) {
                create(s.region, Rank::Arban, heir->id);   // fresh blood at the bottom
            } else {
                w_.seats.push_back(next);
            }
        }
    }
    // Re-link the fallen man's subordinates.
    for (auto& kv : w_.commanders) {
        Commander& o = kv.second;
        if (o.alive && o.superior == s.fallen) {
            o.superior = heir->id;
            if (!o.tieTo(heir->id)) tie(o, *heir, TieType::Superior, TieType::Subordinate, 20, "new commander");
        }
    }
    const Commander* prev = w_.find(s.fallen);
    bool fallenAlive = prev && prev->alive;   // cascade: the previous holder was promoted, not killed
    event("succession", s.region, {heir->id, s.fallen},
          heir->name + " is the new " + rankTitle(s.rank) + " of " + (s.rank == Rank::Noyan ? std::string("Tsushima") : regionName(s.region)) +
              (fallenAlive ? ", taking over from the newly promoted " : ", replacing the late ") + s.fallenName + ".");
}

// ======================================================================= offers / camps
std::optional<Offer> Nemesis::offerFor(const std::string& region, int maxRank, bool allowNew) {
    auto rit = w_.regions.find(region);
    if (rit == w_.regions.end()) { Log::warn("offerFor: unknown region " + region); return std::nullopt; }
    auto make = [&](Commander& c, const std::string& planId, const std::string& kind) {
        Offer o{c.id, planId, kind, buildProfile(c, w_.cfg)};
        return o;
    };
    // 1) a ready plan in this region - revenge ambushes, returns, retaliation
    for (auto& p : w_.plans) {
        if (p.done || p.region != region || p.readyOn > w_.day || p.expiresOn < w_.day) continue;
        Commander* c = w_.find(p.leader);
        if (!c || !c->available(w_.day) || c->active) continue;
        Offer o = make(*c, p.id, "plan:" + p.kind);
        for (int i = 1; i < p.strength; ++i) o.profile.retinue.push_back({Archetype::Swordsman, 2, "plan_extra"});
        return o;
    }
    // 2) a hunter: murderous/vengeful officers come looking (30%)
    std::vector<Commander*> pool, hunters;
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (c.storyProtected || !c.available(w_.day) || c.active || c.region != region) continue;
        if (static_cast<int>(c.rank) > maxRank) {
            if (c.hate >= w_.cfg.hateMurderousAt) hunters.push_back(&c);   // the truly obsessed break rank to hunt
            continue;
        }
        pool.push_back(&c);
        if (c.hate >= w_.cfg.hateVengefulAt && c.goal.type == GoalType::Avenge) hunters.push_back(&c);
    }
    if (!hunters.empty() && w_.rng.chance(30)) return make(*w_.rng.pick(hunters), "", "hunter");
    // 3) a regular officer, weighted toward the lower ranks
    if (!pool.empty()) {
        std::vector<Commander*> weighted;
        for (auto* c : pool) {
            int wgt = std::max(1, 6 - 2 * static_cast<int>(c->rank) + (c->stats.encounters > 0 ? 2 : 0));
            for (int i = 0; i < wgt; ++i) weighted.push_back(c);
        }
        return make(*w_.rng.pick(weighted), "", "regular");
    }
    // 4) nobody free: a new face, if the army has room
    if (!allowNew) return std::nullopt;
    int living = 0;
    for (auto& kv : w_.commanders) living += kv.second.alive && !kv.second.storyProtected;
    if (living >= w_.cfg.maxLivingCommanders) return std::nullopt;
    CommanderId sup;
    for (auto& kv : w_.commanders)
        if (kv.second.alive && kv.second.region == region && kv.second.rank == Rank::Jaghun) { sup = kv.first; break; }
    Commander& c = create(region, Rank::Arban, sup);
    return make(c, "", "new");
}

void Nemesis::markActive(const CommanderId& id, bool active) {
    if (auto* c = w_.find(id)) c->active = active;
}

void Nemesis::onCampLiberated(const std::string& region, const std::string& camp, int menKilled, const CommanderId& leader) {
    auto it = w_.regions.find(region);
    if (it == w_.regions.end()) return;
    Region& r = it->second;
    r.control = clampi(r.control - w_.cfg.controlLossPerCamp, 0, 100);
    r.pressure += 20;
    r.alert = clampi(r.alert + 25, 0, 100);
    event("camp_liberated", region, {}, "Jin liberated " + camp + " (" + std::to_string(menKilled) + " Mongols dead). " +
                                           r.name + " control: " + std::to_string(r.control) + "%.");
    // The local chain of command takes it personally; the most senior available officer retaliates.
    Commander* avenger = nullptr;
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (c.storyProtected || !c.alive || c.region != region) continue;
        if (c.id == leader) continue;
        if (static_cast<int>(c.rank) >= static_cast<int>(Rank::Jaghun)) c.hate = clampi(c.hate + w_.cfg.hateCampLiberated / 2, 0, 100);
        if (c.available(w_.day) && c.rank == Rank::Mingghan && (!avenger || c.hate > avenger->hate)) avenger = &c;
    }
    if (auto* l = w_.find(leader); l && l->alive) {
        l->hate = clampi(l->hate + w_.cfg.hateCampLiberated, 0, 100);
        remember(*l, "camp_lost", Emotion::Shame, 70, "lost " + camp + " to the Ghost", true);
    }
    if (avenger) {
        addPlan("retaliation", *avenger, w_.cfg.retaliationMinDays, w_.cfg.retaliationMaxDays, 2,
                "leads a retaliation for the fall of " + camp);
        event("retaliation", region, {avenger->id}, avenger->name + " has sworn to retake " + camp + ".");
    }
}

void Nemesis::onPlayerSighted(const std::string& region) {
    auto it = w_.regions.find(region);
    if (it != w_.regions.end()) it->second.alert = clampi(it->second.alert + 5, 0, 100);
}

// ======================================================================= hierarchy
std::vector<const Commander*> Nemesis::subordinates(const CommanderId& id) const {
    std::vector<const Commander*> v;
    for (auto& kv : w_.commanders) if (kv.second.alive && kv.second.superior == id) v.push_back(&kv.second);
    std::sort(v.begin(), v.end(), [](auto* a, auto* b) { return static_cast<int>(a->rank) > static_cast<int>(b->rank); });
    return v;
}
std::vector<const Commander*> Nemesis::chainOfCommand(const CommanderId& id) const {
    std::vector<const Commander*> v;
    const Commander* c = w_.find(id);
    for (int guard = 0; c && guard < 16; ++guard) { v.push_back(c); c = w_.find(c->superior); }
    return v;
}
std::vector<const Commander*> Nemesis::livingIn(const std::string& region) const {
    std::vector<const Commander*> v;
    for (auto& kv : w_.commanders)
        if (kv.second.alive && !kv.second.storyProtected && kv.second.region == region) v.push_back(&kv.second);
    return v;
}

// ======================================================================= time
void Nemesis::advanceDays(int n) {
    for (int i = 0; i < n; ++i) { ++w_.day; dailyTick(); }
}

void Nemesis::dailyTick() {
    tickDecay();
    tickRecovery();
    // Fill seats that are due.
    std::vector<PendingSeat> due;
    for (auto it = w_.seats.begin(); it != w_.seats.end();) {
        if (it->fillOn <= w_.day) { due.push_back(*it); it = w_.seats.erase(it); } else ++it;
    }
    for (auto& s : due) fillSeat(s);
    tickRivalries();
    tickPromotions();
    tickPlans();
    tickRegions();
}

void Nemesis::tickDecay() {
    const auto& cfg = w_.cfg;
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (!c.alive || c.storyProtected) continue;
        int floor = c.goal.type == GoalType::Avenge ? cfg.hateVengefulAt : 0;  // a vow keeps the fire lit
        if (c.hate > floor) c.hate = std::max(floor, c.hate - cfg.hateDecayPerDay);
        c.fearOfJin = std::max(0, c.fearOfJin - cfg.fearDecayPerDay);
        for (auto& m : c.memories) if (!m.longTerm) m.intensity = std::max(0, m.intensity - cfg.memoryFadePerDay);
        c.memories.erase(std::remove_if(c.memories.begin(), c.memories.end(),
                                        [](const Memory& m) { return !m.longTerm && m.intensity <= 0; }),
                         c.memories.end());
    }
}

void Nemesis::tickRecovery() {
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (c.alive && c.presumedDead && w_.day >= c.recoverUntil) {
            c.presumedDead = false;
            history(c, "recovered from his wounds");
            event("return", c.region, {c.id}, "Soldiers whisper that " + c.name + " walks again, " +
                  (c.scars.empty() ? std::string("unbowed") : "bearing " + c.scars.back().text) + ".");
        }
    }
}

void Nemesis::tickRivalries() {
    auto& r = w_.rng;
    std::vector<std::pair<CommanderId, CommanderId>> pairs;
    for (auto& kv : w_.commanders) {
        const Commander& a = kv.second;
        if (!a.available(w_.day) || a.storyProtected || a.active) continue;
        for (auto& t : a.ties) {
            if (t.type != TieType::Rival || t.other <= a.id) continue;
            const Commander* b = w_.find(t.other);
            if (b && b->available(w_.day) && !b->active && !b->storyProtected && b->region == a.region) pairs.push_back({a.id, b->id});
        }
    }
    for (auto& pr : pairs) {
        if (!r.chance(w_.cfg.rivalryClashPerDayPct)) continue;
        Commander* a = w_.find(pr.first);
        Commander* b = w_.find(pr.second);
        if (!a || !b || !a->alive || !b->alive) continue;
        auto power = [&](Commander* c) {
            return c->renown + 10 * static_cast<int>(c->rank) + c->facet(Facet::Cunning) / 4 + c->facet(Facet::Aggression) / 4 + r.range(0, 30);
        };
        Commander* win = power(a) >= power(b) ? a : b;
        Commander* lose = win == a ? b : a;
        win->renown += 5; lose->renown = std::max(0, lose->renown - 5);
        if (Tie* t = lose->tieTo(win->id)) t->strength = std::max(-100, t->strength - 15);
        if (w_.cfg.simulationMayKill && r.chance(w_.cfg.rivalryDeathPct)) {
            history(*win, "killed his rival " + lose->name + " in a duel");
            win->epithet = win->epithet.empty() ? epithetFor("rival_slain", 1) : win->epithet;
            kill(*lose, "killed in a duel by his rival " + win->name, false);
            gainXp(*win, w_.cfg.xpDistinguished, "distinguished himself against " + lose->name);   // [enRAGE]
        } else if (r.chance(20) && w_.regions.size() > 1) {
            // Disgraced and sent away.
            std::vector<std::string> ids;
            for (auto& rg : w_.regions) if (rg.first != lose->region) ids.push_back(rg.first);
            std::string to = r.pick(ids);
            if (seatFree(to, lose->rank)) {
                std::string from = lose->region;
                lose->region = to;
                lose->superior.clear();
                for (auto& kv : w_.commanders)
                    if (kv.second.alive && kv.second.region == to && static_cast<int>(kv.second.rank) == static_cast<int>(lose->rank) + 1) { lose->superior = kv.first; break; }
                remember(*lose, "disgraced", Emotion::Shame, 65, "was outmanoeuvred by " + win->name + " and sent to " + regionName(to));
                event("reassigned", to, {lose->id, win->id}, lose->name + " lost a feud with " + win->name + " and was sent from " + regionName(from) + " to " + regionName(to) + ".");
            }
        } else {
            remember(*lose, "feud_lost", Emotion::Shame, 50, "was shamed by " + win->name + " before the men");
            gainXp(*win, 1, "outshone " + lose->name);
            event("feud", win->region, {win->id, lose->id}, win->name + " got the better of his rival " + lose->name + ".");
        }
    }
    // Capped, ambitious officers pick fights with whoever holds the seat above them.
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (!c.available(w_.day) || c.storyProtected || static_cast<int>(c.rank) >= kMaxRank) continue;
        if (c.rankXp < xpNeed(c.rank) || c.facet(Facet::Ambition) < 60 || !r.chance(10)) continue;
        Rank next = static_cast<Rank>(static_cast<int>(c.rank) + 1);
        for (auto& kv2 : w_.commanders) {
            Commander& h = kv2.second;
            if (h.alive && !h.storyProtected && h.rank == next && h.region == c.region && !c.tieTo(h.id)) {
                tie(c, h, TieType::Rival, TieType::Rival, -30, c.name + " covets his command");
                event("rivalry", c.region, {c.id, h.id}, c.name + " covets " + h.name + "'s command.");
                break;
            }
        }
    }
}

void Nemesis::tickPromotions() {
    for (auto& kv : w_.commanders) {
        Commander& c = kv.second;
        if (c.available(w_.day) && !c.storyProtected) tryRankUp(c, "earned his place");
    }
}

void Nemesis::tickPlans() {
    w_.plans.erase(std::remove_if(w_.plans.begin(), w_.plans.end(), [&](const Plan& p) {
                       return p.done && p.expiresOn + 30 < w_.day;   // keep a month of finished plans for the UI
                   }),
                   w_.plans.end());
    for (auto& p : w_.plans) {
        if (!p.done && p.expiresOn < w_.day) {
            p.done = true;
            if (auto* c = w_.find(p.leader)) history(*c, "gave up waiting for the Ghost (" + p.kind + ")");
        }
    }
}

void Nemesis::tickRegions() {
    for (auto& kv : w_.regions) {
        Region& r = kv.second;
        r.pressure = std::max(0, r.pressure - w_.cfg.pressureDecayPerDay);
        r.alert = std::max(0, r.alert - 3);
        if (r.control < 85 && w_.rng.chance(15)) r.control += 1;   // slow re-occupation
        // Heavy pressure: the Noyan sends a hardened officer from the quietest region.
        if (r.pressure >= w_.cfg.reinforceAtPressure) {
            Commander* best = nullptr;
            for (auto& ckv : w_.commanders) {
                Commander& c = ckv.second;
                if (!c.available(w_.day) || c.storyProtected || c.active || c.region == r.id) continue;
                if (c.rank != Rank::Jaghun && c.rank != Rank::Mingghan) continue;
                if (!seatFree(r.id, c.rank)) continue;
                if (!best || c.renown > best->renown) best = &c;
            }
            if (best) {
                std::string from = best->region;
                best->region = r.id;
                for (auto& ckv : w_.commanders)
                    if (ckv.second.alive && ckv.second.region == r.id && static_cast<int>(ckv.second.rank) == static_cast<int>(best->rank) + 1) { best->superior = ckv.first; break; }
                history(*best, "was sent to " + r.name + " to deal with the Ghost");
                event("reinforce", r.id, {best->id}, best->name + " has been sent from " + regionName(from) + " to hunt the Ghost in " + r.name + ".");
                addPlan("hunting_party", *best, 1, 3, 2, "leads a hunting party against the Ghost");
            }
            r.pressure -= 30;
        }
        // High alert: patrols turn into hunting parties.
        if (r.unlocked && r.alert >= 50) {
            bool has = false;
            for (auto& p : w_.plans) has |= (!p.done && p.region == r.id);
            if (!has) {
                Commander* lead = nullptr;
                for (auto& ckv : w_.commanders) {
                    Commander& c = ckv.second;
                    if (c.available(w_.day) && !c.storyProtected && c.region == r.id && c.rank == Rank::Jaghun &&
                        (!lead || c.hate > lead->hate)) lead = &c;
                }
                if (lead) addPlan("hunting_party", *lead, 0, 1, 1, "leads patrols hunting the Ghost");
            }
        }
        // Keep the bottom rung staffed.
        int arbans = seatsUsed(r.id, Rank::Arban);
        if (arbans < w_.cfg.seatsPerRegion[1] - 2 && w_.rng.chance(30)) {
            CommanderId sup;
            for (auto& ckv : w_.commanders)
                if (ckv.second.alive && ckv.second.region == r.id && ckv.second.rank == Rank::Jaghun) { sup = ckv.first; break; }
            create(r.id, Rank::Arban, sup);
        }
    }
}

} // namespace lw
