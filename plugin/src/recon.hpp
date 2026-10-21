// Recon: read-only census of characters, to learn how the game marks camp/patrol leaders.
//
// Hooks SetMaxHealth (exe+0x69F600 on build 0x6A3AAFBD) and SetHealth (exe+0x69F3D0) with
// pass-through detours: they call the original function unchanged and only record what they saw.
// Nothing in the game is modified. Results go to plugins/LivingWar/census.csv.
#pragma once
#include <cstdint>
#include <string>

namespace lwp {

struct ReconAddresses {
    uintptr_t base = 0;
    uintptr_t setMaxHealth = 0;
    uintptr_t setHealth = 0;
    uintptr_t playerGlobal = 0;   // address of the global that holds the player Character*
};

bool reconStart(const ReconAddresses& a, const std::string& csvPath);   // installs hooks
void reconFlush();                                                     // write buffered samples
void reconMark(const std::string& note);                               // player-pressed marker
void reconStop();
std::string reconStatus();

} // namespace lwp
