// Recon: read-only census of characters, to learn how the game marks camp/patrol leaders.
//
// Hooks SetMaxHealth (exe+0x69F600 on build 0x6A3AAFBD) and SetHealth (exe+0x69F3D0) with
// pass-through detours: they call the original function unchanged and only record what they saw.
// Nothing in the game is modified. Results go to plugins/LivingWar/census.csv.
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace lwp {

struct ReconAddresses {
    uintptr_t base = 0;
    uintptr_t setMaxHealth = 0;
    uintptr_t setHealth = 0;
    uintptr_t playerGlobal = 0;   // address of the global that holds the player Character*
    // Leader detection (learned from in-game recon, docs/RE_NOTES.md):
    float leaderRatio = 1.75f;    // the game re-sets a leader's max health to base x 1.75 right after spawn
    std::vector<int> leaderTypes; // extra type ids (+0x3E84) always treated as leaders
};

// Events produced by the observer hooks (game thread) and consumed by the plugin thread.
struct GameEvent {
    enum Kind { LeaderSpawned, LeaderDamaged, LeaderDied, LeaderGone, PlayerDied, SoldierDied } kind;
    uintptr_t entity = 0;
    int type = 0;
    float maxHp = 0, oldHp = 0, newHp = 0;
};
std::vector<GameEvent> observerTakeEvents();

bool reconStart(const ReconAddresses& a, const std::string& csvPath);   // installs hooks
void reconFlush();                                                     // write buffered samples
void reconMark(const std::string& note);                               // player-pressed marker
void reconStop();
std::string reconStatus();

} // namespace lwp
