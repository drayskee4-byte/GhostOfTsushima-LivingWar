#include "lw/util.hpp"

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <mutex>
#include <sstream>

namespace lw {

namespace {
std::mutex g_mu;
Log::Sink g_sink;
std::string g_file;
LogLevel g_level = LogLevel::Info;
const char* levelName(LogLevel l) {
    switch (l) { case LogLevel::Debug: return "DEBUG"; case LogLevel::Info: return "INFO";
                 case LogLevel::Warn: return "WARN"; case LogLevel::Error: return "ERROR"; }
    return "?";
}
} // namespace

void Log::setSink(Sink s) { std::lock_guard<std::mutex> l(g_mu); g_sink = std::move(s); }
void Log::setFile(const std::string& p) { std::lock_guard<std::mutex> l(g_mu); g_file = p; }
void Log::setLevel(LogLevel lv) { std::lock_guard<std::mutex> l(g_mu); g_level = lv; }

void Log::write(LogLevel lv, const std::string& msg) {
    std::lock_guard<std::mutex> l(g_mu);
    if (lv < g_level) return;
    if (g_sink) g_sink(lv, msg);
    if (!g_file.empty()) {
        std::ofstream f(g_file, std::ios::app);
        if (f) {
            std::time_t t = std::time(nullptr);
            char ts[32];
            std::strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", std::localtime(&t));
            f << ts << " [" << levelName(lv) << "] " << msg << "\n";
        }
    }
}

int Rng::range(int lo, int hi) {
    if (hi <= lo) return lo;
    uint64_t span = static_cast<uint64_t>(hi - lo) + 1;
    return lo + static_cast<int>(next() % span);
}
bool Rng::chance(int pct) { return range(0, 99) < pct; }
double Rng::real() { return static_cast<double>(next() >> 11) * (1.0 / 9007199254740992.0); }
std::string Rng::state() const {
    std::ostringstream o;
    o << eng_;
    return o.str();
}
bool Rng::setState(const std::string& s) {
    std::istringstream i(s);
    std::mt19937_64 e;
    i >> e;
    if (i.fail()) return false;
    eng_ = e;
    return true;
}

int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

std::string fmt(const char* f, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, f);
    std::vsnprintf(buf, sizeof buf, f, ap);
    va_end(ap);
    return buf;
}

} // namespace lw
