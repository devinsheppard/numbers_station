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

The Milestone 026 ELF should initially display `Numbers Station` and
`Milestone 009`. After approximately three seconds it should display the normal
Main Menu and `Press START` prompt. The temporary red and green isolation markers
have been removed.

## PCSX2 Main Menu controls verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm Splash appears immediately and Main Menu replaces it after about
   three seconds without input.
4. Press START once and confirm Gameplay displays `Pause Return to Main Menu`, player
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
16. Find the cyan transmission-source marker at `(1360,1020)` and confirm it is
    non-solid, viewport-relative, and visible within its fixed 80×80 bounds.
17. Enter the source area before reading all three notes. Confirm it has no
    effect and Gameplay continues normally.
18. Read all three notes without inspecting the radio and confirm the objective
    becomes `Inspect the stationary receiver.` Enter the source area and confirm
    no prompt or completion is available.
19. Inspect the stationary radio and confirm its overlay reads `RADIO RECEIVER`.
    Confirm the objective becomes `Investigate the transmission source.` Enter
    the cyan source area, confirm `Press CROSS to inspect`, and verify that only
    newly pressed Cross opens a dark overlay displaying
    `MISSION COMPLETE`, `Transmission source located.`, `End of prototype.`,
    and `Press CIRCLE to return.`
20. Hold movement and Cross while the completion overlay is open. Confirm
    player, viewport, collision, terminal, barrier, documents, and objective
    progress remain frozen and the overlay stays open.
21. Newly press Circle and confirm the application returns directly to Main
    Menu without resuming a completed Gameplay frame.
22. Press START and confirm a fresh Gameplay entry restores the initial player,
    viewport, terminal, barrier, document, receiver, source, objective, radio,
    overlay, and completion-request state.
23. Find the magenta radio marker at `(1510,1120)` and confirm it is
    viewport-relative, clipped, and non-solid.
24. Remain outside its 180-pixel center-distance radius and confirm no radio
    transmission is displayed.
25. Enter the radius and confirm `RADIO: 41729 08314 55190` appears near the top
    while movement and all Gameplay behavior continue normally.
26. Remain nearby and confirm the display advances every three seconds through
    `66302 19447 82016`, `09531 77208 43665`, `28144 00973 11852`, and
    `55017 36490 70221`, then loops to the first entry.
27. Leave range and confirm the radio display disappears immediately. Re-enter
    and confirm the sequence deterministically restarts at its first entry.
28. Confirm radio proximity and sequence changes do not alter documents or
    source state; only valid radio inspection records receiver confirmation.
29. On the first in-range frame and while stationary, confirm the additional
    line reads `SIGNAL STABLE`.
30. Move measurably toward the radio and confirm it reads
    `SIGNAL STRENGTHENING` while the existing transmission timing continues.
31. Move measurably away from the radio without leaving range and confirm it
    reads `SIGNAL WEAKENING`.
32. Move approximately tangentially or stop and confirm changes within the fixed
    tolerance read `SIGNAL STABLE` without affecting movement.
33. Open a document or completion overlay and confirm direction comparison
    pauses with radio timing, then resumes safely after dismissal.
34. Leave and re-enter range. Confirm the first status is stable and the
    transmission sequence still restarts from its first entry.
35. Confirm no direction status appears outside radio range and no status
    changes objectives or world state.
36. Confirm no inspection prompt appears outside the radio marker's half-open
    bounds and entering those bounds without pressing Cross does nothing.
37. Hold Cross before entering the radio bounds and confirm the overlay does not
    open. Release Cross and confirm `Press CROSS to inspect` while overlapping.
38. Newly press Cross and confirm a dark full-screen overlay displays
    `RADIO RECEIVER`, `The receiver is disconnected.`,
    `The numbers continue.`, and `Press CIRCLE to close.`
39. Hold movement, Cross, and unrelated buttons. Confirm the overlay remains
    open and player, viewport, interactions, objectives, and world state freeze.
