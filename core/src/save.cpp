#include "lw/save.hpp"

#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace lw {

namespace {
std::string sanitize(const std::string& slot) {
    std::string s;
    for (char c : slot) s += (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') ? c : '_';
    return s.empty() ? "default" : s;
}
} // namespace

bool SaveManager::ensureDir(const std::string& p) {
    std::error_code ec;
    fs::create_directories(p, ec);
    return fs::is_directory(p, ec);
}

std::optional<std::string> SaveManager::readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return std::nullopt;
    std::ostringstream o;
    o << f.rdbuf();
    return o.str();
}

bool SaveManager::writeAtomic(const std::string& path, const std::string& content) {
    std::string tmp = path + ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        f << content;
        f.flush();
        if (!f) return false;
    }
    std::error_code ec;
    fs::rename(tmp, path, ec);          // atomic replace on the same volume
    if (ec) {
        fs::remove(path, ec);
        fs::rename(tmp, path, ec);
    }
    return !ec;
}

std::string SaveManager::pathFor(const std::string& slot) const {
    return (fs::path(dir_) / "saves" / sanitize(slot) / "world.json").string();
}

bool SaveManager::exists(const std::string& slot) const {
    std::error_code ec;
    return fs::exists(pathFor(slot), ec);
}

bool SaveManager::save(const World& w, const std::string& slot) {
    std::string path = pathFor(slot);
    if (!ensureDir(fs::path(path).parent_path().string())) {
        Log::error("save: cannot create folder for " + path);
        return false;
    }
    std::error_code ec;
    if (fs::exists(path, ec)) {
        for (int i = backups_; i >= 1; --i) {
            std::string from = i == 1 ? path : path + ".bak" + std::to_string(i - 1);
            std::string to = path + ".bak" + std::to_string(i);
            if (fs::exists(from, ec)) fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
        }
    }
    bool ok = writeAtomic(path, w.toJson().dump(1));
    if (ok) Log::info("saved world (day " + std::to_string(w.day) + ") to " + path);
    else Log::error("save failed: " + path);
    return ok;
}

std::optional<World> SaveManager::load(const std::string& slot) {
    std::string path = pathFor(slot);
    std::vector<std::string> candidates{path};
    for (int i = 1; i <= backups_; ++i) candidates.push_back(path + ".bak" + std::to_string(i));
    for (auto& p : candidates) {
        auto txt = readFile(p);
        if (!txt) continue;
        try {
            Json j = Json::parse(*txt);
            if (j.i("schemaVersion", 0) > kSchemaVersion) {
                Log::error("save " + p + " was written by a newer Living War (schema " + std::to_string(j.i("schemaVersion")) + ") - not loading it");
                return std::nullopt;
            }
            World w = World::fromJson(j);
            if (p != path) Log::warn("loaded backup " + p + " because the main save was unusable");
            return w;
        } catch (const std::exception& e) {
            std::error_code ec;
            for (int n = 1; n < 100; ++n) {
                std::string aside = p + ".unreadable-" + std::to_string(n);
                if (!fs::exists(aside, ec)) { fs::copy_file(p, aside, ec); break; }
            }
            Log::error("save " + p + " unreadable (" + e.what() + ") - kept a copy, trying the next backup");
        }
    }
    return std::nullopt;
}

} // namespace lw
