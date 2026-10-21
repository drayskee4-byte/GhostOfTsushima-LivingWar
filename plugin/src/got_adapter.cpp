#include "got_adapter.hpp"

#include <windows.h>

#include <cstdio>
#include <ctime>
#include <fstream>
#include <vector>

#include "ini.hpp"
#include "lw/util.hpp"

namespace lwp {

std::string GotAdapter::probe(const std::string& path) {
    image_ = mainModule();
    std::string rep = lw::fmt("GhostOfTsushima.exe image base %p size 0x%zx build timestamp 0x%08X\n",
                              reinterpret_cast<void*>(image_.base), image_.size, image_.timestamp);
    Ini ini;
    if (!ini.load(path)) return rep + "GotSignatures.ini not found - no hooks attempted.\n";
    const auto* sec = ini.section("signatures");
    if (!sec) return rep + "GotSignatures.ini has no [signatures] section.\n";
    int found = 0, missing = 0, unknown = 0;
    for (auto& kv : *sec) {
        // value: pattern | a | b   (see GotSignatures.ini header)
        Signature s;
        s.name = kv.first;
        std::vector<std::string> parts;
        {
            std::string v = kv.second, cur;
            for (char ch : v) { if (ch == '|') { parts.push_back(cur); cur.clear(); } else cur += ch; }
            parts.push_back(cur);
            for (auto& p : parts) { while (!p.empty() && p.back() == ' ') p.pop_back(); while (!p.empty() && p.front() == ' ') p.erase(0, 1); }
        }
        s.pattern = parts[0];
        try { if (parts.size() > 1) s.a = std::stoi(parts[1], nullptr, 0); if (parts.size() > 2) s.b = std::stoi(parts[2], nullptr, 0); } catch (...) {}
        if (s.pattern.empty()) { rep += "  " + s.name + ": UNKNOWN (no signature yet)\n"; ++unknown; continue; }
        auto addr = resolve(image_, s);
        if (addr) {
            resolved_[s.name] = *addr;
            rep += lw::fmt("  %s: FOUND at exe+0x%llx\n", s.name.c_str(), static_cast<unsigned long long>(*addr - image_.base));
            ++found;
        } else {
            rep += "  " + s.name + ": NOT FOUND (wrong game build or bad signature)\n";
            ++missing;
        }
    }
    rep += lw::fmt("signatures: %d found, %d not found, %d unknown\n", found, missing, unknown);
    return rep;
}

lw::Capabilities GotAdapter::capabilities() const {
    // Nothing is wired to game memory yet. Flip these only when the code that uses each
    // resolved address exists and has been tested in-game.
    return {};
}

bool GotAdapter::applyProfile(lw::ActorHandle, const lw::CombatProfile& p) {
    lw::Log::info("applyProfile (not yet possible in-game): " + p.title);
    return false;
}

std::optional<lw::ActorHandle> GotAdapter::spawnCommander(const lw::SpawnRequest& r) {
    lw::Log::info("spawnCommander requested for " + r.profile.title + " - spawning is not implemented yet");
    return std::nullopt;
}

void GotAdapter::notify(const std::string& title, const std::string& body) {
    // No in-game overlay yet: append to a feed file the player can keep open, and beep.
    std::ofstream f(feedPath_, std::ios::app);
    if (f) {
        std::time_t t = std::time(nullptr);
        char ts[16];
        std::strftime(ts, sizeof ts, "%H:%M", std::localtime(&t));
        f << ts << "  " << title << " - " << body << "\n";
    }
    MessageBeep(MB_ICONASTERISK);
    lw::Log::info("NOTIFY " + title + ": " + body);
}

} // namespace lwp
