# Native ARM64 Build

## Supported workflow

Numbers Station builds directly on AArch64 Linux. Docker, Windows, WSL, and
Tyra are deliberately excluded.

The build has two explicit dependencies:

1. `NATIVE_PS2DEV` supplies the native AArch64 EE cross-compiler produced by
   `ps2_arm64_toolchain` Version 1.0.
2. `PS2SDK` supplies PS2 target headers, startup files, and libraries.

The installed PS2 startup object is resolved beside `PS2SDK` and passed to the
linker explicitly because the native compiler and SDK intentionally live under
different installation roots.

Keeping these inputs separate prevents an older PS2DEV directory from
silently selecting an x86-64 compiler on the ARM64 host.

## Verification

Run:

```sh
make verify
```

The target checks:

- the host reports `aarch64`;
- the EE compiler exists and executes;
- `file` identifies the compiler as an AArch64 executable;
- the PS2SDK linkfile, debug header, debug, graph, pad, and draw libraries
  exist;
- the installed PS2 startup object exists.

The compiler is invoked by its canonical name:

```text
mips64r5900el-ps2-elf-gcc
```

## Reproducible build procedure

```sh
make clean
make
sha256sum out/bin/numbers_station.elf
make clean
make
sha256sum out/bin/numbers_station.elf
```

Matching hashes demonstrate that repeated clean builds from the same source,
toolchain, PS2SDK, and environment produce the same ELF.

Generated files stay below `out/` and are never used as source inputs.

## Runtime verification

The Milestone 021 ELF should initially display:

```text
Numbers Station
Milestone 021
```

After approximately three seconds it should display:

```text
Numbers Station

Main Menu

Press START
```

## PCSX2 radio-inspection verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm Splash appears immediately and Main Menu replaces it after about
   three seconds without input.
4. Press START once and confirm Gameplay displays `Radio Inspection`, player
   world X/Y, viewport X/Y, the terminal, barrier, three pale notes, existing
   obstacles, and textured player.
5. Confirm the sprite contains a blue checkerboard, pale border and N-shaped
   emblem. Its corner markers must be red at top-left, green at top-right, blue
   at bottom-left, and yellow at bottom-right.
6. Confirm the initial status reads
   `Objective: Activate the relay terminal.`
7. Read any accessible note before activating the terminal and confirm the
   initial objective remains unchanged.
8. Activate the relay terminal and confirm the objective changes automatically
   to `Objective: Search beyond the barrier.`
9. Visit the notes at `(260,260)`, `(980,680)`, and `(1320,940)` in any order.
   Confirm each is pale, non-solid, viewport-relative, and prompts only during
   its exact half-open overlap.
10. Open each note with newly pressed Cross and verify that it displays its own
   complete text: `MAINTENANCE LOG`, `FIELD NOTE`, or `FREQUENCY RECORD`.
11. Confirm the objective remains `Search beyond the barrier.` until all three
    distinct notes have been opened, then changes to
    `Objective: Investigate the transmission source.`
12. For every note, hold movement and Cross while reading. Confirm the shared
   overlay stays open and all Gameplay values remain frozen.
13. Newly press Circle after each reading. Confirm Gameplay resumes at the exact
   player and viewport position without a movement jump.
14. Reopen every note and confirm the final objective remains unchanged and no
    note is removed, marked, collected, or made unavailable.
15. Confirm all three notes retain the same visual and interaction dimensions.
16. Find the cyan extraction marker at `(1360,1020)` and confirm it is
    non-solid, viewport-relative, and visible within its fixed 80×80 bounds.
17. Enter the extraction zone before reading all three notes. Confirm it has no
    effect and Gameplay continues normally.
18. Complete the existing objective chain, leave the zone if necessary, and
    enter it after the final objective appears. Confirm a dark overlay displays
    `MISSION COMPLETE`, `Transmission source located.`, `End of prototype.`,
    and `Press CIRCLE to return.`
