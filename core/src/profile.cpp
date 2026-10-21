#include "lw/profile.hpp"

#include <algorithm>

#include "lw/util.hpp"

namespace lw {

const char* toString(Stance s) {
    switch (s) { case Stance::Resentful: return "resentful"; case Stance::Vengeful: return "vengeful";
                 case Stance::Murderous: return "murderous"; }
    return "?";
}

Stance stanceOf(const Commander& c, const Config& cfg) {
    if (c.hate >= cfg.hateMurderousAt) return Stance::Murderous;
    if (c.hate >= cfg.hateVengefulAt) return Stance::Vengeful;
    return Stance::Resentful;
}

int CombatProfile::retinueTotal() const {
    int n = 0;
    for (auto& r : retinue) n += r.count;
    return n;
}

Json CombatProfile::toJson() const {
    Json j;
    j["commander"] = commander; j["displayName"] = displayName; j["title"] = title;
    j["archetype"] = toString(archetype); j["appearance"] = appearance; j["rank"] = rank;
    j["stance"] = toString(stance);
    j["healthMult"] = healthMult; j["damageMult"] = damageMult; j["staggerResist"] = staggerResist;
    j["accuracy"] = accuracy; j["aggression"] = aggression; j["alertness"] = alertness;
    j["fleeAtHealth"] = fleeAtHealth;
    Json r; for (auto& kv : resist) r[toString(kv.first)] = kv.second; j["resist"] = r;
    Json p = Json::array(); for (auto& x : panicTriggers) p.push(x); j["panicTriggers"] = p;
    Json t = Json::array(); for (auto& x : tactics) t.push(x); j["tactics"] = t;
    Json rt = Json::array();
    for (auto& s : retinue) { Json e; e["archetype"] = toString(s.archetype); e["count"] = s.count; e["role"] = s.role; rt.push(e); }
    j["retinue"] = rt;
    j["tauntKey"] = tauntKey;
    Json n = Json::array(); for (auto& x : notes) n.push(x); j["notes"] = n;
    return j;
}

namespace {
void addTactic(CombatProfile& p, const std::string& t) {
    if (std::find(p.tactics.begin(), p.tactics.end(), t) == p.tactics.end()) p.tactics.push_back(t);
}
void addRetinue(CombatProfile& p, Archetype a, int n, const std::string& role) {
    if (n <= 0) return;
    for (auto& r : p.retinue) if (r.role == role && r.archetype == a) { r.count += n; return; }
    p.retinue.push_back({a, n, role});
}
void addResist(CombatProfile& p, Wound w, double v) {
    p.resist[w] = std::min(0.8, p.resist[w] + v);
}
} // namespace

CombatProfile buildProfile(const Commander& c, const Config& cfg) {
    CombatProfile p;
    p.commander = c.id;
    p.displayName = c.displayName();
    p.title = c.fullTitle();
    p.archetype = c.archetype;
    p.appearance = c.appearance;
    p.rank = static_cast<int>(c.rank);
    p.stance = stanceOf(c, cfg);
    const int r = p.rank;

    // --- rank scaling. [enRAGE] ApplyRankStats: health 300 + 100*rank (+60 tough, +40 brawler),
    //     accuracy 25 + 10*rank (+12 marksman) capped at 92. Expressed relative to a 300 base.
    int hp = 300 + 100 * r;
    if (c.strengths.count("ironhide")) hp += 60;
    if (c.strengths.count("unyielding")) hp += 40;
    p.healthMult = hp / 300.0;
    int acc = std::min(92, 25 + 10 * r + (c.strengths.count("marksman") ? 12 : 0));
    p.accuracy = acc / 100.0;
    p.damageMult = 1.0 + 0.08 * (r - 1);
    p.staggerResist = 0.08 * (r - 1) + (c.strengths.count("unyielding") ? 0.15 : 0.0);

    // --- personality
    p.aggression = clampi(c.facet(Facet::Aggression), 0, 100) / 100.0;
    p.alertness = 0.35 + c.facet(Facet::Cunning) / 400.0 + c.facet(Facet::Discipline) / 400.0;
    if (c.weaknesses.count("hothead")) { p.aggression = std::min(1.0, p.aggression + 0.2); p.notes.push_back("hothead: charges in"); }
    if (c.weaknesses.count("drunkard")) { p.alertness -= 0.15; p.accuracy *= 0.85; p.notes.push_back("drunkard: slow to notice, shaky aim"); }
    if (c.weaknesses.count("coward") || c.facet(Facet::Courage) < 25) p.fleeAtHealth = 0.3;
    if (c.weaknesses.count("arrogant")) addTactic(p, "lead_from_front");
    if (c.strengths.count("tactician")) { p.alertness += 0.1; addTactic(p, "flank"); }

    // --- base retinue. [enRAGE] followers by rank: 0/1/3/5/10
    int guards = cfg.followersByRank[std::min<size_t>(r, cfg.followersByRank.size() - 1)];
    if (c.strengths.count("leader")) guards += 1;   // [enRAGE] "a leader of men (+1 man)"
    addRetinue(p, Archetype::Swordsman, cfg.baseRetinue, "soldiers");
    addRetinue(p, c.archetype == Archetype::Archer ? Archetype::Archer : Archetype::Spearman, guards, "bodyguard");

    // --- fears (traumas)
    if (c.fears.count("fire")) { p.panicTriggers.push_back("fire"); p.notes.push_back("fears fire: breaks when burned"); }
    if (c.fears.count("the_ghost")) { p.fleeAtHealth = std::max(p.fleeAtHealth, 0.5); p.notes.push_back("dreads the Ghost: shaken by terrify"); }
    if (c.fears.count("assassination")) { addTactic(p, "never_alone"); p.alertness += 0.1; }

    // --- learned adaptations (the core of "memories change tactics")
    for (auto& a : c.adaptations) {
        const int L = a.level;
        if (a.id == "paranoid") {
            p.alertness += 0.12 * L;
            addTactic(p, "never_alone");
            addTactic(p, "lookouts");
            addRetinue(p, Archetype::Archer, L, "lookout");
            p.notes.push_back("paranoid after stealth defeat: lookouts, wider awareness");
        } else if (a.id == "fire_ward") {
            addResist(p, Wound::Burn, 0.25 * L);
            addTactic(p, "douses_fire");
            p.notes.push_back("fire-warded gear after being burned");
        } else if (a.id == "fire_fear") {
            if (std::find(p.panicTriggers.begin(), p.panicTriggers.end(), "fire") == p.panicTriggers.end())
                p.panicTriggers.push_back("fire");
            addTactic(p, "keep_distance");
        } else if (a.id == "shield_wall") {
            addResist(p, Wound::Arrow, 0.2 * L);
            addRetinue(p, Archetype::Shield, 1 + L, "shield_wall");
            p.notes.push_back("brings shields after taking arrows");
        } else if (a.id == "standoff_wary") {
            addTactic(p, "refuse_standoff");
            addRetinue(p, Archetype::Archer, L, "archer_screen");
            p.notes.push_back("won't be called out again: archers cover the approach");
        } else if (a.id == "spread_out") {
            addResist(p, Wound::Blast, 0.2 * L);
            addTactic(p, "keep_distance");
        } else if (a.id == "poison_wary") {
            addResist(p, Wound::Poison, 0.3 * L);
        } else if (a.id == "horse_wary") {
            addTactic(p, "anti_horse");
            addRetinue(p, Archetype::Spearman, L, "spear_line");
        } else if (a.id == "emboldened") {
            p.aggression = std::min(1.0, p.aggression + 0.15 * L);
            p.damageMult += 0.05 * L;
            addTactic(p, "lead_from_front");
            p.notes.push_back("emboldened by beating Jin");
        } else if (a.id == "vengeful") {
            p.aggression = std::min(1.0, p.aggression + 0.1 * L);
            addTactic(p, "hunts_jin");
            addTactic(p, "berserk_opener");
        } else if (a.id == "ambusher") {
            addTactic(p, "ambush");
            addRetinue(p, Archetype::Archer, L, "archer_screen");
        }
    }

    // --- hate stance. [enRAGE] resentful = fists, vengeful = brings a blade, murderous = wants you dead
    switch (p.stance) {
        case Stance::Resentful:
            addTactic(p, "challenges_duel");
            break;
        case Stance::Vengeful:
            p.damageMult += 0.1;
            addRetinue(p, Archetype::Swordsman, 1 + r / 2, "soldiers");
            break;
        case Stance::Murderous:
            p.damageMult += 0.2;
            p.fleeAtHealth = 0.0;
            addTactic(p, "ambush");
            addTactic(p, "fire_arrows");
            addRetinue(p, Archetype::Archer, 2, "archer_screen");
            addRetinue(p, Archetype::Swordsman, r, "soldiers");
            break;
    }

    // Fear of Jin softens everything; a terrified commander hangs back.
    if (c.fearOfJin >= 60) {
        p.aggression *= 0.6;
        p.fleeAtHealth = std::max(p.fleeAtHealth, 0.35);
        addTactic(p, "hangs_back");
        p.notes.push_back("afraid of Jin");
    }

    p.alertness = std::min(1.0, std::max(0.0, p.alertness));
    p.aggression = std::min(1.0, std::max(0.0, p.aggression));

    // Dialogue bank selection (adapter maps keys to voice lines/subtitles).
    if (c.hasAdaptation("fire_ward") || c.hasAdaptation("fire_fear")) p.tauntKey = "taunt_burned";
    else if (c.stats.timesCheatedDeath > 0) p.tauntKey = "taunt_returned";
    else if (c.stats.victoriesOverJin > 0) p.tauntKey = "taunt_victor";
    else if (c.goal.type == GoalType::Avenge && c.goal.target != "jin") p.tauntKey = "taunt_avenger";
    else if (c.goal.type == GoalType::RestoreHonor) p.tauntKey = "taunt_humiliated";
    else p.tauntKey = c.stats.encounters > 0 ? "taunt_remembers" : "taunt_first";
    return p;
}

} // namespace lw
