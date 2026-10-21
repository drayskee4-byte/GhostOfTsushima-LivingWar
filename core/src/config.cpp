#include "lw/config.hpp"

namespace lw {

namespace {
// One table drives both load and save so the JSON keys never drift from the fields.
template <class F>
void eachInt(Config& c, F&& f) {
    f("cheatDeathBase", c.cheatDeathBase); f("cheatDeathPerRank", c.cheatDeathPerRank);
    f("cheatDeathMax", c.cheatDeathMax); f("recoverMinDays", c.recoverMinDays); f("recoverMaxDays", c.recoverMaxDays);
    f("xpPerRankStep", c.xpPerRankStep); f("xpCheatedDeath", c.xpCheatedDeath); f("xpKilledJin", c.xpKilledJin);
    f("xpDistinguished", c.xpDistinguished); f("xpJinFled", c.xpJinFled); f("xpSurvived", c.xpSurvived);
    f("hateVengefulAt", c.hateVengefulAt); f("hateMurderousAt", c.hateMurderousAt);
    f("hateDefeated", c.hateDefeated); f("hateFled", c.hateFled); f("hatePerManLost", c.hatePerManLost);
    f("hateMenLostCap", c.hateMenLostCap); f("hateSuperiorKilled", c.hateSuperiorKilled);
    f("hateKinKilled", c.hateKinKilled); f("hateBrotherKilled", c.hateBrotherKilled);
    f("hateCampLiberated", c.hateCampLiberated); f("hateSpared", c.hateSpared);
    f("fearFled", c.fearFled); f("fearDefeated", c.fearDefeated); f("fearSpared", c.fearSpared);
    f("fearKilledJin", c.fearKilledJin); f("hateDecayPerDay", c.hateDecayPerDay);
    f("fearDecayPerDay", c.fearDecayPerDay); f("memoryFadePerDay", c.memoryFadePerDay);
    f("baseRetinue", c.baseRetinue); f("successionMinDays", c.successionMinDays);
    f("successionMaxDays", c.successionMaxDays); f("successionKinChance", c.successionKinChance);
    f("maxLivingCommanders", c.maxLivingCommanders); f("rivalryClashPerDayPct", c.rivalryClashPerDayPct);
    f("rivalryDeathPct", c.rivalryDeathPct); f("revengeMinDays", c.revengeMinDays);
    f("revengeMaxDays", c.revengeMaxDays); f("retaliationMinDays", c.retaliationMinDays);
    f("retaliationMaxDays", c.retaliationMaxDays); f("planLifetimeDays", c.planLifetimeDays);
    f("reinforceAtPressure", c.reinforceAtPressure); f("pressureDecayPerDay", c.pressureDecayPerDay);
    f("controlLossPerCamp", c.controlLossPerCamp);
}
template <class F>
void eachBool(Config& c, F&& f) {
    f("cheatDeathAlways", c.cheatDeathAlways); f("simulationMayKill", c.simulationMayKill);
}
} // namespace

Config Config::fromJson(const Json& j) {
    Config c;
    eachInt(c, [&](const char* k, int& v) { if (j.has(k)) v = j.i(k, v); });
    eachBool(c, [&](const char* k, bool& v) { if (j.has(k)) v = j.flag(k, v); });
    if (j.has("riseGoalXpMult")) c.riseGoalXpMult = j.num("riseGoalXpMult");
    if (j.has("finalMethods")) {
        c.finalMethods.clear();
        for (auto& m : j["finalMethods"].elements()) c.finalMethods.push_back(fromString<Method>(m.asString()));
    }
    if (j.has("methodSurvival")) {
        for (auto& kv : j["methodSurvival"].items()) c.methodSurvival[fromString<Method>(kv.first)] = kv.second.asNumber(1);
    }
    auto ivec = [&](const char* k, std::vector<int>& v) {
        if (!j.has(k)) return;
        v.clear();
        for (auto& x : j[k].elements()) v.push_back(x.asInt());
    };
    ivec("seatsPerRegion", c.seatsPerRegion);
    ivec("followersByRank", c.followersByRank);
    while (c.seatsPerRegion.size() < 6) c.seatsPerRegion.push_back(1);
    while (c.followersByRank.size() < 6) c.followersByRank.push_back(0);
    return c;
}

Json Config::toJson() const {
    Config copy = *this;
    Json j;
    eachInt(copy, [&](const char* k, int& v) { j[k] = v; });
    eachBool(copy, [&](const char* k, bool& v) { j[k] = v; });
    j["riseGoalXpMult"] = riseGoalXpMult;
    Json fm = Json::array();
    for (auto m : finalMethods) fm.push(toString(m));
    j["finalMethods"] = fm;
    Json ms;
    for (auto& kv : methodSurvival) ms[toString(kv.first)] = kv.second;
    j["methodSurvival"] = ms;
    Json s = Json::array(); for (int v : seatsPerRegion) s.push(v); j["seatsPerRegion"] = s;
    Json f = Json::array(); for (int v : followersByRank) f.push(v); j["followersByRank"] = f;
    return j;
}

} // namespace lw
