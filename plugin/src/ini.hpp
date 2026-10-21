// Tiny INI reader/writer (sections, key=value, ; comments).
#pragma once
#include <map>
#include <string>

namespace lwp {

class Ini {
public:
    bool load(const std::string& path);
    std::string get(const std::string& section, const std::string& key, const std::string& def = "") const;
    int getInt(const std::string& section, const std::string& key, int def) const;
    bool getBool(const std::string& section, const std::string& key, bool def) const;
    const std::map<std::string, std::string>* section(const std::string& s) const;
private:
    std::map<std::string, std::map<std::string, std::string>> data_;
};

} // namespace lwp
