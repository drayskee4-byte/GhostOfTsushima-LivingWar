// A pretend game for tests and the simulator: records what the core asked it to do.
#pragma once
#include <map>
#include <string>
#include <vector>

#include "lw/adapter.hpp"

namespace lw {

class MockAdapter : public IGameAdapter {
public:
    bool quiet = false;
    std::map<ActorHandle, CombatProfile> applied;
    std::vector<std::string> notifications;
    ActorHandle nextHandle = 1000;

    std::string name() const override { return "mock"; }
    Capabilities capabilities() const override;
    bool applyProfile(ActorHandle a, const CombatProfile& p) override;
    std::optional<ActorHandle> spawnCommander(const SpawnRequest& r) override;
    void despawn(ActorHandle a) override;
    void notify(const std::string& title, const std::string& body) override;
};

} // namespace lw
