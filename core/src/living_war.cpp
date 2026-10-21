#include "lw/living_war.hpp"

#include <filesystem>

namespace lw {

Json Capabilities::toJson() const {
    Json j;
    j["applyHealth"] = applyHealth; j["applyAI"] = applyAI; j["applyResist"] = applyResist;
    j["spawnActors"] = spawnActors; j["damageEvents"] = damageEvents; j["deathEvents"] = deathEvents;
    j["campEvents"] = campEvents; j["gameClock"] = gameClock; j["notifications"] = notifications;
    return j;
}

LivingWar::LivingWar(IGameAdapter* adapter, const std::string& dataDir)
    : adapter_(adapter ? adapter : &nullAdapter_), saves_(dataDir), nemesis_(world_) {}

bool LivingWar::start(const std::string& slot, uint64_t seedIfNew, int act) {
    slot_ = slot;
    SaveManager::ensureDir(saves_.dataDir());
    // Config: create with defaults on first run so players can see and edit every value.
    std::string cfgPath = (std::filesystem::path(saves_.dataDir()) / "LivingWar.json").string();
    Config cfg;
    if (auto txt = SaveManager::readFile(cfgPath)) {
        try { cfg = Config::fromJson(Json::parse(*txt)); }
        catch (const std::exception& e) { Log::error(std::string("LivingWar.json unreadable, using defaults: ") + e.what()); }
    } else {
        SaveManager::writeAtomic(cfgPath, cfg.toJson().dump(2));
    }
    if (auto w = saves_.load(slot)) {
        world_ = std::move(*w);
        world_.cfg = cfg;   // the file on disk wins, so tuning changes apply to existing saves
        Log::info("Living War: loaded slot '" + slot + "' at day " + std::to_string(world_.day) + " with " +
                  std::to_string(world_.commanders.size()) + " commanders");
    } else {
        nemesis_.seed(seedIfNew, act);
        world_.cfg = cfg;
        Log::info("Living War: new world for slot '" + slot + "' (seed " + std::to_string(seedIfNew) + ")");
        saves_.save(world_, slot_);
    }
    Log::info("adapter '" + adapter_->name() + "' capabilities: " + adapter_->capabilities().toJson().dump());
    return true;
}

bool LivingWar::save() { return saves_.save(world_, slot_); }

LivingWar::Tracked* LivingWar::tracked(ActorHandle h) {
    auto it = live_.find(h);
    return it == live_.end() ? nullptr : &it->second;
}

std::optional<CombatProfile> LivingWar::onLeaderSpawned(ActorHandle actor, const std::string& region, int maxRank) {
    if (!actor) return std::nullopt;
    if (auto* t = tracked(actor)) return buildProfile(*world_.find(t->commander), world_.cfg);   // already bound
    auto offer = nemesis_.offerFor(region, maxRank);
    if (!offer) return std::nullopt;
    nemesis_.markActive(offer->commander, true);
    live_[actor] = Tracked{offer->commander, offer->planId, region, {}, {}, 0, false};
    bool applied = adapter_->applyProfile(actor, offer->profile);
    Log::info("leader spawned in " + region + " -> " + offer->profile.title + " [" + offer->kind + "]" +
              (applied ? "" : " (profile NOT applied in-game: adapter lacks hooks)"));
    const Commander* c = world_.find(offer->commander);
    if (c && c->stats.encounters > 0) {
        // Lead with the strongest memory of Jin himself (long-term ones win ties).
        const Memory* best = nullptr;
        for (auto& m : c->memories) {
            if (m.kind == "loss" || m.kind == "promoted" || m.kind == "succeeded" || m.kind == "feud_lost") continue;
            if (!best || m.intensity + (m.longTerm ? 10 : 0) >= best->intensity + (best->longTerm ? 10 : 0)) best = &m;
        }
        adapter_->notify(offer->profile.title, best ? "He remembers: " + best->text : "He remembers you.");
    }
    return offer->profile;
}

void LivingWar::onPlayerDetected(ActorHandle h) { if (auto* t = tracked(h)) t->detected = true; }

void LivingWar::onLeaderDamaged(ActorHandle h, Wound w, int amount, Method m) {
    auto* t = tracked(h);
    if (!t || amount <= 0) return;
    t->damage[w] += amount;
    t->methods[m] += amount;
}

void LivingWar::onSoldierKilled(ActorHandle h) { if (auto* t = tracked(h)) t->soldiersKilled++; }

void LivingWar::finish(ActorHandle h, Outcome o, Method finalBlow) {
    auto it = live_.find(h);
    if (it == live_.end()) return;
    Tracked t = it->second;
    live_.erase(it);
    EncounterReport r;
    r.commander = t.commander;
    r.outcome = o;
    r.damageTaken = t.damage;
    r.subordinatesKilled = t.soldiersKilled;
    r.detectedJin = t.detected;
    r.region = t.region;
    // The killing blow decides "how" if we have it; otherwise whatever did the most damage.
    r.method = finalBlow;
    if (r.method == Method::Unknown) {
        int best = 0;
        for (auto& kv : t.methods) if (kv.first != Method::Unknown && kv.second > best) { best = kv.second; r.method = kv.first; }
    }
    lastResult_ = nemesis_.resolve(r);
    for (auto& line : lastResult_->lines) Log::info("  " + line);
    if (!lastResult_->lines.empty()) adapter_->notify("Living War", lastResult_->lines.front());
    save();
}

void LivingWar::onLeaderKilled(ActorHandle h, Method m) { finish(h, Outcome::CommanderKilled, m); }
void LivingWar::onLeaderFled(ActorHandle h) { finish(h, Outcome::CommanderFled); }
void LivingWar::onLeaderSpared(ActorHandle h) { finish(h, Outcome::CommanderSpared); }
void LivingWar::onPlayerEscaped(ActorHandle h) { finish(h, Outcome::JinFled); }
void LivingWar::onEncounterEnded(ActorHandle h) {
    if (auto* t = tracked(h)) nemesis_.markActive(t->commander, false);
    finish(h, Outcome::Inconclusive);
}

void LivingWar::onLeaderReleased(ActorHandle h) {
    auto it = live_.find(h);
    if (it == live_.end()) return;
    nemesis_.markActive(it->second.commander, false);
    live_.erase(it);
}

bool LivingWar::wasEngaged(ActorHandle h) const {
    auto it = live_.find(h);
    return it != live_.end() && (it->second.detected || !it->second.damage.empty() || it->second.soldiersKilled > 0);
}

std::string LivingWar::trackedName(ActorHandle h) const {
    auto it = live_.find(h);
    if (it == live_.end()) return "";
    const Commander* c = world_.find(it->second.commander);
    return c ? c->fullTitle() : "";
}

void LivingWar::onPlayerKilled(ActorHandle killer) {
    if (killer && tracked(killer)) { finish(killer, Outcome::JinKilled); }
    // Every other tracked leader saw Jin fall: they drove him off.
    std::vector<ActorHandle> rest;
    for (auto& kv : live_) rest.push_back(kv.first);
    for (auto h : rest) finish(h, Outcome::JinFled);
}

void LivingWar::onCampLiberated(const std::string& campId, const std::string& region, int menKilled, ActorHandle leader) {
    CommanderId lid;
    if (auto* t = tracked(leader)) lid = t->commander;
    nemesis_.onCampLiberated(region, campId, menKilled, lid);
    save();
}

void LivingWar::onGameMinutes(int64_t m) {
    if (m < 0) return;
    if (dayZeroMinutes_ < 0) dayZeroMinutes_ = m - static_cast<int64_t>(world_.day) * 1440;   // anchor to the save
    if (m < lastMinutes_) dayZeroMinutes_ = m - static_cast<int64_t>(world_.day) * 1440;     // clock went back (load)
    lastMinutes_ = m;
    Day d = static_cast<Day>((m - dayZeroMinutes_) / 1440);
    if (d > world_.day) {
        int before = world_.day;
        nemesis_.advanceTo(std::min(d, world_.day + world_.cfg.planLifetimeDays * 3));  // cap catch-up after long sleeps
        Log::info("world advanced from day " + std::to_string(before) + " to " + std::to_string(world_.day));
        save();
    }
}

void LivingWar::setAct(int act) { nemesis_.setAct(act); }

std::optional<ActorHandle> LivingWar::devSpawn(const CommanderId& id, const std::string& region) {
    Commander* c = world_.find(id);
    if (!c || !c->alive || c->storyProtected) return std::nullopt;
    SpawnRequest req{buildProfile(*c, world_.cfg), region.empty() ? c->region : region, true};
    auto h = adapter_->spawnCommander(req);
    if (h) {
        nemesis_.markActive(id, true);
        live_[*h] = Tracked{id, "", req.region, {}, {}, 0, false};
    }
    return h;
}

std::optional<ActorHandle> LivingWar::devSpawnOffer(const std::string& region) {
    auto o = nemesis_.offerFor(region, 5);
    if (!o) return std::nullopt;
    auto h = adapter_->spawnCommander(SpawnRequest{o->profile, region, true});
    if (h) {
        nemesis_.markActive(o->commander, true);
        live_[*h] = Tracked{o->commander, o->planId, region, {}, {}, 0, false};
    }
    return h;
}

bool LivingWar::exportHierarchy(const std::string& path) const {
    return SaveManager::writeAtomic(path, renderHierarchyHtml(world_));
}

std::string LivingWar::statusLine() const {
    int alive = 0, dead = 0, hunting = 0;
    for (auto& kv : world_.commanders) {
        if (kv.second.storyProtected) continue;
        kv.second.alive ? ++alive : ++dead;
        hunting += kv.second.alive && kv.second.hate >= world_.cfg.hateVengefulAt;
    }
    return fmt("Day %d | %d officers alive, %d dead, %d hunting Jin | %zu live encounters", world_.day, alive, dead, hunting, live_.size());
}

} // namespace lw