19. Hold movement and Cross while the completion overlay is open. Confirm
    player, viewport, collision, terminal, barrier, documents, and objective
    progress remain frozen and the overlay stays open.
20. Newly press Circle and confirm Gameplay resumes at the exact player and
    viewport position without a movement jump or immediate overlay reopening.
21. Leave and re-enter the extraction zone. Confirm the completion overlay may
    be opened and dismissed again without storing completion state.
22. Find the magenta radio marker at `(1510,1120)` and confirm it is
    viewport-relative, clipped, and non-solid.
23. Remain outside its 180-pixel center-distance radius and confirm no radio
    transmission is displayed.
24. Enter the radius and confirm `RADIO: 41729 08314 55190` appears near the top
    while movement and all Gameplay behavior continue normally.
25. Remain nearby and confirm the display advances every three seconds through
    `66302 19447 82016`, `09531 77208 43665`, `28144 00973 11852`, and
    `55017 36490 70221`, then loops to the first entry.
26. Leave range and confirm the radio display disappears immediately. Re-enter
    and confirm the sequence deterministically restarts at its first entry.
27. Confirm radio proximity and sequence changes do not alter the current
    objective, world state, documents, or extraction behavior.
28. On the first in-range frame and while stationary, confirm the additional
    line reads `SIGNAL STABLE`.
29. Move measurably toward the radio and confirm it reads
    `SIGNAL STRENGTHENING` while the existing transmission timing continues.
30. Move measurably away from the radio without leaving range and confirm it
    reads `SIGNAL WEAKENING`.
31. Move approximately tangentially or stop and confirm changes within the fixed
    tolerance read `SIGNAL STABLE` without affecting movement.
32. Open a document or completion overlay and confirm direction comparison
    pauses with radio timing, then resumes safely after dismissal.
33. Leave and re-enter range. Confirm the first status is stable and the
    transmission sequence still restarts from its first entry.
34. Confirm no direction status appears outside radio range and no status
    changes objectives or world state.
35. Confirm no inspection prompt appears outside the radio marker's half-open
    bounds and entering those bounds without pressing Cross does nothing.
36. Hold Cross before entering the radio bounds and confirm the overlay does not
    open. Release Cross and confirm `Press CROSS to inspect` while overlapping.
37. Newly press Cross and confirm a dark full-screen overlay displays
    `TRANSMISSION SOURCE`, `The receiver is disconnected.`,
    `The numbers continue.`, and `Press CIRCLE to close.`
38. Hold movement, Cross, and unrelated buttons. Confirm the overlay remains
    open and player, viewport, interactions, objectives, and world state freeze.
39. Confirm radio transmission timing and direction comparison remain paused
    while inspection is open.
40. Newly press Circle and confirm normal Gameplay resumes at the exact player
    and viewport position without a movement jump.
41. Confirm the previous transmission index, timing, and direction state resume
    safely, then dismiss and reopen the inspection repeatedly.
42. Confirm inspection never changes objective, extraction, terminal, barrier,
    document, radio, or world state.
43. Hold each direction against its world boundary. Confirm player X remains
    within `0–1576`, player Y remains within `0–1176`, viewport X remains within
    `0–960`, viewport Y remains within `0–752`, and no outside area is exposed.
44. Confirm display/draw indices are always opposite and alternate `0/1` then
    `1/0` as frames are presented.
45. Visit all four edges and multiple corners. Confirm corner markers appear,
    clipped landmarks remain inside the display, the environment fills every
    frame, and player placement remains correct when the viewport is clamped.
46. Disconnect or disable the controller near the notes, extraction zone, or
    radio and while either overlay is open. Confirm no accidental opening or
    dismissal, drift, crash, or hang.
47. Confirm Gameplay remains active after repeated reading, extraction, radio
    reception, and inspection; rendering continues after the textured player,
    and START does not exit it.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
