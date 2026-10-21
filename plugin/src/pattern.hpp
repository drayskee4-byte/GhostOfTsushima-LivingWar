// Byte-signature scanning over the game's executable image.
// Signatures are IDA-style: "48 8B 05 ?? ?? ?? ?? 48 85 C0" (?, ?? and ** are wildcards). They live in GotSignatures.ini
// so a game patch can be handled by editing data, not recompiling.
#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace lwp {

struct Signature {
    std::string name;
    std::string pattern;      // empty = unknown (not yet reverse-engineered)
    int a = 0;                // b == 0: address = match + a
    int b = 0;                // b != 0: address = match + b + int32(match + a)  (RIP-relative)
};

struct ModuleImage {
    uintptr_t base = 0;
    size_t size = 0;
    uint32_t timestamp = 0;   // PE TimeDateStamp - identifies the game build
};

ModuleImage mainModule();
bool parsePattern(const std::string& text, std::vector<int>& out);   // -1 = wildcard
std::optional<uintptr_t> scan(const ModuleImage& m, const std::vector<int>& pat);
std::optional<uintptr_t> resolve(const ModuleImage& m, const Signature& s);

} // namespace lwp
