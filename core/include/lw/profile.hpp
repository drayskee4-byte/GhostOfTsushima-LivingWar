// Living War - CombatProfile: the concrete, game-applicable consequence of a commander's history.
// The game adapter turns these numbers into engine changes (health, AI flags, retinue spawns).
#pragma once
#include <map>
#include <string>
#include <vector>

#include "lw/config.hpp"
#include "lw/model.hpp"

namespace lw {

enum class Stance { Resentful, Vengeful, Murderous };   // [enRAGE] hate tiers
const char* toString(Stance);

struct RetinueSlot {
    Archetype archetype;
    int count;
    std::string role;   // "bodyguard", "lookout", "shield_wall", "spear_line", "archer_screen"
};

struct CombatProfile {
    CommanderId commander;
    std::string displayName;
    std::string title;
    Archetype archetype = Archetype::Swordsman;
    std::string appearance;
    int rank = 1;
    Stance stance = Stance::Resentful;

    // Multipliers relative to the game's base values for the archetype.
    double healthMult = 1.0;
    double damageMult = 1.0;
    double staggerResist = 0.0;   // 0..1 extra poise
    double accuracy = 0.25;       // 0..1, ranged
    double aggression = 0.5;      // 0..1 AI aggressiveness
    double alertness = 0.5;       // 0..1 detection speed / radius scale
    double fleeAtHealth = 0.0;    // flee when health fraction drops below this (0 = never)

    std::map<Wound, double> resist;     // damage reduction by type, 0..0.8
    std::vector<std::string> panicTriggers;  // e.g. "fire" -> breaks and flees when burned
    std::vector<std::string> tactics;        // AI hints: "never_alone", "refuse_standoff", "ambush",
                                             // "lead_from_front", "keep_distance", "anti_horse", "fire_arrows"
    std::vector<RetinueSlot> retinue;
    std::string tauntKey;                     // dialogue bank key for the adapter, e.g. "taunt_burned"
    std::vector<std::string> notes;           // why each change happened (for logs/UI)

    int retinueTotal() const;
    Json toJson() const;
};

Stance stanceOf(const Commander&, const Config&);
CombatProfile buildProfile(const Commander&, const Config&);

} // namespace lw
