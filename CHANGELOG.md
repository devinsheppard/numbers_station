# CHANGELOG

All notable project changes will be appended here. Do not delete previous entries.

## 2026-07-16 - Milestone 019 Ambient Radio Prototype

- Added one fixed non-solid radio marker at `(1510,1120)`.
- Added a fixed 180-pixel center-distance activation radius.
- Added five compile-time number-group transmissions advancing every three
  nearby Gameplay seconds and looping deterministically.
- Reset the sequence predictably when the player leaves range.
- Preserved all objective, extraction, overlay, movement, collision, rendering,
  texture, and presentation behavior.

## 2026-07-16 - Milestone 018 World Exit Prototype

- Added one fixed non-solid extraction zone at `(1360,1020)`.
- Gated entry completion directly on the existing all-documents-read condition.
- Added a fixed full-screen completion overlay dismissed by newly pressed
  Circle.
- Preserved player and world state during the overlay and allowed it to reopen
  after leaving and re-entering the zone.
- Added no mission, ending, transition, checkpoint, event, or save architecture.

## 2026-07-16 - Milestone 017 Fixed Objective Prototype

- Added three compile-time objective strings selected by existing Gameplay
  state.
- Added one Gameplay-local three-bit mask recording which fixed notes have been
  opened during the current Gameplay entry.
- Advanced the objective after terminal activation and after all three notes
  have been read at least once.
- Kept every document rereadable and added no quest, journal, collection, save,
  event, or generalized objective architecture.

## 2026-07-16 - Milestone 016 Multiple Fixed Readable Documents

- Expanded the single field note into three fixed 48×48 world documents.
- Added unique compile-time field-note text at three separated locations.
- Reused one proximity, opening, overlay, dismissal, and timing path for every
  note.
- Kept every note non-solid, repeatable, and free of read or collection state.
- Preserved movement, collision, terminal, barrier, viewport, texture, and
  presentation behavior.

## 2026-07-16 - Milestone 015 Fixed Readable Document

- Added one fixed, pale, non-solid field note beyond the signal barrier.
- Added newly pressed Cross reading and newly pressed Circle dismissal.
- Added a full-screen dark overlay with deterministic compile-time narrative
  text.
- Froze gameplay while reading while continuing frame-timer refreshes to avoid
  a movement jump on dismissal.
- Preserved the player, viewport, terminal, barrier, collision, texture, and
  presentation states across reading.

## 2026-07-16 - Milestone 014 Signal-Controlled Barrier

- Added one fixed world-space barrier controlled directly by terminal
  activation.
- Added bright-red active and dark-gray disabled barrier rendering.
- Included the active barrier in the same nearest-boundary collision decision
  as the four existing obstacles.
- Removed barrier collision immediately after terminal activation without a
  second state flag or delayed transition.
- Preserved terminal, movement, collision, viewport, texture, and presentation
  behavior.

## 2026-07-15 - Milestone 013 Fixed World Interaction

- Added one fixed, visible, non-solid world-space signal terminal.
- Reused the player and terminal half-open AABBs for proximity testing.
- Added newly pressed Cross activation with one Gameplay-local latched flag.
- Added distinct inactive and activated colors plus minimal prompt and success
  messages.
- Preserved movement, collision, viewport, texture, and frame-presentation
  behavior.

## 2026-07-15 - Milestone 012 Static Obstacle Collision Prototype

- Added four fixed world-space solid rectangles shared by rendering and
  collision.
- Added half-open player/obstacle AABB checks with touching edges permitted.
- Added nearest-boundary horizontal-then-vertical resolution for deterministic
  stopping, corner behavior, and wall sliding.
- Preserved player movement, world bounds, viewport behavior, texture rendering,
  and the established frame lifecycle.

## 2026-07-15 - Milestone 011 Left Analog Stick Movement

- Added centered signed left-stick values to the existing primary input state.
- Requested DualShock analog mode only when the connected pad supports it.
- Added a fixed 25% radial deadzone with proportional magnitude rescaling.
- Combined D-pad and analog intent with a final unit-length speed clamp.
- Preserved world movement, viewport scrolling, textured rendering, and
  disconnected-controller safety.

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
