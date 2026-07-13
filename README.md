# Numbers Station

Numbers Station is a PlayStation 2 homebrew project targeting original PS2
hardware and PCSX2. Development is native on a Raspberry Pi 4 running Ubuntu
26.04 ARM64. Docker, Windows, WSL, and Tyra are not part of the build workflow.

Milestone 009 adds one procedurally generated 32×32 RGBA texture and the minimum
GS state needed to upload it and draw a textured quad. The moving player is the
test sprite; the existing double-buffered frame lifecycle remains unchanged.

## Requirements

- AArch64 Ubuntu host
- GNU Make and `file`
- The verified native toolchain from `ps2_arm64_toolchain` Version 1.0
- A compatible PS2SDK installation

By default, the Makefile expects the repositories to be siblings and resolves
the native compiler from:

```text
../ps2_arm64_toolchain/build/ps2dev
```

It uses `PS2SDK` from the environment when set, otherwise:

```text
/usr/local/ps2dev/ps2sdk
```

Override either location without editing the repository:

```sh
make NATIVE_PS2DEV=/path/to/native/ps2dev PS2SDK=/path/to/ps2sdk
```

## Build

```sh
make verify
make
```

The ELF is written to:

```text
out/bin/numbers_station.elf
```

Additional commands:

```sh
make info
make clean
make run PCSX2=/path/to/pcsx2
```

`make run` is optional and requires a locally installed PCSX2 executable.
ISO packaging is not implemented; the ELF can be loaded directly by
PCSX2 or suitable PS2 homebrew launch software.

See `docs/BUILDING.md` for verification and reproducibility details.

## Repository layout

- `src/main.c` — process entry point
- `src/application.*` — startup, loop control, and shutdown sequencing
- `src/input.*` — primary PS2 controller initialization and button transitions
- `src/video.*` — double-buffered display, primitives, one test texture, and
  diagnostic text
- `src/state_manager.*` — active-state ownership and transitions
- `src/splash_state.*` — timed launch screen
- `src/main_menu_state.*` — menu display and START response
- `src/gameplay_state.*` — Gameplay lifecycle and player ownership
- `src/player.*` — pixel position, normalized movement, clamping, and test sprite
- `src/frame_timer.*` — elapsed gameplay frame time with stall clamping
- `docs/` — current engineering documentation and archived milestone evidence
- `out/` — generated objects and ELF files; ignored by Git

New architecture and content directories will be introduced only when a
milestone requires them.
