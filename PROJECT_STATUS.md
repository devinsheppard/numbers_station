# Project Status

## Current milestone

Milestone 026: Main Menu Controls Screen is complete and has passed external
runtime validation through the established Batocera EmulationStation/BUA PCSX2
launch path. Milestones 002 through 026 are complete. Milestone 027 has not
started and remains pending approval.

## Validated capabilities

- Splash, Main Menu, Gameplay entry, completion return to Main Menu, and fresh
  Gameplay-session reset. Main Menu includes a fixed, local controls screen
  opened with newly pressed Triangle and closed with newly pressed Circle.
- D-pad and analog movement, collision, wall sliding, viewport scrolling,
  textured player rendering, terminal and barrier progression, three readable
  documents, fixed objectives, stationary receiver broadcasts and inspection,
  radio direction finding, and transmission-source completion.
- Gameplay-local pause toggled by newly pressed START during ordinary Gameplay.
  Simulation, interactions, viewport, objectives, and radio processing freeze
  while the frame-time baseline continues refreshing; resume preserves state
  without a timing or movement jump. Newly pressed Circle while paused returns
  through the existing Main Menu request and lifecycle. Existing overlays retain
  input precedence.
- Stable double-buffered world, texture, overlay, and text presentation.

## Renderer constraints

The renderer retains double buffering and owns active draw-framebuffer
selection. Game text does not use `scr_vprintf` or `scr_putchar`, whose glyph
transfers target framebuffer zero. The existing linked 8x8 MSX bitmap is emitted
as filled horizontal runs through the active libdraw packet so text follows the
same framebuffer as world primitives. Do not restore the fixed-framebuffer
libdebug glyph path or introduce BIOS fonts, font textures, VRAM font atlases,
runtime font loading, or a generalized font system.

## Repository workflow

Development uses the verified native ARM64 Ubuntu PS2 toolchain. Builds must
remain warning-free and deterministic, and each milestone requires external
Batocera/PCSX2 runtime validation before commit.
