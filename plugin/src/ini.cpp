#include "ini.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace lwp {

namespace {
std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}
std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}
} // namespace

bool Ini::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line, sec;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        if (line.front() == '[' && line.back() == ']') { sec = lower(trim(line.substr(1, line.size() - 2))); continue; }
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string v = trim(line.substr(eq + 1));
        auto sc = v.find(" ;");
        if (sc != std::string::npos) v = trim(v.substr(0, sc));
        data_[sec][lower(trim(line.substr(0, eq)))] = v;
    }
    return true;
}

std::string Ini::get(const std::string& s, const std::string& k, const std::string& def) const {
    auto a = data_.find(lower(s));
    if (a == data_.end()) return def;
    auto b = a->second.find(lower(k));
    return b == a->second.end() ? def : b->second;
}
int Ini::getInt(const std::string& s, const std::string& k, int def) const {
    std::string v = get(s, k, "");
    if (v.empty()) return def;
    try { return std::stoi(v, nullptr, 0); } catch (...) { return def; }
}
bool Ini::getBool(const std::string& s, const std::string& k, bool def) const {
    std::string v = lower(get(s, k, ""));
    if (v.empty()) return def;
    return v == "1" || v == "true" || v == "yes" || v == "on";
}
const std::map<std::string, std::string>* Ini::section(const std::string& s) const {
    auto a = data_.find(lower(s));
    return a == data_.end() ? nullptr : &a->second;
}

} // namespace lwp
