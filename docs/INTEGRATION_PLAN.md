# Game Integration Plan

The core is finished and tested without the game. This plan covers connecting it to Ghost of Tsushima. Each step ends with something you can test in-game, and nothing is marked working until it has been.

## Step 0 — Loader + Chronicle mode (ready to try)
- Install Ultimate ASI Loader as `d3d12.dll`, plus `plugins/LivingWar.asi`.
- **Test:** `plugins/LivingWar/LivingWar.log` shows `==== Living War 0.1.0 starting ====`, the exe build timestamp, and the signature probe report. F9 opens the hierarchy page, and F7 then a numpad key resolves an encounter.
- This confirms: the plugin loads, the file locations are writable, and the save and world loop work during real play sessions.

## Step 1 — Get the reverse-engineering data
Master Thyself's Cheat Engine table already contains the spawner and the enemy modifiers. A `.CT` file is XML: its Auto Assembler scripts hold the byte patterns (`aobscanmodule`) and structure offsets.
1. Download Master Thyself from Nexus and send the `.CT` file.
2. Extract each `aobscanmodule(name, GhostOfTsushima.exe, <pattern>)` and the offsets used, into `GotSignatures.ini` and an `Offsets` table, crediting Musky, Destinate and cfemen.
3. **Test:** the startup log reports `FOUND` for each one on your Steam build.

## Step 2 — Observer adapter (read-only, no detours)
Poll the enemy list every ~250 ms from the plugin thread:
- New enemy whose type is a leader or captain → `onLeaderSpawned(handle, region)`
- Health drops → `onLeaderDamaged(...)`, with the damage type inferred (see Step 4)
- Health reaches 0 → `onLeaderKilled`
- Enemy despawns or gets far away while alive → `onLeaderFled` or `onEncounterEnded`
- Player health reaches 0 → `onPlayerKilled(nearest tracked leader)`
- Region from the player's position (four bounding areas)

Read-only polling can't corrupt game state. Worst case it misses events.
**Test:** fight a camp leader. The log shows the encounter and `hierarchy.html` updates with no hotkeys pressed.

## Step 3 — Apply profiles (write)
Using the Master Thyself modifier paths:
- `healthMult` → write max/current health. `staggerResist` → stagger multiplier.
- `resist[Arrow]` → helmet/armour flags. `resist[Burn]` → armour or attunement equivalent.
- Retinue → spawner calls (type, faction, count) near the leader.

**Test:** a commander with `shield_wall` arrives with extra shield bearers, and an emboldened one takes visibly longer to kill.

## Step 4 — How Jin fought
Infer `Method` at the moment health drops:
- Player was undetected / in an assassination animation → Stealth
- Standoff state active → Standoff
- Equipped tool was fire/sticky bomb or the enemy is burning → Fire
- Bow equipped → Ranged
- Black powder → Explosive

Each needs one game-state read (the player's equipped item and stance), to be found with Cheat Engine.

## Step 5 — In-game UI
Draw the "He remembers you" banner and a commander card over the game. The ImGui-on-swapchain approach used by Simple Mod Menu and Transmog shows it's possible. Until then, use `feed.txt` and the F9 page.

## Step 6 — Behaviour (stretch)
Map `tactics` (`refuse_standoff`, `never_alone`, `ambush`, `hangs_back`) to AI flags. This needs AI-state reverse-engineering that doesn't exist yet. The retinue and stat changes from Step 3 already make memories visible in combat.

## Safety rules for every hook
- Only enable hooks when the exe timestamp matches a tested build (`ExpectedBuildTimestamp`). Otherwise stay in Chronicle mode.
- Never run in Legends or online. Never touch the anti-cheat.
- Every hook logs success or failure. Signatures are data (`GotSignatures.ini`), never hard-coded addresses.
- Original game files are never modified.
