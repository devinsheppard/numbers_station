# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Application architecture

Milestone 003 uses three small modules:

- `main` is the process entry point and delegates to the application.
- `application` owns startup, the running state, update/render sequencing, and
  orderly shutdown.
- `video` owns PS2SDK display initialization, vertical-blank synchronization,
  screen rendering, and display cleanup.

The boundaries reflect current runtime responsibilities. There is no generic
engine, renderer interface, game-state framework, asset system, input layer,
audio layer, or third-party game engine.

## Startup sequence

1. `main` calls `application_run`.
2. The application initializes SIF RPC.
3. The application asks the video module to initialize PS2SDK's debug display.
4. Startup sets the application running state only after initialization
   succeeds.

## Application lifecycle

The application owns a single `running` flag. A failed startup returns a
nonzero process result. A successful startup enters the loop. When future
shutdown logic clears the flag, the loop ends, video shutdown runs, and the
application returns zero.

No current input or gameplay condition changes the flag, so Milestone 003
continues until the console or emulator is reset or powered off.

## Game loop

Each iteration performs exactly two phases:

```text
update application state
render one frame
```

Rendering waits for vertical blank, clears the screen, and writes the three
framework verification lines. Frame counters and timing diagnostics were
omitted because they are not needed to prove the loop and would add timing and
formatting code with no architectural value yet.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libdebug`, `libgraph`, `libdma`, and C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.

## Build interface

GNU Make provides `make`, `make verify`, `make info`, `make clean`, and the
optional `make run`. All generated output is isolated under `out/`.