40. Confirm radio transmission timing and direction comparison remain paused
    while inspection is open.
41. Newly press Circle and confirm normal Gameplay resumes at the exact player
    and viewport position without a movement jump.
42. Confirm the previous transmission index, timing, and direction state resume
    safely, then dismiss and reopen the inspection repeatedly.
43. Confirm repeat inspection does not toggle confirmation, disturb radio state,
    or change terminal, barrier, document, player, or viewport state.
44. Hold each direction against its world boundary. Confirm player X remains
    within `0–1576`, player Y remains within `0–1176`, viewport X remains within
    `0–960`, viewport Y remains within `0–752`, and no outside area is exposed.
45. Confirm display/draw indices are always opposite and alternate `0/1` then
    `1/0` as frames are presented.
46. Visit all four edges and multiple corners. Confirm corner markers appear,
    clipped landmarks remain inside the display, the environment fills every
    frame, and player placement remains correct when the viewport is clamped.
47. Disconnect or disable the controller near the notes, source area, or
    radio and while either overlay is open. Confirm no accidental opening or
    dismissal, drift, crash, or hang.
48. Complete at least two Main Menu-to-Gameplay-to-completion cycles. Confirm
    no stale transition, retained progress, input lock, black screen, rendering
    corruption, timing jump, crash, or lockup occurs.
49. During ordinary Gameplay, newly press START and confirm a dark panel displays
    `PAUSED` and `Press START to resume` over the still-visible world.
50. While paused, hold movement and press Cross or Circle. Confirm player,
    viewport, collision, interactions, objective, and radio state remain frozen.
51. Wait several seconds, newly press START, and confirm exact position and state
    resume without a movement, viewport, timing, or transmission jump.
52. Press START while a document, stationary receiver, or completion overlay is
    open and confirm pause does not open or toggle. Confirm their existing Circle
    behavior remains unchanged.
53. Return to Main Menu through completion, press START, and confirm the fresh
    Gameplay entry is unpaused. Repeat pause/resume and full completion cycles
    without stale state, repeated toggles, corruption, crash, or lockup.
54. Confirm no red or green isolation marker remains. Verify Splash, Main Menu,
    Gameplay diagnostics, documents, receiver, completion, and pause text stay
    continuously visible without alternating-frame disappearance, stale glyphs,
    GS-state damage, or rendering corruption.
55. Pause ordinary Gameplay and confirm the panel displays both `Press START to
    resume` and `Press CIRCLE for Main Menu`.
56. Newly press Circle while paused. Confirm Main Menu is rendered next without
    a resumed Gameplay frame or repeated transition.
57. Press START from Main Menu and confirm a fresh Gameplay session resets all
    position, viewport, progression, interaction, overlay, radio, objective,
    pause, and request state.
58. Confirm held Circle does not leak into pause or Main Menu, Circle remains
    inactive during ordinary Gameplay, and document, receiver, and completion
    overlays retain their established Circle behavior.
59. On the ordinary Main Menu, confirm `Press START to begin` and `Press TRIANGLE
    for controls` remain continuously visible and stable.
60. Newly press Triangle and confirm the fixed controls screen lists D-pad/left
    stick movement, Cross interaction, Circle overlay dismissal, START pause and
    resume, and the Circle return instruction.
61. While controls are open, confirm START, Triangle, Cross, and unrelated
    buttons do nothing. Newly press Circle and confirm only the ordinary Main
    Menu returns.
62. Newly press START from the ordinary Main Menu and confirm fresh Gameplay
    initialization remains unchanged. If Triangle and START are newly pressed in
    the same update, confirm Triangle takes priority.
63. Return to Main Menu from completion and pause exit. Confirm controls begin
    closed after each entry and after repeated menu/gameplay cycles, with no held
    input leakage, stale modal state, flicker, corruption, crash, or lockup.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
