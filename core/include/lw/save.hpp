// Living War - persistence. Mod state lives in its own folder, never inside the game's save files.
//
//   <dataDir>/saves/<slot>/world.json        current state
//   <dataDir>/saves/<slot>/world.json.bak1.. rotating backups (bak1 = newest)
//
// Writes go to a .tmp file first and are renamed into place, so a crash mid-write can't corrupt
// the save. On load, an unreadable file is kept aside as .unreadable-<n> and the next backup is tried
// (the same safety approach enRAGE's SafeFile uses).
#pragma once
#include <optional>
#include <string>

#include "lw/world.hpp"

namespace lw {

class SaveManager {
public:
    explicit SaveManager(std::string dataDir, int backups = 3) : dir_(std::move(dataDir)), backups_(backups) {}

    bool save(const World& w, const std::string& slot);
    std::optional<World> load(const std::string& slot);   // nullopt = no usable save for this slot
    bool exists(const std::string& slot) const;
    std::string pathFor(const std::string& slot) const;
    const std::string& dataDir() const { return dir_; }

    // File helpers (also used by the export/report code).
    static bool writeAtomic(const std::string& path, const std::string& content);
    static std::optional<std::string> readFile(const std::string& path);
    static bool ensureDir(const std::string& path);

private:
    std::string dir_;
    int backups_;
};

} // namespace lw
