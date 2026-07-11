# TECHNICAL DESIGN

## Platform

NUMBERS STATION targets PlayStation 2 homebrew using legal open-source tools.

Supported runtime targets:

- Original PlayStation 2 hardware
- Current PCSX2

No other runtime targets are planned until the PS2 version is complete.

## Language

- Modern C++ where supported by PS2SDK
- C only where required by PS2SDK libraries

## Engine Direction

The engine will be written specifically for this project. External engines such as Unity, Godot, Unreal, SDL game engines, Python game frameworks, DirectX, OpenGL, and Vulkan are not permitted.

Initial system boundaries:

- Rendering
- Input
- Game state
- World state
- Asset loading
- UI
- Audio

Project 001 does not implement these systems beyond reserving the structure.

## Build System

GNU Make is the project build interface. The stable command set is:

- `make`
- `make clean`
- `make run`
- `make iso`

The current Makefile verifies required environment variables and tools before attempting compilation.

## Toolchain Verification Status

Verification failed on 2026-07-11. See `docs/TOOLCHAIN_VERIFICATION.md`.

## Revision Log

### 2026-07-11 - Project 001

Created initial technical boundaries and blocked runtime implementation pending PS2 toolchain installation.

