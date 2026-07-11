# NUMBERS STATION

NUMBERS STATION is a long-term PlayStation 2 homebrew project targeting original PlayStation 2 hardware and current PCSX2.

This repository is not disposable. Changes must preserve existing decisions and working behavior unless a future project prompt explicitly approves a breaking change.

## Project Scope

- Genre: psychological horror, survival horror, investigation, puzzle, exploration
- Platform: Sony PlayStation 2
- Tooling: legal open-source PS2 homebrew tools only
- Target style: commercial-quality late 2003 / early 2004 PS2 title

No non-PS2 platforms are in scope until the PS2 version is complete.

## Required Toolchain

The project assumes:

- Windows 11
- WSL2 Ubuntu
- Git
- GNU Make
- PS2DEV
- PS2SDK
- gsKit
- Open-source PS2 libraries

## Current Status

Project 001 foundation has been created, but PS2 toolchain verification failed on this machine:

- WSL2 is available, but no Linux distributions are installed.
- PS2DEV and PS2SDK environment variables are not available.
- `ee-gcc`, `ee-g++`, `ee-ld`, and GNU Make for the PS2 build were not found.
- gsKit and controller library availability could not be verified.

Because required tools are missing, no PS2 application code has been written and no successful build is claimed.

## Build Commands

The build commands are reserved now so future sessions keep a stable interface:

```sh
make
make clean
make run
make iso
```

At present, `make` stops during toolchain verification until PS2DEV, PS2SDK, gsKit, and the PS2 compiler tools are installed.

## Repository Layout

- `docs/` - supporting project documentation and verification notes
- `src/` - application entry code after toolchain verification succeeds
- `include/` - public project headers
- `engine/` - project-specific engine code
- `game/` - gameplay-specific code
- `assets/` - source assets
- `world/` - world design and structured world data
- `tools/` - project support scripts
- `build/` - local build output
- `iso/` - ISO staging and output
- `tests/` - verification tests and future host-side checks
- `third_party/` - vendored open-source dependencies, if approved

