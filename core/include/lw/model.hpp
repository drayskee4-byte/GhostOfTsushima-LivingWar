// Living War - core data model. Engine-agnostic: nothing here knows about the game.
#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "lw/json.hpp"

namespace lw {

using CommanderId = std::string;   // stable, e.g. "c0007"
using Day = int;                   // in-game day counter maintained by the core

// ------------------------------------------------------------ enums
// Mongol officer ranks, after the decimal army structure.
enum class Rank : int {
    Arban = 1,     // leader of ten
    Jaghun = 2,    // leader of a hundred
    Mingghan = 3,  // leader of a thousand
    Tumen = 4,     // general of a region
    Noyan = 5,     // the Khan's chosen - one per island
};
constexpr int kMinRank = 1, kMaxRank = 5;

// In-game enemy class the commander is spawned as.
enum class Archetype { Swordsman, Shield, Spearman, Brute, Archer, Shaman };

// How Jin fought.
enum class Method { Unknown, Stealth, Standoff, Melee, Ranged, Fire, Explosive, Poison, GhostWeapons, Mounted };

// Wound / damage categories.
enum class Wound { None, Cut, Burn, Arrow, Blast, Poison, Blunt };

enum class Outcome {
    CommanderKilled,   // Jin landed a killing blow (may still cheat death)
    CommanderFled,     // commander escaped the fight
    CommanderSpared,   // Jin let him go / left him alive
    JinKilled,         // the commander (or his men) killed Jin
    JinFled,           // Jin broke off / escaped
    Inconclusive,      // encounter ended without a result (left range etc.)
};

enum class Emotion { Rage, Fear, Shame, Pride, Grief, Triumph, Respect };

enum class TieType { Superior, Subordinate, Rival, BloodBrother, Kin, Mentor, Protege };

enum class GoalType { None, Avenge, RestoreHonor, Rise, Protect, Survive };

// Facets are 0..100.
enum class Facet { Aggression, Courage, Pride, Cunning, Ambition, Cruelty, Loyalty, Discipline };
constexpr int kFacetCount = 8;

const char* toString(Rank);
const char* toString(Archetype);
const char* toString(Method);
const char* toString(Wound);
const char* toString(Outcome);
const char* toString(Emotion);
const char* toString(TieType);
const char* toString(GoalType);
const char* toString(Facet);
std::string rankTitle(Rank);  // English display title

template <class E> E fromString(const std::string&);
template <> Rank fromString<Rank>(const std::string&);
template <> Archetype fromString<Archetype>(const std::string&);
template <> Method fromString<Method>(const std::string&);
template <> Wound fromString<Wound>(const std::string&);
template <> Outcome fromString<Outcome>(const std::string&);
template <> Emotion fromString<Emotion>(const std::string&);
template <> TieType fromString<TieType>(const std::string&);
template <> GoalType fromString<GoalType>(const std::string&);
template <> Facet fromString<Facet>(const std::string&);

// ------------------------------------------------------------ records
struct Memory {
    Day day = 0;
    std::string kind;       // machine tag, e.g. "defeated_by_stealth"
    Emotion emotion = Emotion::Rage;
    int intensity = 50;     // 0..100, fades unless longTerm
    bool longTerm = false;
    std::string text;       // human-readable line for the UI
    Json toJson() const; static Memory fromJson(const Json&);
};

struct Scar {
    Day day = 0;
    Wound wound = Wound::Cut;
    std::string location;   // "face", "left arm", ...
    bool fromJin = true;
    std::string text;
    Json toJson() const; static Scar fromJson(const Json&);
};

struct Tie {
    CommanderId other;
    TieType type = TieType::Rival;
    int strength = 50;      // -100..100 (negative = hostile)
    std::string note;
    Json toJson() const; static Tie fromJson(const Json&);
};

struct Goal {
    GoalType type = GoalType::None;
    std::string target;     // "jin" or a commander id
    Day since = 0;
    std::string text;
    Json toJson() const; static Goal fromJson(const Json&);
};

struct HistoryEntry {
    Day day = 0;
    std::string text;
    Json toJson() const; static HistoryEntry fromJson(const Json&);
};

// A learned counter-tactic or trauma: these drive actual combat behaviour.
// Known ids (see profile.cpp for their effects):
//   paranoid      - defeated by stealth: lookouts, wider awareness, never alone
//   fire_fear     - burned and lacks courage: panics near fire
//   fire_ward     - burned but brave: fire-resistant gear, brings water/sand
//   shield_wall   - shot by arrows: brings shield bearers, raised guard
//   standoff_wary - lost a standoff: refuses duels, keeps archers back
//   spread_out    - hit by explosives: men keep distance from each other
//   poison_wary   - poisoned: resists poison
//   emboldened    - beat Jin: fights in front, aggressive, fewer guards
//   vengeful      - lost men/superior to Jin: hunts him, berserk opener
//   ambusher      - humiliated: prefers to strike from ambush
//   horse_wary    - ridden down: brings spears against horsemen
struct Adaptation {
    std::string id;
    int level = 1;          // stacks 1..3
    Day learned = 0;
    std::string reason;
    Json toJson() const; static Adaptation fromJson(const Json&);
};

struct EncounterStats {
    int encounters = 0;
    int defeatsByJin = 0;      // knocked down / "killed" by Jin
    int victoriesOverJin = 0;  // killed Jin
    int timesFled = 0;
    int timesSpared = 0;
    int timesCheatedDeath = 0;
    int menLostToJin = 0;
    Json toJson() const; static EncounterStats fromJson(const Json&);
};

struct Commander {
    CommanderId id;
    std::string name;          // personal name, e.g. "Temur"
    std::string epithet;       // earned nickname, e.g. "the Burned"
    Rank rank = Rank::Arban;
    int rankXp = 0;
    int renown = 0;            // military influence
    Archetype archetype = Archetype::Swordsman;
    std::string region;        // assigned region id
    CommanderId superior;      // empty for top of chain
    std::string appearance;    // adapter-interpreted appearance key (model/armour variant)
    std::string origin;        // homeland flavour

