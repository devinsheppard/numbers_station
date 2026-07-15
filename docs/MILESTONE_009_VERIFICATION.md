# Milestone 009 Verification

Date: 2026-07-14

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both final clean builds had the same SHA-256 digest:

```text
c9a5501c26fe6fd3b20192d66052be03970cc39c1e98d547bdeeda03329e2dba
```

The resulting ELF is 1,423,756 bytes.

## Static inspection

The native toolchain reports an ELF32, little-endian MIPS N32 executable with
R5900 and MIPS III flags, static linking, and entry point `0x100d38`. The ELF
contains the Milestone 009 title, texture-rendering status, texture-format
diagnostic, and textured-sprite instructions.

Code-path inspection confirms:

- deterministic procedural generation in an aligned 64×32 RGBA upload surface,
  with the logical 32×32 image in its left half and cleared padding;
- a 64×32 `GS_PSM_32` transfer, 64-pixel GS storage stride, and 8,192-byte
  texture allocation and source range;
- data-cache writeback before GIF DMA reads the source pixels;
- DMA-chain texture transfer, texture-cache flush, and GIF DMA completion
  before packet reuse;
- texture allocation after both framebuffers;
- explicit GS-range and framebuffer-overlap validation;
- nearest-neighbor sampling, U/V clamping, RGBA decal mode, and an immediate
  textured rectangle;
- texture-state binding after diagnostic text and immediately before the quad;
- fixed texel-space UV endpoints `(0,0)–(32,32)` converted by libdraw to the
  GS 12.4 UV format;
- the existing begin, completion, vertical-blank, swap, and retarget sequence;
- the existing 24×24 player bounds of X `0–616` and Y `0–424`, delta-time
  movement, and normalized diagonals.

## GS VRAM layout

| Allocation | GS word start | Byte start | Bytes |
| --- | ---: | ---: | ---: |
| Framebuffer 0 | `0x00000` | `0x000000` | 1,146,880 |
| Framebuffer 1 | `0x46000` | `0x118000` | 1,146,880 |
| Test texture | `0x8c000` | `0x230000` | 8,192 |

The final allocation ends at byte offset `0x232000`, safely below the 4 MiB GS
VRAM limit at `0x400000`. The texture begins exactly after framebuffer 1 and
does not overlap either framebuffer.

## PCSX2 failures and corrective fixes

The original Milestone 009 ELF was tested externally in PCSX2 and produced a
completely black screen, including Splash and Main Menu. Milestones 007 and 008
continued to work, isolating the failure to texture initialization.

The first fault was the use of logical width 32 for `texbuffer_t.width` and the
transfer destination stride. PS2SDK converts both to GS `TBW` and `DBW` fields
by dividing by 64, so 32 encoded an invalid zero width. Correcting those fields
alone did not resolve runtime initialization. The completed fix uses a padded
64×32 CPU surface and transfers the complete 64×32 storage region with TBW and
DBW equal to one. Its aligned cache-writeback range is exactly 8,192 bytes.
Official PS2SDK examples confirmed submission on `DMA_CHANNEL_GIF` followed by
`dma_wait_fast()` is the correct synchronization sequence.

Staged runtime isolation then verified allocation, generation, cache writeback,
upload, synchronization, binding, textured drawing, and subsequent primitive
rendering independently. A later solid-red sprite was traced to coordinates of
`(0,0)–(1,1)`. Inspection of `draw_rect_textured()` confirmed it sets `FST=1`,
emits fixed GS UV coordinates, and multiplies input coordinates by 16 for GS
12.4 format. Restoring pixel-style endpoints `(0,0)–(32,32)` made the complete
procedural texture visible.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. External validation was
completed through the Batocera EmulationStation/BUA launch route, which provides
working controller mapping. The final runtime confirmed:

1. Splash transitions to Main Menu and START enters Gameplay.
2. The sprite shows the documented checkerboard, N emblem, and four correctly
   oriented corner colors without corruption.
3. D-pad movement is smooth in all directions, diagonals are normalized, and
   the complete sprite remains inside the screen bounds.
4. Display and draw indices remained opposite and the established
   double-buffered presentation continued without a persistent black screen.
5. Splash, Main Menu, input, timing, state transitions, player movement, and
   rendering after the textured draw showed no regressions.

The checkerboard, pale border, four orientation corners, and intentionally
narrow centered N emblem all rendered correctly. There was no freeze or
persistent black screen, and primitive rendering continued after the textured
draw. The known libdebug text flicker is a pre-existing double-buffer artifact:
libdebug targets framebuffer address zero rather than tracking the active draw
buffer.

## Limitations

- There is one video-owned procedural texture and one purpose-specific draw
  function; there is no general texture or asset interface.
- The texture is uploaded only during video initialization and cannot be
  replaced at runtime.
- Drawing remains conservative and synchronous rather than pipelined.
- Pause/resume behavior was not repeated during the final corrective runtime
  pass.
- Real PlayStation 2 hardware remains untested.
- There is no filesystem loading, disc loading, texture cache, sprite batching,
  animation, camera, map, collision, or 3D rendering.
