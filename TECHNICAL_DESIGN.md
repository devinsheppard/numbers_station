# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Application architecture

Milestone 004 uses four small modules:

- `main` delegates process execution to the application.
- `application` owns startup, running state, frame count, update/render
  sequencing, and orderly shutdown.
- `input` owns PS2 controller modules, port 0/slot 0, pad DMA storage, polling,
  and button state transitions.
- `video` owns display initialization, vertical-blank synchronization, and the
  diagnostic screen.

There is no generic engine, device abstraction, remapping layer, gameplay
action map, scene system, or other speculative input architecture.

## Startup sequence

1. `main` calls `application_run`.
2. The application initializes SIF RPC.
3. The video module initializes PS2SDK's debug display.
4. The input module loads `rom0:SIO2MAN` and `rom0:PADMAN`.
5. The input module initializes `libpad` and opens controller port 0, slot 0,
   using a 256-byte, 64-byte-aligned DMA buffer.
6. The application enters the running state only after subsystem startup
   succeeds.

A missing controller is not a startup error. The opened port remains available
for later connection and the screen reports `Not connected` until a stable pad
can be read.

## Polling and state transitions

Input is polled exactly once during each application update. `padRead` is only
called when `padGetState` reports `PAD_STATE_STABLE` or
`PAD_STATE_FINDCTP1`. Other states return immediately without waiting.

PS2 pads report pressed buttons as zero bits. The input module normalizes that
into an active-high 16-bit mask and calculates:

```text
pressed  = current AND NOT previous
released = previous AND NOT current
```

When a controller becomes unavailable, current and pressed are cleared, held
buttons appear once in released, and the last-pressed label is preserved.
Repeated disconnected frames remain empty and non-blocking.

The label table covers D-pad directions, Cross, Circle, Square, Triangle,
Start, Select, L1, R1, L2, R2, L3, and R3. When multiple buttons become pressed
in one poll, the first matching table entry becomes the diagnostic label.

## Game loop

Each iteration performs:

```text
poll controller and update transitions
increment frame counter
wait for vertical blank and render diagnostics
```

Input does not stop the application. The existing shutdown path remains for a
future milestone; current execution continues until reset or power-off.

## Shutdown sequence

If future logic clears the application running flag, the controller port is
closed, `libpad` is ended, the display is cleared, and the application returns
successfully.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libgraph`, `libdma`, and C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
