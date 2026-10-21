# Technical Report: Ghost of Tsushima PC modding & enRAGE analysis

*Living War, 21 Oct 2026. Phase 1 (ecosystem) and Phase 2 (enRAGE) findings.*

## 1. The game

| Fact | Status | Source |
|---|---|---|
| Ghost of Tsushima Director's Cut runs on Sucker Punch's proprietary engine, ported by Nixxes. There is no official modding support or scripting API. | Verified | Nixxes/Sony announcement; 3DJuegos |
| DirectX 12 is the only renderer. | Verified | Simple Mod Menu page |
| Executable: `GhostOfTsushima.exe` (x64). Native mods target specific Steam builds: `1053.8.1212.1408` (GoT Native Loader) and `1053.9.0623.1807` / Steam build 23879181 (Shadow Strike Story). | Verified | Nexus 845; GitHub ezzysx/ShadowStrikeStory |
| Epic Games version: no native mod lists it as tested. Shadow Strike Story says "Epic and Proton are not validated" and Master Thyself says "no epic support yet". | Verified, treat Epic as unsupported | same |
| Archives are `.psarc` files, but their magic is `DSAR`, so standard PSARC unpackers fail. | Verified | UnPSARC issue #6 |
| Mesh (`.xmesh`) and texture (`.sps`) formats have been reversed far enough for a Blender import/export pipeline. | Verified | GoT Blender Toolkit (Nexus 819) |
| The game shows "PlayStation Network communication error" popups when it detects cheat tables (an internal integrity check). | Reported by Master Thyself's author | Nexus 735 |

## 2. Existing modding tools

| Tool | What it actually does | Relevance to Living War |
|---|---|---|
| **Ultimate ASI Loader** (ThirteenAG, MIT), installed as `d3d12.dll` | Loads `plugins/*.asi` into the game. The GoT Native Loader proof of concept proved runtime code mods work. | **Our loader.** Living War ships as `LivingWar.asi`. |
| **GoT Native Loader – FOV PoC** (Nexus 845) | ASI plugin with a pattern file (`GoTPatterns.txt`) that patches FOV. | Shows that signature-based patching works on this game. |
| **Shadow Strike Story** (Nexus/GitHub, source public) | ASI built with MinHook. Checks the exe timestamp, size and code signatures before hooking, stays inactive on unknown builds, logs to `plugins/*.log`, and excludes Legends. | **Best template for our hook layer.** Its safety pattern is copied in our plugin design. |
| **GoT Mod Launcher** (Nexus 860, Dave349234) | Its own `d3d12.dll` proxy reads `plugins/load.txt` and loads ScriptHook (Lua scripts from `scripts/`), MeshHooker and TextureInjector. PSARC overrides go in `cache_pc/psarc/`. Single-player only. | **Conflicts** with Ultimate ASI Loader, since both are `d3d12.dll`. Its Lua API is **not publicly documented**. Its permissions forbid modifying or reusing it without asking. |
| **Simple GoT Mod Menu** (Nexus 355) | Proxy-loaded DLL with an in-game menu drawn through the game's own DXGI swapchain (INSERT key). AMD frame generation must be off. | Shows an in-game overlay is possible. Entity spawning by this menu is **not verified**: its page doesn't describe spawning. |
| **Master Thyself** (Nexus 735, Musky; Cheat Engine table) | Entity spawner (type, faction, quantity, despawn, teleport), summon remapping, a CSV-driven enemy composition loader, and per-enemy modifiers: armour, helmet, elemental attunement, invisibility, tethering, mounted, health/stagger multipliers. Mostly aimed at **Legends**, with some story-mode features (story multiplayer, first-person). | **The richest reverse-engineering of enemies and spawning that exists.** Its permissions allow modification and conversion **with credit**, but not sale. Builds on Destinate's and cfemen's tables. |
| **Transmog System** (Nexus 884) | ASI plugin with an in-game menu (F5/F6), loaded by Ultimate ASI Loader. | Another working ASI example. |

## 3. Feasibility of what Living War needs

| Capability | Verdict | Notes |
|---|---|---|
| Load our own native code in the game | **Exists** | Ultimate ASI Loader |
| Persistent mod save, separate from game saves | **Exists, built** | Our own JSON saves in `plugins/LivingWar/saves/<slot>/`, with atomic writes and rotating backups |
| Spawn enemies of a chosen type | **Exists, in a Cheat Engine table** | Master Thyself's spawner. Needs converting from CE scripts to C++ signatures. |
| Change an enemy's health, armour, stagger or attunement | **Exists, in a Cheat Engine table** | Same source |
| Enumerate live enemies and read health, faction and type | **Partly exists** | The CE table must read these to work. Offsets need extracting. |
| Detect a leader's death and who caused it | **Needs a new hook** | Can be done by polling health from the entity list, without a function detour |
| Damage type (fire, arrow, stealth, standoff) | **Needs new reverse-engineering** | Hardest part. Fallback: infer it from Jin's equipped tool or stance when health drops. |
| Steer AI tactics (flee, ambush, refuse standoff) | **Needs new reverse-engineering, may not be feasible at first** | Retinue changes (more archers or shields) are feasible through the spawner. Deep AI flags are a stretch goal. |
| Campaign act and region | **Needs a new hook** | Stand-in: a setting in `LivingWar.ini` |
| Lua scripting interface | **Undocumented** | GoT Mod Launcher's ScriptHook exists, but its API isn't public |
| Overlay UI in-game | **Possible later** | Simple Mod Menu and Transmog draw menus. Stand-in: the F9 hierarchy page plus `feed.txt`. |

