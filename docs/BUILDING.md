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

The Milestone 012 ELF should initially display:

```text
Numbers Station
Milestone 012
```

After approximately three seconds it should display:

```text
Numbers Station

Main Menu

Press START
```

## PCSX2 static-collision verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm Splash appears immediately and Main Menu replaces it after about
   three seconds without input.
4. Press START once and confirm Gameplay displays `Static Collision`, player
   world X/Y, viewport X/Y, blocked-axis diagnostics, four bright solid
   obstacles, and a textured square sprite.
5. Confirm the sprite contains a blue checkerboard, pale border and N-shaped
   emblem. Its corner markers must be red at top-left, green at top-right, blue
   at bottom-left, and yellow at bottom-right.
6. Approach every solid rectangle from each reachable side with cardinal
   D-pad and analog movement. Confirm the player stops flush without entering
   the obstacle and can move away immediately.
7. Apply diagonal D-pad and analog movement at horizontal and vertical walls.
   Confirm the blocked axis stops while the unobstructed axis continues, with
   no jitter.
8. Test the L-shaped obstacle corner from multiple directions. Confirm the
   fixed X-then-Y resolution is repeatable and does not embed or trap the player.
9. Combine and oppose D-pad and analog input against obstacles. Confirm mixed
   input cannot force penetration and maximum speed remains unchanged.
10. Hold each direction against its world boundary. Confirm player X remains
   within `0–1576`, player Y remains within `0–1176`, viewport X remains within
   `0–960`, viewport Y remains within `0–752`, and no outside area is exposed.
11. Confirm display/draw indices are always opposite and alternate `0/1` then
   `1/0` as frames are presented.
12. Visit all four edges and multiple corners. Confirm corner markers appear,
    clipped landmarks remain inside the display, the environment fills every
    frame, and player placement remains correct when the viewport is clamped.
13. Disconnect or disable the controller and confirm neutral axes, no drift,
    crash, or hang. Reconnect it and confirm buttons and analog movement return.
14. Pause or stall emulation temporarily, then move at full speed toward each
    obstacle orientation. Confirm the 18-pixel capped displacement cannot cross
    a 36-pixel-or-thicker obstacle.
15. Confirm Gameplay remains active after movement, rendering continues after
    the textured player, and START does not exit it.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
