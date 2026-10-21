# Ghost of Tsushima: Living War

A Nemesis system for Ghost of Tsushima Director's Cut (PC, Steam). Mongol commanders get persistent identities, remember Jin, cheat death, carry scars, learn counter-tactics, rise through a living chain of command, and come back for revenge. Its rules are adapted, with permission, from **enRAGE**, the Living World & Nemesis System for Red Dead Redemption 2.

## Status (v0.1.0, prototype)

| Part | State |
|---|---|
| Nemesis core: identities, personalities, memories, scars, cheat death, adaptations, hate/fear stances, rank XP, succession, rivalries, revenge plans, region control, retaliation, reinforcement | **Working**: 22 automated tests |
| Milestone: *one commander who remembers Jin* (spawn → fight → survive → save → reload → return changed) | **Working** in simulation (`lwsim demo`) |
| Persistence separate from game saves (atomic, backups, corruption recovery) | **Working** |
| Hierarchy viewer (HTML) | **Working** |
| ASI plugin: loads, logs, probes signatures, Chronicle mode (hotkey-reported fights), F9 hierarchy | **Written, not yet tested in-game** |
| Automatic detection of fights in-game | **Not yet**: needs reverse-engineering (see `docs/INTEGRATION_PLAN.md`) |
| Changing enemies in-game (health, retinue, AI) | **Not yet**: same |

## Layout
```
core/        engine-agnostic C++17 library (no game code)
  include/lw/  model, config, world (rules), profile, save, adapter, living_war (facade)
plugin/      Ghost of Tsushima ASI plugin (Windows) + LivingWar.ini, GotSignatures.ini
tools/lwsim/ command-line simulator + mock game adapter
tools/re/    .NET metadata/IL reader used to study enRAGE (analysis tool only)
tests/       unit tests
docs/        technical report, integration plan
```

## How it fits together
```
 Game hooks / Chronicle hotkeys
          │  onLeaderSpawned, onLeaderDamaged, onLeaderKilled, onCampLiberated, onGameMinutes…
          ▼
   LivingWar (facade) ── EncounterTracker turns raw events into an EncounterReport
          │
          ▼
   Nemesis rules (world.cpp) ── World state ── SaveManager (JSON, per slot)
          │
          ▼ buildProfile()
   CombatProfile (health, resistances, retinue, tactics, stance, taunt)
          │
          ▼
   IGameAdapter::applyProfile / spawnCommander / notify   (GotAdapter, MockAdapter, NullAdapter)
```

## Build & try it
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/lw_tests
./build/lwsim demo                 # the milestone
./build/lwsim simulate --days 60   # a war without you watching
```
Windows plugin: open the folder in Visual Studio 2022 (CMake project) and build `LivingWar`, or push to GitHub and let `.github/workflows/build.yml` build the `.asi` file. See `INSTALL.md`.

## Docs
- `docs/TECHNICAL_REPORT.md`: what's possible in GoT modding today, and how enRAGE works
- `docs/INTEGRATION_PLAN.md`: the step-by-step plan to hook the game
- `ROADMAP.md`, `CHANGELOG.md`, `CREDITS.md`