Conclusion: the right path is **native ASI plugin + polling observer + spawner/modifier calls ported from the Cheat Engine research**. Everything above the adapter line (the whole Nemesis simulation) is built and tested now, independent of the game.

## 4. enRAGE 0.9.5.1 analysis (Phase 2)

**Package:** `scripts/NemesisMod.dll` (3.6 MB, .NET Framework 4.8, assembly `NemesisMod` 0.9.5.1) and `Newtonsoft.Json.dll` 13.0.3, with LICENSE, THIRD-PARTY, CHANGELOG and an install guide. It runs on ScriptHookRDR2 and ScriptHookRDR2DotNet V2 (`ScriptHookRDRNetAPI` 2.2.0). The mod is roughly 1,211 types and 9,806 methods. It was examined with a metadata and IL reader written for this project (`tools/re/`), not a commercial decompiler.

### Where the Nemesis system lives
| enRAGE class | Role | Living War equivalent |
|---|---|---|
| `NemesisRecord` (123 fields) | Persistent identity: rank and XP, memories, scars, ties, goals, habits, fear and hate, counters (times ambushed, cheated death, spared...) | `lw::Commander` |
| `Memory`, `Scar`, `HistoryEntry`, `Tie`, `Goal`, `Thought` | Memory model | `Memory`, `Scar`, `HistoryEntry`, `Tie`, `Goal` |
| `Mind` | 10 personality facets (Anger, Bravery, Pride, Humour, Honesty, Ambition, Cruelty, Greed, Loyalty, Piety); goals Avenge/Prove/Rise/Rich/Settle/Protect; emotions Fear/Shame/Grief/Joy/Pride | 8 facets; goals Avenge/RestoreHonor/Rise/Protect/Survive |
| `Traits`, `Quirks`, `Personalities`, `Tempers` | Trades, skills (marksman, brawler, tough, tracker, horseman, leader) and per-temper reaction tables | strengths, weaknesses, fears |
| `NemesisMod.ResolveNemesisDeath`, `PickScar`, `ScarFromWound` | Cheat death: `min(Max, Base + PerRank×(rank−1))`, fatal head/neck wounds are final, and the scar is chosen from the actual wound bone and weapon | `Nemesis::resolve` (same formula, wound-to-scar) |
| `GainRankXp`, `TryRankUp`, `XpNeed`, `ApplyRankStats` | XP = max(1, round(amount × (0.5 + Ambition/100) × (Rise? 1.5:1))), none if Settle; need = 2×rank; rank 5 cap. Health 300+100·rank, accuracy 25+10·rank. | `Nemesis::gainXp`, `tryRankUp`, `buildProfile` |
| `Hate` (40 vengeful / 70 murderous), `ArmForHate` | Hate tiers decide whether he wants a fistfight, brings a blade, or wants you dead, and change weapons and combat attributes | `Stance` in `CombatProfile` |
| `RunSuccession`, `FillLeaderSeat`, `LeaderSeat` | Leader seats refill after 2–6 days, with a 35% kin chance | `PendingSeat`, `fillSeat` |
| `Plan`, `Agenda`, `UpdatePlans`, `StartAmbush` | Scheduled ambushes, jobs and raids | `Plan` (revenge_ambush, return, retaliation, hunting_party) |
| `WorldState`, `WorldEvent`, `Gangs`, wars, territory `Control` | Gang wars, takeovers, treasuries, events | `Region` control, pressure and alert, `WorldEvent`, rivalries, reassignment |
| `SafeFile`, `SavePoint*` | Atomic writes with `.bak` and `.unreadable-*` copies, plus mod-state snapshots tied to game save times | `SaveManager` (same safety model). Save points are on the roadmap. |
| `Balance`, `NemesisSettings` (173 settings) | Tuning | `Config` + `LivingWar.json` (enRAGE defaults marked `[enRAGE]`) |

### RDR2-specific (not portable)
Everything that touches `RDR2.*`: `PED`/`ENTITY`/`WEAPON` natives, ped models, outfits, blips, towns, saloons, doors (`doors.json`), telegrams, newspapers, interrogation, horses and wagons, X-ray bullet-impact anatomy, and RDR2's cause-of-death and bone queries. Its text content (names, taunts, backstories) is Wild West material and doesn't fit Mongol officers.

### Ported to Living War (with the author's permission, credited)
The algorithms and tuning numbers above. Code was re-implemented in C++. The decompiled C# is **not** included in this repository, and no enRAGE text is reused.

### Licence note
enRAGE's public `LICENSE.txt` forbids reusing its code or content in other projects. The project owner reports **separate permission from the enRAGE author**. Keep a copy of that permission (for example `docs/permission/`) before publishing.
