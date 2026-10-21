// Ghost of Tsushima adapter.
//
// STATUS: the game-facing hooks are NOT implemented yet. This adapter resolves the signatures
// listed in GotSignatures.ini and reports which ones match this exe build, but it doesn't
// read or write game memory. Every capability is reported false until the matching
// reader/writer is written and tested in-game. See docs/INTEGRATION_PLAN.md.
#pragma once
#include <map>
#include <string>

#include "lw/adapter.hpp"
#include "pattern.hpp"

namespace lwp {

class GotAdapter : public lw::IGameAdapter {
public:
    explicit GotAdapter(std::string feedPath) : feedPath_(std::move(feedPath)) {}

    // Load GotSignatures.ini and try to resolve each entry. Returns a human-readable report.
    std::string probe(const std::string& signaturesIni);
    const ModuleImage& image() const { return image_; }
    // Resolved address for a signature name (names are lower-case, as the INI reader stores them). 0 if missing.
    uintptr_t addr(const std::string& lowerName) const {
        auto it = resolved_.find(lowerName);
        return it == resolved_.end() ? 0 : it->second;
    }

    std::string name() const override { return "ghost-of-tsushima (probe only)"; }
    lw::Capabilities capabilities() const override;
    bool applyProfile(lw::ActorHandle actor, const lw::CombatProfile& p) override;
    std::optional<lw::ActorHandle> spawnCommander(const lw::SpawnRequest& r) override;
    void despawn(lw::ActorHandle) override {}
    void notify(const std::string& title, const std::string& body) override;

private:
    std::string feedPath_;
    ModuleImage image_;
    std::map<std::string, uintptr_t> resolved_;
};

} // namespace lwp
