# Numbers Station

Numbers Station is a PlayStation 2 homebrew project targeting original PS2
hardware and PCSX2. Development is native on a Raspberry Pi 4 running Ubuntu
26.04 ARM64. Docker, Windows, WSL, and Tyra are not part of the build workflow.

Milestone 026 adds one fixed controls screen to Main Menu. Newly pressed
Triangle opens it, newly pressed Circle returns to the ordinary Main Menu, and
START retains its established fresh-Gameplay behavior while controls are closed.

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
- `src/input.*` — primary controller buttons and centered left-stick values
- `src/video.*` — double-buffered display, primitives, one test texture, and
  diagnostic text
- `src/state_manager.*` — active-state ownership and transitions
- `src/splash_state.*` — timed launch screen
- `src/main_menu_state.*` — menu display, fixed controls screen, and START response
- `src/gameplay_state.*` — Gameplay lifecycle, procedural scene, viewport,
  fixed collision, terminal/barrier interaction, three world documents, and
  fixed objective, radio reception and inspection, transmission-source
  inspection, completion, Main Menu return request, and local pause controls
- `src/player.*` — combined digital/analog movement, world clamping, and sprite
- `src/frame_timer.*` — elapsed gameplay frame time with stall clamping
- `docs/` — current engineering documentation and archived milestone evidence
- `out/` — generated objects and ELF files; ignored by Git

New architecture and content directories will be introduced only when a
milestone requires them.
