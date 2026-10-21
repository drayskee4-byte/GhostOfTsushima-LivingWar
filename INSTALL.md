# Installing Living War (prototype, Chronicle mode)

> This build doesn't change enemies in-game yet. It runs the Nemesis world alongside your game: you report fights with hotkeys, and Living War tracks who survived, who remembers you, and who is coming for revenge. Back up your saves anyway. The mod never touches them.

## 1. Find your game folder
Steam → right-click Ghost of Tsushima DIRECTOR'S CUT → Manage → Browse local files. It's the folder that contains `GhostOfTsushima.exe`.

## 2. Install Ultimate ASI Loader (skip if you already use ASI mods)
Download the x64 release from https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases, rename its `dinput8.dll` to **`d3d12.dll`**, and put it next to `GhostOfTsushima.exe`.
*If a `d3d12.dll` is already there from GoT Mod Launcher, the two can't be installed side by side. Ultimate ASI Loader also works under the name `version.dll`, so try that name instead.*

## 3. Copy Living War
Copy the `plugins` folder into the game folder:
```
Ghost of Tsushima DIRECTOR'S CUT\
  GhostOfTsushima.exe
  d3d12.dll                  (Ultimate ASI Loader)
  plugins\
    LivingWar.asi
    LivingWar.ini
    GotSignatures.ini
```

## 4. Play
Start the game and load your story save. After about 10 seconds, `plugins\LivingWar\LivingWar.log` appears.

| Key | Does |
|---|---|
| F7 | You're fighting a Mongol leader. Living War picks who he is (it may be someone you've met before). |
| Numpad 0 | You killed 5 of his men |
| Numpad 1 / 2 / 3 / 4 | You killed him with blade / stealth / fire / arrows (he may cheat death) |
| Numpad 5 | He fled |
| Numpad 6 | You spared him |
| Numpad 7 | He killed you |
| Numpad 8 | You escaped |
| F8 | Cycle region (Izuhara → Toyotama → Kamiagata → Iki) |
| F9 | Open the hierarchy page (who's alive, who hates you, what they remember) |
| F10 | Save now (it also saves after every fight) |

News appears in `plugins\LivingWar\feed.txt`. Set `CampaignAct` in `LivingWar.ini` as you progress.

## Recon (helps build the automatic version)
On the tested game build, Living War also quietly logs every character that spawns, gets hit or dies to `plugins\LivingWar\census.csv`. It only watches and changes nothing.
To help: load a story save, attack a Mongol camp, **kill the camp leader, then press F11 straight away**. Do this for two or three camps, quit the game, and send `census.csv` and `LivingWar.log`.

## Uninstall
Delete `plugins\LivingWar.asi` (and the `LivingWar` folder if you don't want the world kept). Remove `d3d12.dll` only if no other ASI mod needs it.

## What to send back if something goes wrong
`plugins\LivingWar\LivingWar.log`. It includes your game build and the signature probe.
