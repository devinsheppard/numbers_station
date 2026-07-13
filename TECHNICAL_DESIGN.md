# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Framebuffer layout

Milestone 008 retains the 640×448, 32-bit PS2SDK display mode and immediate-mode
`libdraw` primitives. The video module resets PS2SDK's VRAM allocator and
allocates two `GS_PSM_32` framebuffers with `GRAPH_ALIGN_PAGE` alignment.

Each framebuffer contains 286,720 pixels and consumes 1,146,880 bytes. Together
they consume 2,293,760 bytes of the GS's 4 MiB VRAM. There is no z-buffer,
texture storage, or other video allocation.

At initialization:

- framebuffer 1 is attached to the GS read circuit for display;
- framebuffer 0 is selected as the drawing target;
- both framebuffers are cleared before the first presentation.

The indices describe roles, not fixed ownership. They alternate after every
presented frame.

## Frame lifecycle

All three application states use the same explicit lifecycle:

```text
video_begin_frame
draw primitives and diagnostic text
video_present_frame
```

`video_begin_frame` clears only the active draw buffer. Rectangle and clear
operations reuse one 64-qword packet and complete before that packet is reused.
PS2SDK debug text draws through the currently selected GS drawing context, so
it lands in the same hidden buffer as the primitives.

`video_present_frame` performs:

1. Submit a `draw_finish` fence after all primitive and text commands.
2. Wait until the GS reports that fence complete.
3. Wait for vertical blank.
4. Point the filtered GS read circuit at the completed draw framebuffer.
5. Make that index the display index.
6. Make the previous display index the next draw index.
7. Submit `draw_framebuffer` and wait for completion so subsequent drawing
   targets the hidden buffer.

The next `video_begin_frame` then clears that new draw target. The display
buffer is never cleared or modified while it is being scanned out.

## Synchronization decisions

`submit_packet` calls `dma_wait_fast` before each send and `draw_wait_finish`
after the packet's finish tag. This deliberately favors correctness and clear
packet ownership over pipelined throughput. Packet memory is never modified
while DMA or the GS may still consume it.

The display-buffer switch occurs only after drawing completion and during
vertical blank. This is the minimum reliable swap sequence for the current
single-context, immediate-mode path.

## Runtime diagnostics

Gameplay displays the display and draw indices captured while building the
frame. Because those roles swap at presentation, both values alternate between
zero and one and remain opposite. The diagnostic text itself becomes visible
as part of the completed draw buffer on that presentation.

Existing delta-time movement, 100 ms stall clamp, diagonal normalization,
24×24 player rectangle, and `[0,616] × [0,424]` boundary clamping are unchanged.

## Scope

The video module exposes only frame begin, filled rectangle, diagnostic text,
frame present, buffer-index queries, and shutdown. It has no render queue,
materials, assets, textures, sprites, cameras, scene graph, or generalized
graphics engine.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libdraw`, `libpacket`, `libgraph`, `libdma`, and
  C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
