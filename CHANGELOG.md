# CHANGELOG

All notable project changes will be appended here. Do not delete previous entries.

## 2026-07-15 - Milestone 010 Minimal Scrolling Gameplay World

- Expanded Gameplay to a fixed 1600×1200 world with a clamped 640×448 viewport.
- Converted player position and full-sprite boundary checks to world space.
- Added deterministic clipped colored regions and cardinal/corner landmarks.
- Preserved delta-time movement, diagonal normalization, and the verified
  Milestone 009 textured-player rendering path.
- Added world-position and viewport diagnostics and runtime verification steps.

## 2026-07-13 - Milestone 009 Texture Upload and Sprite Rendering

- Corrected the GS texture storage stride to the required 64-pixel minimum;
  using the 32-pixel logical width had encoded invalid zero-width transfer and
  texture-buffer fields and caused a black screen in PCSX2.
- Added a deterministic procedural 32×32 `GS_PSM_32` RGBA diagnostic texture.
- Allocated and validated texture VRAM after both page-aligned framebuffers.
- Added cache writeback, GIF DMA-chain upload, texture flush, and GS completion.
- Added nearest-filtered, clamped textured-quad drawing with explicit state.
- Corrected the upload to use a padded 64×32 CPU surface and fixed UV endpoints
  spanning the logical 32×32 image.
- Replaced the player rectangle with the textured test sprite.
- Added texture dimensions and pixel-format runtime diagnostics.
- Documented texture generation, VRAM layout, rendering, and external checks.

## 2026-07-13 - Milestone 008 Double-Buffered Frame Presentation

- Allocated two page-aligned 640×448, 32-bit GS framebuffers.
- Added explicit begin, draw, finish, vertical-blank, swap, and retarget flow.
- Ensured packet completion before packet storage or framebuffer role reuse.
- Added alternating display/draw index diagnostics.
- Applied the presentation lifecycle to Splash, Main Menu, and Gameplay.
- Documented VRAM layout, synchronization, swapping, and PCSX2 verification.

## 2026-07-12 - Milestone 007 Pixel Rendering and Delta-Time Movement

- Added PS2SDK immediate-mode framebuffer clearing and filled rectangles.
- Converted the player from text cells to floating-point pixel coordinates.
- Added PS2 system-clock delta timing with a 100 ms stall cap.
- Added frame-rate-independent movement and normalized diagonals.
- Clamped the complete 24×24 player rectangle inside 640×448 pixels.
- Documented rendering, timing, boundaries, and PCSX2 verification.

## 2026-07-12 - Milestone 006 Gameplay State and Player Movement

- Added Main Menu to Gameplay transition on newly pressed START.
- Added a Gameplay state that owns and updates one player.
- Added held-D-pad movement, screen-cell position diagnostics, and clamping.
- Added a visible `@` player marker without sprites or world systems.
- Documented movement flow, bounds, architecture, and PCSX2 verification.

## 2026-07-12 - Milestone 005 Application State Manager

- Added a minimal single-active-state manager.
- Added timed Splash and controller-driven Main Menu states.
- Added clean state initialization, update, rendering, and shutdown dispatch.
- Added the START-triggered gameplay placeholder without starting gameplay.
- Documented state lifecycle, transitions, and PCSX2 verification.

## 2026-07-12 - Milestone 004 Controller Input and Runtime Diagnostics

- Added primary-controller initialization and non-blocking per-frame polling.
- Added current, newly pressed, and newly released button state tracking.
- Added disconnected-controller handling and last-pressed button names.
- Added controller status and frame counter runtime diagnostics.
- Documented input lifecycle and PCSX2 controller verification.

## 2026-07-12 - Milestone 003 Application Framework

- Split the demonstration entry point into application and video modules.
- Added explicit startup, update/render loop, and shutdown sequencing.
- Synchronized rendering to PS2 vertical blank through PS2SDK `libgraph`.
- Updated architecture, lifecycle, build, and verification documentation.

## 2026-07-12 - Milestone 002 Native ARM64 Build Foundation

- Replaced the blocked Windows/WSL build plan with a native AArch64 build.
- Added a minimal PS2SDK executable and reproducible GNU Make pipeline.
- Removed the staged Tyra submodule and all Docker-oriented dependency setup.
- Archived the obsolete Project 001 toolchain verification record.
- Updated documentation for the Raspberry Pi 4 Ubuntu 26.04 workflow.

## 2026-07-11 - Project 001 Foundation

- Created repository directory structure.
- Created core project documentation.
- Created GNU Make interface with toolchain verification gates.
- Recorded failed local PS2 toolchain verification.
- Did not create PS2 runtime application code because required tools are missing.
