#include "mock_adapter.hpp"

#include <cstdio>

namespace lw {

Capabilities MockAdapter::capabilities() const {
    Capabilities c;
    c.applyHealth = c.applyAI = c.applyResist = c.spawnActors = true;
    c.damageEvents = c.deathEvents = c.campEvents = c.gameClock = c.notifications = true;
    return c;
}

bool MockAdapter::applyProfile(ActorHandle a, const CombatProfile& p) {
    applied[a] = p;
    return true;
}

std::optional<ActorHandle> MockAdapter::spawnCommander(const SpawnRequest& r) {
    ActorHandle h = nextHandle++;
    applied[h] = r.profile;
    if (!quiet) std::printf("  [game] spawned %s with %d men in %s\n", r.profile.title.c_str(), r.profile.retinueTotal(), r.region.c_str());
    return h;
}

void MockAdapter::despawn(ActorHandle a) { applied.erase(a); }

void MockAdapter::notify(const std::string& title, const std::string& body) {
    notifications.push_back(title + ": " + body);
    if (!quiet) std::printf("  [game banner] %s - %s\n", title.c_str(), body.c_str());
}

} // namespace lw
