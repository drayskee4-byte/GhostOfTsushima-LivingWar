// Living War - the boundary between the engine-agnostic core and a specific game.
//
// Game -> core: the plugin calls LivingWar::on*() from its hooks.
// Core -> game: the core calls IGameAdapter methods; each adapter reports which it can really do,
// so features degrade gracefully while hooks are still being reverse-engineered.
#pragma once
#include <cstdint>
#include <optional>
#include <string>

#include "lw/profile.hpp"

namespace lw {

using ActorHandle = uint64_t;   // opaque game entity handle/pointer; 0 = none

struct Capabilities {
    bool applyHealth = false;     // can scale a spawned actor's health/damage
    bool applyAI = false;         // can set aggression/alertness/flee behaviour
    bool applyResist = false;     // can reduce damage by type
    bool spawnActors = false;     // can spawn a commander + retinue on demand
    bool damageEvents = false;    // reports damage with type
    bool deathEvents = false;     // reports leader deaths
    bool campEvents = false;      // reports camp liberation
    bool gameClock = false;       // reports in-game time
    bool notifications = false;   // can show on-screen text
    Json toJson() const;
};

struct SpawnRequest {
    CombatProfile profile;
    std::string region;
    bool nearPlayer = true;
};

class IGameAdapter {
public:
    virtual ~IGameAdapter() = default;
    virtual std::string name() const = 0;
    virtual Capabilities capabilities() const = 0;
    // Apply a profile to a leader the game itself spawned. Return false if nothing could be applied.
    virtual bool applyProfile(ActorHandle actor, const CombatProfile& p) = 0;
    // Spawn a commander (and retinue) for a plan or dev command.
    virtual std::optional<ActorHandle> spawnCommander(const SpawnRequest& r) = 0;
    virtual void despawn(ActorHandle actor) = 0;
    virtual void notify(const std::string& title, const std::string& body) = 0;
};

// An adapter that does nothing in-game; used when hooks are unavailable so the simulation
// can still run and be inspected from the hierarchy report.
class NullAdapter : public IGameAdapter {
public:
    std::string name() const override { return "null"; }
    Capabilities capabilities() const override { return {}; }
    bool applyProfile(ActorHandle, const CombatProfile&) override { return false; }
    std::optional<ActorHandle> spawnCommander(const SpawnRequest&) override { return std::nullopt; }
    void despawn(ActorHandle) override {}
    void notify(const std::string&, const std::string&) override {}
};

} // namespace lw
