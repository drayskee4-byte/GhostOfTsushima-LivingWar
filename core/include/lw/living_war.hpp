// Living War - the facade a game plugin talks to.
#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "lw/adapter.hpp"
#include "lw/save.hpp"
#include "lw/world.hpp"

namespace lw {

class LivingWar {
public:
    LivingWar(IGameAdapter* adapter, const std::string& dataDir);

    // ---- lifecycle
    // Loads <slot>, or seeds a fresh world if none exists. Config overrides come from
    // <dataDir>/LivingWar.json (written with defaults on first run).
    bool start(const std::string& slot, uint64_t seedIfNew = 1274, int act = 1);
    bool save();
    const std::string& slot() const { return slot_; }

    // ---- game -> core
    // The game spawned an enemy leader (camp commander, patrol leader...). Returns the profile to
    // apply, which has already been passed to adapter->applyProfile().
    std::optional<CombatProfile> onLeaderSpawned(ActorHandle actor, const std::string& region, int maxRank = 2);
    void onPlayerDetected(ActorHandle leader);
    void onLeaderDamaged(ActorHandle leader, Wound w, int amount, Method m);
    void onSoldierKilled(ActorHandle leader);                // one of his men died
    void onLeaderKilled(ActorHandle leader, Method m);
    void onLeaderFled(ActorHandle leader);
    void onLeaderSpared(ActorHandle leader);
    void onPlayerKilled(ActorHandle killerLeader);           // 0 if not by a tracked leader
    void onPlayerEscaped(ActorHandle leader);
    void onEncounterEnded(ActorHandle leader);               // despawned / out of range
    void onLeaderReleased(ActorHandle leader);               // gone without ever fighting Jin: no record
    bool isTracked(ActorHandle leader) const { return live_.count(leader) > 0; }
    bool wasEngaged(ActorHandle leader) const;
    std::string trackedName(ActorHandle leader) const;
    void onCampLiberated(const std::string& campId, const std::string& region, int menKilled, ActorHandle leader = 0);
    void onGameMinutes(int64_t absoluteMinutes);             // in-game clock
    void setAct(int act);

    // ---- dev / tools
    std::optional<ActorHandle> devSpawn(const CommanderId& id, const std::string& region);
    std::optional<ActorHandle> devSpawnOffer(const std::string& region);
    bool exportHierarchy(const std::string& htmlPath) const;
    std::string statusLine() const;

    World& world() { return world_; }
    Nemesis& nemesis() { return nemesis_; }
    const std::optional<EncounterResult>& lastResult() const { return lastResult_; }

private:
    struct Tracked {
        CommanderId commander;
        std::string planId;
        std::string region;
        std::map<Wound, int> damage;
        std::map<Method, int> methods;
        int soldiersKilled = 0;
        bool detected = false;
    };
    void finish(ActorHandle h, Outcome o, Method finalBlow = Method::Unknown);
    Tracked* tracked(ActorHandle h);

    IGameAdapter* adapter_;
    NullAdapter nullAdapter_;
    SaveManager saves_;
    World world_;
    Nemesis nemesis_;
    std::string slot_ = "default";
    std::map<ActorHandle, Tracked> live_;
    std::optional<EncounterResult> lastResult_;
    int64_t lastMinutes_ = -1;
    int64_t dayZeroMinutes_ = -1;
};

// HTML hierarchy viewer (self-contained page).
std::string renderHierarchyHtml(const World& w);

} // namespace lw
