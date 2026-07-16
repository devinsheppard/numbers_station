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

The Milestone 015 ELF should initially display:

```text
Numbers Station
Milestone 015
```

After approximately three seconds it should display:

```text
Numbers Station

Main Menu

Press START
```

## PCSX2 readable-document verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm Splash appears immediately and Main Menu replaces it after about
   three seconds without input.
4. Press START once and confirm Gameplay displays `Readable Document`, player
   world X/Y, viewport X/Y, the terminal, barrier, pale field note, existing
   obstacles, and textured player.
5. Confirm the sprite contains a blue checkerboard, pale border and N-shaped
   emblem. Its corner markers must be red at top-left, green at top-right, blue
   at bottom-left, and yellow at bottom-right.
6. Activate the terminal and cross the disabled barrier. Confirm all existing
   prompt, latch, visual, and collision behavior remains correct.
7. Approach the pale field note beyond the barrier. Confirm it is non-solid,
   scrolls with the world, and displays no prompt outside its exact bounds.
8. Hold Cross before entering and confirm the note does not open. Release Cross
   while overlapping, confirm `Press CROSS to read.`, then newly press Cross.
9. Confirm a dark full-screen overlay displays the complete fixed field-note
   text and no gameplay world rendering.
10. Hold movement and Cross while reading. Confirm player, viewport, terminal,
    barrier, and collision state remain unchanged and the overlay stays open.
11. Newly press Circle. Confirm Gameplay resumes immediately at the exact player
    and viewport position without a movement jump.
12. Leave and return to the note, then open and dismiss it again. Confirm it
    remains visible and repeatable with no collected or read state.
13. Hold each direction against its world boundary. Confirm player X remains
   within `0–1576`, player Y remains within `0–1176`, viewport X remains within
   `0–960`, viewport Y remains within `0–752`, and no outside area is exposed.
14. Confirm display/draw indices are always opposite and alternate `0/1` then
   `1/0` as frames are presented.
15. Visit all four edges and multiple corners. Confirm corner markers appear,
    clipped landmarks remain inside the display, the environment fills every
    frame, and player placement remains correct when the viewport is clamped.
16. Disconnect or disable the controller near the note and while reading.
    Confirm no accidental opening or dismissal, drift, crash, or hang.
17. Confirm Gameplay remains active after repeated reading, rendering continues
    after the textured player, and START does not exit it.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
