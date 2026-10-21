# Changelog

## 0.2.0 — 2026-10-21 (recon build)
- `GotSignatures.ini`: 96 game signatures from Master Thyself 1.10.0 (credit Musky / Destinate / cfemen), plus Living War's own SetMaxHealth and SetHealth. **98/98 verified** against Steam build 0x6A3AAFBD.
- Plugin now pinned to that build (`ExpectedBuildTimestamp`). Any other build stays in Chronicle mode only.
- **Recon mode** (read-only): MinHook pass-through hooks on SetMaxHealth and SetHealth log every character's spawn, damage and death to `LivingWar\census.csv`. F11 adds a "leader killed" mark. Nothing in the game is changed.
- Static CRT for the whole build. MinHook 1.3.3 is compiled into the plugin.
- `tools/re/check_sigs.py` (offline signature checker) and `tools/re/fn.py` (function disassembler), plus `docs/RE_NOTES.md`.

## 0.1.0 — 2026-10-21 (prototype)
- Engine-agnostic Nemesis core (C++17, no dependencies):
  - Commander identities: names, origins, archetypes, 8 personality facets, strengths, weaknesses, fears
  - Memories (fading and long-term), scars from the actual wound, history log, ties (superior, subordinate, rival, blood brother, kin), goals
  - Cheat death (enRAGE formula), with per-method survival multipliers and optional always-final methods
  - Learned adaptations: paranoid, fire ward / fire fear, shield wall, standoff wary, spread out, poison wary, horse wary, emboldened, vengeful, ambusher
  - CombatProfile: rank-scaled health, accuracy and damage, resistances, retinue composition, tactics, panic triggers, hate stance, dialogue key
  - Mongol decimal hierarchy (Arban → Jaghun → Mingghan → Tumen → Khan's Noyan) under protected story leaders (Khotun Khan, The Eagle)
  - Rank XP and promotions limited by seats, succession with kin chance and cascading vacancies, rivalries (feuds, duels, disgrace and reassignment)
  - Revenge ambushes, returns after recovery, retaliation for liberated camps, hunting parties on high alert, reinforcements under pressure
  - Regions gated by campaign act, with Mongol control, pressure and alert
- Persistence: per-slot JSON saves, atomic writes, 3 rotating backups, recovery from corrupt files, RNG state saved
- `LivingWar` facade and `IGameAdapter` boundary, including an encounter tracker for raw game events
- HTML hierarchy viewer
- `lwsim` simulator (demo, simulate, encounter, advance, show, list, export)
- ASI plugin: process/build checks, signature probe, Chronicle mode hotkeys, real-time world clock stand-in, F9 hierarchy
- Technical report, integration plan, CI workflow (Linux tests + Windows plugin build)
