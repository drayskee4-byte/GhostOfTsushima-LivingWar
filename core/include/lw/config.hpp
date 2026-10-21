// Living War - tunable rules. Every value can be overridden from LivingWar.json.
//
// Many defaults are taken from enRAGE 0.9.5.1 (RDR2 Nemesis System) by the enRAGE author,
// used with the author's permission. Each is marked [enRAGE].
#pragma once
#include <map>
#include <string>
#include <vector>

#include "lw/json.hpp"
#include "lw/model.hpp"

namespace lw {

struct Config {
    // --- cheating death ------------------------------------------------------------
    int cheatDeathBase = 35;          // [enRAGE] % chance at rank 1
    int cheatDeathPerRank = 5;        // [enRAGE] + per rank above 1
    int cheatDeathMax = 60;           // [enRAGE] cap
    bool cheatDeathAlways = false;    // [enRAGE] debug: always survive
    // Methods that are always final (enRAGE: FatalWoundsFinal for head/neck wounds).
    std::vector<Method> finalMethods = {};
    // Survival multiplier per method (clean kills are harder to walk away from).
    std::map<Method, double> methodSurvival = {
        {Method::Stealth, 0.6}, {Method::Standoff, 0.7}, {Method::Explosive, 0.85}, {Method::Poison, 1.1},
    };
    int recoverMinDays = 3, recoverMaxDays = 7;

    // --- ranks ---------------------------------------------------------------------
    int xpPerRankStep = 2;            // [enRAGE] XP needed = step * rank
    int xpCheatedDeath = 3;           // [enRAGE] "cheated death - carries your scar"
    int xpKilledJin = 3;              // [enRAGE] "killed you"
    int xpDistinguished = 2;          // [enRAGE] "distinguished himself"
    int xpJinFled = 2;
    int xpSurvived = 1;               // [enRAGE] "a job that paid" equivalent
    double riseGoalXpMult = 1.5;      // [enRAGE] Rise goal
    // Seats per region at each rank (index = rank). Rank 5 is world-wide.
    std::vector<int> seatsPerRegion = {0, 8, 4, 2, 1, 1};

    // --- hate & fear ---------------------------------------------------------------
    int hateVengefulAt = 40;          // [enRAGE] Hate.VengefulAt
    int hateMurderousAt = 70;         // [enRAGE] Hate.MurderousAt
    int hateDefeated = 30, hateFled = 15, hatePerManLost = 2, hateMenLostCap = 30;
    int hateSuperiorKilled = 35, hateKinKilled = 50, hateBrotherKilled = 40, hateCampLiberated = 20;
    int hateSpared = -15;
    int fearFled = 20, fearDefeated = 15, fearSpared = 10, fearKilledJin = -30;
    int hateDecayPerDay = 1, fearDecayPerDay = 2;
    int memoryFadePerDay = 2;

    // --- retinues ------------------------------------------------------------------
    std::vector<int> followersByRank = {0, 0, 1, 3, 5, 10};   // [enRAGE] FollowersRank1..5
    int baseRetinue = 3;

    // --- succession ----------------------------------------------------------------
    int successionMinDays = 2, successionMaxDays = 6;          // [enRAGE]
    int successionKinChance = 35;                              // [enRAGE]

    // --- world simulation ----------------------------------------------------------
    int maxLivingCommanders = 100;                             // [enRAGE] MaxLivingNemeses
    int rivalryClashPerDayPct = 4;
    int rivalryDeathPct = 12;          // share of clashes that end in a death
    bool simulationMayKill = true;     // allow off-screen deaths (rivalry); never story NPCs
    int revengeMinDays = 2, revengeMaxDays = 5;
    int retaliationMinDays = 1, retaliationMaxDays = 3;
    int planLifetimeDays = 10;
    int reinforceAtPressure = 60;      // region pressure that triggers reassignment of a stronger officer
    int pressureDecayPerDay = 4;
    int controlLossPerCamp = 10;

    static Config fromJson(const Json& j);  // overrides defaults with any keys present
    Json toJson() const;
};

} // namespace lw