    std::array<int, kFacetCount> facets{};      // personality
    std::set<std::string> strengths;            // e.g. "ironhide","marksman","tactician"
    std::set<std::string> weaknesses;           // e.g. "hothead","drunkard","arrogant"
    std::set<std::string> fears;                // e.g. "fire","assassination","the_ghost"

    int hate = 0;              // toward Jin, 0..100
    int fearOfJin = 0;         // 0..100

    bool alive = true;
    bool presumedDead = false; // cheated death, recovering, will return
    Day recoverUntil = 0;
    bool storyProtected = false;
    bool active = false;       // currently spawned in the game
    std::string fate;          // how he died, if dead
    Day diedOn = -1;

    std::vector<Memory> memories;
    std::vector<Scar> scars;
    std::vector<Tie> ties;
    std::vector<Adaptation> adaptations;
    Goal goal;
    EncounterStats stats;
    std::vector<HistoryEntry> history;
    Day lastSeenByJin = -1;

    int facet(Facet f) const { return facets[static_cast<int>(f)]; }
    int& facet(Facet f) { return facets[static_cast<int>(f)]; }
    std::string displayName() const;   // "Temur the Burned"
    std::string fullTitle() const;     // "Jaghun-u Noyan Temur the Burned"
    bool hasAdaptation(const std::string& id) const;
    int adaptationLevel(const std::string& id) const;
    const Tie* tieTo(const CommanderId& other) const;
    Tie* tieTo(const CommanderId& other);
    bool available(Day today) const { return alive && !(presumedDead && today < recoverUntil); }

    Json toJson() const;
    static Commander fromJson(const Json&);
};

// A scheduled future encounter the world wants to throw at Jin.
struct Plan {
    std::string id;
    std::string kind;         // "revenge_ambush", "hunting_party", "retaliation", "return"
    CommanderId leader;
    std::string region;
    Day readyOn = 0;
    Day expiresOn = 0;
    int strength = 1;         // retinue scale
    bool done = false;
    std::string text;
    Json toJson() const; static Plan fromJson(const Json&);
};

struct Region {
    std::string id;           // "izuhara"
    std::string name;         // "Izuhara"
    bool unlocked = true;     // gated by campaign act
    int control = 80;         // Mongol control 0..100
    int pressure = 0;         // how hard Jin is pushing here, decays
    int alert = 0;            // patrol/hunt intensity 0..100
    Json toJson() const; static Region fromJson(const Json&);
};

struct WorldEvent {
    Day day = 0;
    std::string kind;
    std::string region;
    std::vector<CommanderId> who;
    std::string text;
    Json toJson() const; static WorldEvent fromJson(const Json&);
};

// What the game (or a test) tells the core about a finished fight.
struct EncounterReport {
    CommanderId commander;
    Outcome outcome = Outcome::Inconclusive;
    Method method = Method::Unknown;          // Jin's dominant approach
    std::map<Wound, int> damageTaken;         // damage the commander took, by type
    int subordinatesKilled = 0;
    bool campLiberated = false;
    bool detectedJin = true;                  // false = never saw him coming
    std::string region;
    std::string place;                        // optional location name
};

// What the core decided, for logs/UI/tests.
struct EncounterResult {
    bool died = false;
    bool cheatedDeath = false;
    bool rankedUp = false;
    std::vector<std::string> newAdaptations;
    std::vector<std::string> lines;           // narrative lines
    std::vector<std::string> plansCreated;
};

} // namespace lw
