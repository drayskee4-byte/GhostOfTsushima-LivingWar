// Living War - the persistent world and the Nemesis rules that act on it.
#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "lw/config.hpp"
#include "lw/model.hpp"
#include "lw/profile.hpp"
#include "lw/util.hpp"

namespace lw {

constexpr int kSchemaVersion = 1;

struct PendingSeat {
    CommanderId fallen;
    std::string fallenName;
    std::string region;
    Rank rank = Rank::Arban;
    CommanderId superior;
    Day fillOn = 0;
    bool killedByJin = false;
    Json toJson() const; static PendingSeat fromJson(const Json&);
};

struct World {
    int schemaVersion = kSchemaVersion;
    Config cfg;
    Rng rng;
    Day day = 0;
    int act = 1;                     // campaign act reported by the game/user (1..3)
    int nextId = 1;
    std::map<CommanderId, Commander> commanders;
    std::map<std::string, Region> regions;
    std::vector<Plan> plans;
    std::vector<WorldEvent> events;
    std::vector<PendingSeat> seats;

    Commander* find(const CommanderId& id);
    const Commander* find(const CommanderId& id) const;
    Json toJson() const;
    static World fromJson(const Json& j);   // keeps j's config
};

// Who the world wants Jin to meet when the game spawns an enemy leader.
struct Offer {
    CommanderId commander;
    std::string planId;     // empty for an ordinary appearance
    std::string kind;       // "plan:<kind>", "hunter", "regular", "new"
    CombatProfile profile;
};

class Nemesis {
public:
    explicit Nemesis(World& w) : w_(w) {}

    // ---- setup
    void seed(uint64_t seed, int act = 1);
    void setAct(int act);
    Commander& create(const std::string& region, Rank rank, const CommanderId& superior,
                      std::optional<Archetype> arch = std::nullopt);

    // ---- the Nemesis loop
    EncounterResult resolve(const EncounterReport& r);
    std::optional<Offer> offerFor(const std::string& region, int maxRank = 2, bool allowNew = true);
    void markActive(const CommanderId& id, bool active);
    void onCampLiberated(const std::string& region, const std::string& camp, int menKilled,
                         const CommanderId& leader = "");
    void onPlayerSighted(const std::string& region);   // bumps alert (patrols hunting Jin)

    // ---- time
    void advanceDays(int n);
    void advanceTo(Day d) { if (d > w_.day) advanceDays(d - w_.day); }

    // ---- hierarchy
    std::vector<const Commander*> subordinates(const CommanderId& id) const;
    std::vector<const Commander*> chainOfCommand(const CommanderId& id) const;   // self -> top
    std::vector<const Commander*> livingIn(const std::string& region) const;
    int seatsUsed(const std::string& region, Rank rank) const;
    bool seatFree(const std::string& region, Rank rank) const;
    int xpNeed(Rank r) const { return w_.cfg.xpPerRankStep * std::max(1, static_cast<int>(r)); }  // [enRAGE] XpNeed

    // ---- record keeping (public for tests / dev tools)
    void gainXp(Commander& c, int amount, const std::string& reason);
    bool tryRankUp(Commander& c, const std::string& reason);
    void remember(Commander& c, const std::string& kind, Emotion e, int intensity, const std::string& text,
                  bool longTerm = false);
    void history(Commander& c, const std::string& text);
    void event(const std::string& kind, const std::string& region, std::vector<CommanderId> who,
               const std::string& text);
    bool adapt(Commander& c, const std::string& id, const std::string& reason);   // true if new or levelled
    void kill(Commander& c, const std::string& cause, bool byJin, Method m = Method::Unknown);

    World& world() { return w_; }

private:
    void rollPersonality(Commander& c, Archetype a);
    void learnFrom(Commander& c, const EncounterReport& r, EncounterResult& out);
    void addPlan(const std::string& kind, Commander& leader, int minDays, int maxDays, int strength,
                 const std::string& text, EncounterResult* out = nullptr);
    void scheduleSeat(const Commander& fallen, bool byJin);
    void fillSeat(const PendingSeat& s);
    void dailyTick();
    void tickDecay();
    void tickRecovery();
    void tickRivalries();
    void tickPromotions();
    void tickPlans();
    void tickRegions();
    void tie(Commander& a, Commander& b, TieType ab, TieType ba, int strength, const std::string& note);
    std::string newId();
    std::string commonName(const Commander& c) const { return c.displayName(); }
    std::string regionName(const std::string& id) const;

    World& w_;
};

} // namespace lw
