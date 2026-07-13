# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Rendering approach

Milestone 007 uses PS2SDK `libdraw`, `libpacket`, `libgraph`, and GIF DMA for a
minimal immediate-mode 2D path. The video module configures a 640×448, 32-bit
framebuffer at GS address zero, matching the existing PS2SDK debug display. A
single reusable 64-qword packet is sufficient for current operations.

At startup, the existing debug display initializes the GS and text path. The
video module then initializes GIF DMA and submits `draw_setup_environment` for
the same framebuffer with depth buffering disabled.

Each frame:

1. Wait for vertical blank.
2. Clear the framebuffer to a dark solid color with `draw_clear`.
3. Submit a filled rectangle with `draw_rect_filled` when Gameplay renders the
   player.
4. Draw diagnostic text through PS2SDK `libdebug` on the same framebuffer.

Every primitive packet ends with `draw_finish` and is complete before its
storage is reused. There is no render queue, material, texture, sprite, camera,
or general renderer layer.

## Frame timing

Gameplay owns one `FrameTimer`. Initialization records
`GetTimerSystemTime`; each Gameplay update subtracts the previous PS2 bus-clock
value and converts the elapsed ticks to seconds.

Elapsed time is capped at 100 milliseconds before conversion. An emulator
pause or host stall therefore cannot apply more than 0.1 seconds of movement
in one update. The diagnostic screen displays the bounded delta in integer
milliseconds.

This is a Gameplay-local frame timer, not a scheduler, profiler, or global time
service.

## Player movement

Gameplay continues to own one `Player`. Its state is now:

```text
floating-point X/Y pixel position
24×24 pixel dimensions
movement speed of 180 pixels per second
```

Held D-pad input builds a direction vector with components `-1`, `0`, or `1`.
When both axes are nonzero, both are multiplied by `0.70710678` (`1/√2`) before
applying speed and delta time. Diagonal and cardinal movement therefore have
the same effective speed.

After movement, the top-left position is clamped to:

```text
X: 0–616
Y: 0–424
```

These limits subtract the 24-pixel dimensions from the 640×448 framebuffer,
keeping the entire rectangle visible. Coordinates are truncated to integers
only for display; movement retains floating-point precision.

## State integration

Splash and Main Menu keep their existing lifecycles. Newly pressed START still
requests Main Menu → Gameplay through the state manager. Gameplay initializes
the player and timer, updates both once per frame, draws the background,
rectangle, and diagnostics once per frame, and remains active until reset or
power-off.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libdraw`, `libpacket`, `libgraph`, `libdma`, and
  C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
