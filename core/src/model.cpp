#include "lw/model.hpp"

#include <algorithm>
#include <stdexcept>

namespace lw {

// ------------------------------------------------------------ enum tables
namespace {
template <class E, size_t N>
struct EnumTable {
    std::array<std::pair<E, const char*>, N> rows;
    const char* name(E e) const {
        for (auto& r : rows) if (r.first == e) return r.second;
        return "?";
    }
    E value(const std::string& s, E def) const {
        for (auto& r : rows) if (s == r.second) return r.first;
        return def;
    }
};

const EnumTable<Rank, 5> kRank{{{{Rank::Arban, "arban"}, {Rank::Jaghun, "jaghun"}, {Rank::Mingghan, "mingghan"},
                                  {Rank::Tumen, "tumen"}, {Rank::Noyan, "noyan"}}}};
const EnumTable<Archetype, 6> kArch{{{{Archetype::Swordsman, "swordsman"}, {Archetype::Shield, "shield"},
                                      {Archetype::Spearman, "spearman"}, {Archetype::Brute, "brute"},
                                      {Archetype::Archer, "archer"}, {Archetype::Shaman, "shaman"}}}};
const EnumTable<Method, 10> kMethod{{{{Method::Unknown, "unknown"}, {Method::Stealth, "stealth"},
                                      {Method::Standoff, "standoff"}, {Method::Melee, "melee"},
                                      {Method::Ranged, "ranged"}, {Method::Fire, "fire"},
                                      {Method::Explosive, "explosive"}, {Method::Poison, "poison"},
                                      {Method::GhostWeapons, "ghost_weapons"}, {Method::Mounted, "mounted"}}}};
const EnumTable<Wound, 7> kWound{{{{Wound::None, "none"}, {Wound::Cut, "cut"}, {Wound::Burn, "burn"},
                                   {Wound::Arrow, "arrow"}, {Wound::Blast, "blast"}, {Wound::Poison, "poison"},
                                   {Wound::Blunt, "blunt"}}}};
const EnumTable<Outcome, 6> kOutcome{{{{Outcome::CommanderKilled, "commander_killed"},
                                       {Outcome::CommanderFled, "commander_fled"},
                                       {Outcome::CommanderSpared, "commander_spared"},
                                       {Outcome::JinKilled, "jin_killed"}, {Outcome::JinFled, "jin_fled"},
                                       {Outcome::Inconclusive, "inconclusive"}}}};
const EnumTable<Emotion, 7> kEmotion{{{{Emotion::Rage, "rage"}, {Emotion::Fear, "fear"}, {Emotion::Shame, "shame"},
                                       {Emotion::Pride, "pride"}, {Emotion::Grief, "grief"},
                                       {Emotion::Triumph, "triumph"}, {Emotion::Respect, "respect"}}}};
const EnumTable<TieType, 7> kTie{{{{TieType::Superior, "superior"}, {TieType::Subordinate, "subordinate"},
                                   {TieType::Rival, "rival"}, {TieType::BloodBrother, "blood_brother"},
                                   {TieType::Kin, "kin"}, {TieType::Mentor, "mentor"},
                                   {TieType::Protege, "protege"}}}};
const EnumTable<GoalType, 6> kGoal{{{{GoalType::None, "none"}, {GoalType::Avenge, "avenge"},
                                     {GoalType::RestoreHonor, "restore_honor"}, {GoalType::Rise, "rise"},
                                     {GoalType::Protect, "protect"}, {GoalType::Survive, "survive"}}}};
const EnumTable<Facet, 8> kFacet{{{{Facet::Aggression, "aggression"}, {Facet::Courage, "courage"},
                                   {Facet::Pride, "pride"}, {Facet::Cunning, "cunning"},
                                   {Facet::Ambition, "ambition"}, {Facet::Cruelty, "cruelty"},
                                   {Facet::Loyalty, "loyalty"}, {Facet::Discipline, "discipline"}}}};
} // namespace

const char* toString(Rank v) { return kRank.name(v); }
const char* toString(Archetype v) { return kArch.name(v); }
const char* toString(Method v) { return kMethod.name(v); }
const char* toString(Wound v) { return kWound.name(v); }
const char* toString(Outcome v) { return kOutcome.name(v); }
const char* toString(Emotion v) { return kEmotion.name(v); }
const char* toString(TieType v) { return kTie.name(v); }
const char* toString(GoalType v) { return kGoal.name(v); }
const char* toString(Facet v) { return kFacet.name(v); }

template <> Rank fromString<Rank>(const std::string& s) { return kRank.value(s, Rank::Arban); }
template <> Archetype fromString<Archetype>(const std::string& s) { return kArch.value(s, Archetype::Swordsman); }
template <> Method fromString<Method>(const std::string& s) { return kMethod.value(s, Method::Unknown); }
template <> Wound fromString<Wound>(const std::string& s) { return kWound.value(s, Wound::None); }
template <> Outcome fromString<Outcome>(const std::string& s) { return kOutcome.value(s, Outcome::Inconclusive); }
template <> Emotion fromString<Emotion>(const std::string& s) { return kEmotion.value(s, Emotion::Rage); }
template <> TieType fromString<TieType>(const std::string& s) { return kTie.value(s, TieType::Rival); }
template <> GoalType fromString<GoalType>(const std::string& s) { return kGoal.value(s, GoalType::None); }
template <> Facet fromString<Facet>(const std::string& s) { return kFacet.value(s, Facet::Aggression); }

std::string rankTitle(Rank r) {
    switch (r) {
        case Rank::Arban: return "Arban-u Noyan";      // commander of ten
        case Rank::Jaghun: return "Jaghun-u Noyan";    // commander of a hundred
        case Rank::Mingghan: return "Mingghan-u Noyan"; // commander of a thousand
        case Rank::Tumen: return "Tumen-u Noyan";      // general
        case Rank::Noyan: return "Khan's Noyan";       // the Khan's chosen
    }
    return "Officer";
}

// ------------------------------------------------------------ helpers
namespace {
Json strSet(const std::set<std::string>& s) {
    Json a = Json::array();
    for (auto& x : s) a.push(x);
    return a;
}
std::set<std::string> toSet(const Json& j) {
    std::set<std::string> s;
    for (auto& x : j.elements()) s.insert(x.asString());
    return s;
}
template <class T>
Json vec(const std::vector<T>& v) {
    Json a = Json::array();
    for (auto& x : v) a.push(x.toJson());
    return a;
}
template <class T>
std::vector<T> unvec(const Json& j) {
    std::vector<T> v;
    for (auto& x : j.elements()) v.push_back(T::fromJson(x));
    return v;
}
} // namespace

// ------------------------------------------------------------ records
Json Memory::toJson() const {
    Json j; j["day"] = day; j["kind"] = kind; j["emotion"] = toString(emotion);
    j["intensity"] = intensity; j["longTerm"] = longTerm; j["text"] = text; return j;
}
Memory Memory::fromJson(const Json& j) {
    Memory m; m.day = j.i("day"); m.kind = j.str("kind"); m.emotion = fromString<Emotion>(j.str("emotion"));
    m.intensity = j.i("intensity", 50); m.longTerm = j.flag("longTerm"); m.text = j.str("text"); return m;
}

Json Scar::toJson() const {
    Json j; j["day"] = day; j["wound"] = toString(wound); j["location"] = location;
    j["fromJin"] = fromJin; j["text"] = text; return j;
}
Scar Scar::fromJson(const Json& j) {
    Scar s; s.day = j.i("day"); s.wound = fromString<Wound>(j.str("wound")); s.location = j.str("location");
    s.fromJin = j.flag("fromJin", true); s.text = j.str("text"); return s;
}

Json Tie::toJson() const {
    Json j; j["other"] = other; j["type"] = toString(type); j["strength"] = strength; j["note"] = note; return j;
}
Tie Tie::fromJson(const Json& j) {
    Tie t; t.other = j.str("other"); t.type = fromString<TieType>(j.str("type"));
    t.strength = j.i("strength", 50); t.note = j.str("note"); return t;
}

Json Goal::toJson() const {
    Json j; j["type"] = toString(type); j["target"] = target; j["since"] = since; j["text"] = text; return j;
}
Goal Goal::fromJson(const Json& j) {
    Goal g; g.type = fromString<GoalType>(j.str("type", "none")); g.target = j.str("target");
    g.since = j.i("since"); g.text = j.str("text"); return g;
}

Json HistoryEntry::toJson() const { Json j; j["day"] = day; j["text"] = text; return j; }
HistoryEntry HistoryEntry::fromJson(const Json& j) { return {j.i("day"), j.str("text")}; }

Json Adaptation::toJson() const {
    Json j; j["id"] = id; j["level"] = level; j["learned"] = learned; j["reason"] = reason; return j;
}
Adaptation Adaptation::fromJson(const Json& j) {
    Adaptation a; a.id = j.str("id"); a.level = j.i("level", 1); a.learned = j.i("learned");
    a.reason = j.str("reason"); return a;
}

Json EncounterStats::toJson() const {
    Json j;
    j["encounters"] = encounters; j["defeatsByJin"] = defeatsByJin; j["victoriesOverJin"] = victoriesOverJin;
    j["timesFled"] = timesFled; j["timesSpared"] = timesSpared; j["timesCheatedDeath"] = timesCheatedDeath;
    j["menLostToJin"] = menLostToJin;
    return j;
}
EncounterStats EncounterStats::fromJson(const Json& j) {
    EncounterStats s;
    s.encounters = j.i("encounters"); s.defeatsByJin = j.i("defeatsByJin");
    s.victoriesOverJin = j.i("victoriesOverJin"); s.timesFled = j.i("timesFled");
    s.timesSpared = j.i("timesSpared"); s.timesCheatedDeath = j.i("timesCheatedDeath");
    s.menLostToJin = j.i("menLostToJin");
    return s;
}

// ------------------------------------------------------------ commander
std::string Commander::displayName() const { return epithet.empty() ? name : name + " " + epithet; }
std::string Commander::fullTitle() const { return rankTitle(rank) + " " + displayName(); }

bool Commander::hasAdaptation(const std::string& a) const { return adaptationLevel(a) > 0; }
int Commander::adaptationLevel(const std::string& a) const {
    for (auto& x : adaptations) if (x.id == a) return x.level;
    return 0;
}
const Tie* Commander::tieTo(const CommanderId& o) const {
    for (auto& t : ties) if (t.other == o) return &t;
    return nullptr;
}
Tie* Commander::tieTo(const CommanderId& o) {
    for (auto& t : ties) if (t.other == o) return &t;
    return nullptr;
}

Json Commander::toJson() const {
    Json j;
    j["id"] = id; j["name"] = name; j["epithet"] = epithet; j["rank"] = toString(rank);
    j["rankXp"] = rankXp; j["renown"] = renown; j["archetype"] = toString(archetype);
    j["region"] = region; j["superior"] = superior; j["appearance"] = appearance; j["origin"] = origin;
    Json f;
    for (int i = 0; i < kFacetCount; ++i) f[toString(static_cast<Facet>(i))] = facets[i];
    j["facets"] = f;
    j["strengths"] = strSet(strengths); j["weaknesses"] = strSet(weaknesses); j["fears"] = strSet(fears);
    j["hate"] = hate; j["fearOfJin"] = fearOfJin;
    j["alive"] = alive; j["presumedDead"] = presumedDead; j["recoverUntil"] = recoverUntil;
    j["storyProtected"] = storyProtected; j["fate"] = fate; j["diedOn"] = diedOn;
    j["memories"] = vec(memories); j["scars"] = vec(scars); j["ties"] = vec(ties);
    j["adaptations"] = vec(adaptations); j["goal"] = goal.toJson(); j["stats"] = stats.toJson();
    j["history"] = vec(history); j["lastSeenByJin"] = lastSeenByJin;
    return j;
}

Commander Commander::fromJson(const Json& j) {
    Commander c;
    c.id = j.str("id"); c.name = j.str("name"); c.epithet = j.str("epithet");
    c.rank = fromString<Rank>(j.str("rank")); c.rankXp = j.i("rankXp"); c.renown = j.i("renown");
    c.archetype = fromString<Archetype>(j.str("archetype")); c.region = j.str("region");
    c.superior = j.str("superior"); c.appearance = j.str("appearance"); c.origin = j.str("origin");
    for (int i = 0; i < kFacetCount; ++i) c.facets[i] = j["facets"].i(toString(static_cast<Facet>(i)), 50);
    c.strengths = toSet(j["strengths"]); c.weaknesses = toSet(j["weaknesses"]); c.fears = toSet(j["fears"]);
    c.hate = j.i("hate"); c.fearOfJin = j.i("fearOfJin");
    c.alive = j.flag("alive", true); c.presumedDead = j.flag("presumedDead"); c.recoverUntil = j.i("recoverUntil");
    c.storyProtected = j.flag("storyProtected"); c.fate = j.str("fate"); c.diedOn = j.i("diedOn", -1);
    c.memories = unvec<Memory>(j["memories"]); c.scars = unvec<Scar>(j["scars"]); c.ties = unvec<Tie>(j["ties"]);
    c.adaptations = unvec<Adaptation>(j["adaptations"]); c.goal = Goal::fromJson(j["goal"]);
    c.stats = EncounterStats::fromJson(j["stats"]); c.history = unvec<HistoryEntry>(j["history"]);
    c.lastSeenByJin = j.i("lastSeenByJin", -1);
    return c;
}

// ------------------------------------------------------------ world records
Json Plan::toJson() const {
    Json j; j["id"] = id; j["kind"] = kind; j["leader"] = leader; j["region"] = region; j["readyOn"] = readyOn;
    j["expiresOn"] = expiresOn; j["strength"] = strength; j["done"] = done; j["text"] = text; return j;
}
Plan Plan::fromJson(const Json& j) {
    Plan p; p.id = j.str("id"); p.kind = j.str("kind"); p.leader = j.str("leader"); p.region = j.str("region");
    p.readyOn = j.i("readyOn"); p.expiresOn = j.i("expiresOn"); p.strength = j.i("strength", 1);
    p.done = j.flag("done"); p.text = j.str("text"); return p;
}

Json Region::toJson() const {
    Json j; j["id"] = id; j["name"] = name; j["unlocked"] = unlocked; j["control"] = control;
    j["pressure"] = pressure; j["alert"] = alert; return j;
}
Region Region::fromJson(const Json& j) {
    Region r; r.id = j.str("id"); r.name = j.str("name"); r.unlocked = j.flag("unlocked", true);
    r.control = j.i("control", 80); r.pressure = j.i("pressure"); r.alert = j.i("alert"); return r;
}

Json WorldEvent::toJson() const {
    Json j; j["day"] = day; j["kind"] = kind; j["region"] = region; j["text"] = text;
    Json w = Json::array(); for (auto& x : who) w.push(x); j["who"] = w; return j;
}
WorldEvent WorldEvent::fromJson(const Json& j) {
    WorldEvent e; e.day = j.i("day"); e.kind = j.str("kind"); e.region = j.str("region"); e.text = j.str("text");
    for (auto& x : j["who"].elements()) e.who.push_back(x.asString());
    return e;
}

} // namespace lw
