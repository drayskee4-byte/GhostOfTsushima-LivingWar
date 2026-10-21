// Living War - logging and deterministic random numbers.
#pragma once
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <vector>

namespace lw {

// ------------------------------------------------------------ logging
enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3 };

class Log {
public:
    using Sink = std::function<void(LogLevel, const std::string&)>;
    static void setSink(Sink s);
    static void setFile(const std::string& path);   // also appends to this file
    static void setLevel(LogLevel l);
    static void write(LogLevel l, const std::string& msg);
    static void debug(const std::string& m) { write(LogLevel::Debug, m); }
    static void info(const std::string& m) { write(LogLevel::Info, m); }
    static void warn(const std::string& m) { write(LogLevel::Warn, m); }
    static void error(const std::string& m) { write(LogLevel::Error, m); }
};

// ------------------------------------------------------------ rng
// Seeded, serialisable RNG so a save + seed reproduces the same world.
class Rng {
public:
    explicit Rng(uint64_t seed = 1) { reseed(seed); }
    void reseed(uint64_t seed) { seed_ = seed; eng_.seed(seed); }
    int range(int lo, int hi);            // inclusive
    bool chance(int percent);             // 0..100
    double real();                        // [0,1)
    template <class T> const T& pick(const std::vector<T>& v) { return v[static_cast<size_t>(range(0, static_cast<int>(v.size()) - 1))]; }
    uint64_t seed() const { return seed_; }
    std::string state() const;                 // full engine state, for saving
    bool setState(const std::string& s);       // false if malformed
private:
    uint64_t next() { return eng_(); }
    std::mt19937_64 eng_;
    uint64_t seed_ = 1;
};

int clampi(int v, int lo, int hi);
std::string fmt(const char* f, ...);

} // namespace lw
