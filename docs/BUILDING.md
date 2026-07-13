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

The Milestone 007 ELF should initially display:

```text
Numbers Station
Milestone 007
```

After approximately three seconds it should display:

```text
Numbers Station

Main Menu

Press START
```

## PCSX2 Gameplay verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm Splash appears immediately and Main Menu replaces it after about
   three seconds without input.
4. Press START once and confirm Gameplay immediately displays `Pixel Rendering
   Active`, player X/Y pixel coordinates, bounded delta time, D-pad
   instructions, and a filled colored rectangle.
5. Hold each D-pad direction separately. Confirm the rectangle and matching
   coordinate move smoothly in all four directions.
6. Test diagonal and opposing directions. Confirm diagonal movement changes
   both axes and opposing inputs cancel on their axis.
7. Hold each direction against its boundary. Confirm X remains within `0–616`,
   Y remains within `0–424`, and the entire 24×24 rectangle remains visible.
8. Pause or stall emulation temporarily, then resume. Confirm displayed delta
   does not exceed 100 ms and the player advances no more than 18 pixels from
   that update.
9. Confirm Gameplay remains active after movement and START does not exit it.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
