# Milestone 009 Verification

Date: 2026-07-13

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
15db7821696e70797b6b16bc3fa8fa06d493f517f721c7556c65abf6e56128cf
```

The resulting ELF is 1,423,884 bytes.

## Static inspection

The native toolchain reports an ELF32, little-endian MIPS N32 executable with
R5900 and MIPS III flags, static linking, and entry point `0x100d38`. The ELF
contains the Milestone 009 title, texture-rendering status, texture-format
diagnostic, and textured-sprite instructions.

Code-path inspection confirms:

- deterministic procedural generation of 1,024 aligned RGBA pixels;
- a 32×32 `GS_PSM_32` source and 4,096-byte texture allocation;
- data-cache writeback before GIF DMA reads the source pixels;
- DMA-chain texture transfer, texture-cache flush, DMA completion, and a GS
  finish fence before packet reuse;
- texture allocation after both framebuffers;
- explicit GS-range and framebuffer-overlap validation;
- nearest-neighbor sampling, U/V clamping, RGBA decal mode, and an immediate
  textured rectangle;
- texture-state binding after diagnostic text and immediately before the quad;
- the existing begin, completion, vertical-blank, swap, and retarget sequence;
- the existing 24×24 player bounds of X `0–616` and Y `0–424`, delta-time
  movement, and normalized diagonals.

## GS VRAM layout

| Allocation | GS word start | Byte start | Bytes |
| --- | ---: | ---: | ---: |
| Framebuffer 0 | `0x00000` | `0x000000` | 1,146,880 |
| Framebuffer 1 | `0x46000` | `0x118000` | 1,146,880 |
| Test texture | `0x8c000` | `0x230000` | 4,096 |

The final allocation ends at byte offset `0x231000`, safely below the 4 MiB GS
VRAM limit at `0x400000`. The texture begins exactly after framebuffer 1 and
does not overlap either framebuffer.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. Visual, controller, and runtime
claims therefore remain external checks. Follow `docs/BUILDING.md` and confirm:

1. Splash transitions to Main Menu and START enters Gameplay.
2. The sprite shows the documented checkerboard, N emblem, and four correctly
   oriented corner colors without corruption.
3. D-pad movement is smooth in all directions, diagonals are normalized, and
   the complete sprite remains inside the screen bounds.
4. Display and draw indices remain opposite and alternate every frame.
5. Splash, Main Menu, input, timing, state transitions, and double-buffered
   presentation show no regressions.
6. Pause and resume does not produce a movement jump, damaged texture, partial
   frame, or stopped buffer alternation.

Until those checks are performed, the native build, deterministic ELF, texture
generation and upload path, VRAM safety checks, GS state submission, and static
PS2 suitability are verified; successful rendering is not claimed locally.

## Limitations

- There is one video-owned procedural texture and one purpose-specific draw
  function; there is no general texture or asset interface.
- The texture is uploaded only during video initialization and cannot be
  replaced at runtime.
- Drawing remains conservative and synchronous rather than pipelined.
- Runtime appearance and controller behavior require external PCSX2 or hardware
  validation.
- There is no filesystem loading, disc loading, texture cache, sprite batching,
  animation, camera, map, collision, or 3D rendering.
