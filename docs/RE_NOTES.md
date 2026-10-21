# Reverse-engineering notes: Ghost of Tsushima Director's Cut (Steam)

Build verified: **PE timestamp 0x6A3AAFBD**, `GhostOfTsushima.exe` 29,324,752 bytes (sha256 prefix `ed0931b6f6d3ca38`), downloaded 21 Oct 2026.
The exe is not packed, so static disassembly works (`tools/re/fn.py`).

## Signature check
All **96/96** signatures from Master Thyself 1.10.0 match this build, each at exactly one location
(`docs/sigcheck_6A3AAFBD.json`, produced by `tools/re/check_sigs.py`).

## Confirmed by disassembly
| What | Where | Notes |
|---|---|---|
| SetMaxHealth(Character* c, float max) | exe+0x69F600 | Writes `c+0x1970`. If current health is above the new max, calls SetHealth. Master Thyself's `MaxHealthUpdate` site is +0x16 inside it. Called for every character as it is set up. **Plan: spawn detection + per-commander health multiplier.** |
| SetHealth(Character* c, float hp) | exe+0x69F3D0 | Reads old `c+0x1974`, writes new. Every hit and heal goes through it. **Plan: damage events, death (hp ≤ 0), per-commander damage resistance.** |
| Character max health | `+0x1970` (float) | |
| Character current health | `+0x1974` (float) | |
| Character state id | `+0x3E84` (int) | Written by SetEntityArchetype (exe+0x69DD50). **Live data: it changes during a fight (e.g. 14 ↔ 15), so it's a combat/AI state, not a fixed type.** |
| Character type | `vtable[+0x20]()` returns an int; `0xF` takes a special path in RecomputeEntityDisposition | Meaning still unknown |
| Player character pointer | global at exe+0x34C6F20 (`pPlayer`) | RecomputeEntityDisposition compares against it |
| Entity attribute table | global at exe+0x33B4950 (`pEntityAttrTable`) | |
| Local user slot | byte at exe+0x3481A74 | Health functions compare it to `[c+0xB8]+4` (network ownership) |

## Open questions (need live data)
1. **Which characters are camp or patrol leaders?** Candidates: archetype id (`+0x3E84`), vtable type, definition hash, max health. → Recon build logs these for every spawned character, then you fight a leader.
2. **Damage type and attacker.** SetHealth has no source argument. Look one frame up (caller address), or at a damage-info struct the caller holds.
3. **Region from the player position.** Need the transform offset (Master Thyself's `ApplyEntityTransform` shows the layout).
4. **Game thread.** Spawning must run on the game thread (Master Thyself refuses otherwise). Find a per-frame function to pump our queue.

## First in-game run (21 Oct 2026, prologue)
- Plugin loaded through Ultimate ASI Loader v10.0.0 as `d3d12.dll`. 98/98 signatures resolved at runtime. Recon hooks installed. No crash.
- 208 characters seen. Classes by vtable: `0x112DBC0` (humans) and `0x125E700` (lower health; possibly horses or animals). Player vtable is `0x1202ED0`.
- Max health seen: Jin 225 → 245. Soldiers 50, 70, 75, 120, 138, 150, 200, 400. One character with 2000 (likely the prologue boss).
- **SetHealth is NOT the hit path.** 336 of 337 "damage" records came from inside SetMaxHealth (caller exe+0x69F723 clamping health). Normal hits update health somewhere else. Only killing blows reach SetHealth, from exe+0x6A128E.
- **KillCharacter = exe+0x6A1250** `(Character* c, int64 rdx /* -1 = no killer? */, int64 r8, uint8 r9b)`: SetHealth(c, 0) → exe+0x69DF00(c, 0) → exe+0x69ED10(c, 3, 0x80000000, 0). **Next: hook it to get deaths and, possibly, killer ids.**
- Only two direct writes to `+0x1974` exist (SetHealth, plus a constant 100.0 at exe+0x689CEA). The per-hit damage path must work through another component (`[c+0x16F8]` is passed to health-change notifiers). To find.
