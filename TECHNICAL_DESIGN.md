# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Milestone 002 architecture

The current program is intentionally one C entry point using PS2SDK directly.
It initializes SIF RPC, initializes PS2SDK's debug display, prints two lines,
and remains running.

There is no engine layer, rendering abstraction, asset system, game state,
input layer, audio layer, or third-party game engine. Those boundaries will be
introduced only in milestones that require them.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers, startup linkfile, `libdebug`, and C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are not
dependencies of the Milestone 002 executable.

## Build interface

GNU Make provides `make`, `make verify`, `make info`, `make clean`, and the
optional `make run`. All generated output is isolated under `out/`.
