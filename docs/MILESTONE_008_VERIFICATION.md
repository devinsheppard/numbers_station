# Milestone 008 Verification

Date: 2026-07-13

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
cc48ef1d17d29d224a11dbb29386ab3354bf08adb43b11535788d7d997cbf56e
```

The resulting ELF is 1,415,880 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100d38`;
- double-buffer title and display/draw diagnostics present;
- PS2SDK graph, draw, packet, and DMA libraries linked successfully.

Code-path inspection confirms:

- two page-aligned 640×448 `GS_PSM_32` allocations;
- distinct initial display index 1 and draw index 0;
- both buffers cleared before first presentation;
- a finish fence before vertical-blank presentation;
- display selection from the completed draw buffer;
- next draw index calculated as `1 - completed_buffer_index`;
- draw-target retargeting completes before the next frame begins;
- Splash, Main Menu, and Gameplay each call begin and present once per render.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. Runtime verification is
therefore external. Follow `docs/BUILDING.md` and confirm:

1. Splash, Main Menu, START transition, controller input, and timing still work.
2. Display/draw diagnostics remain opposite and alternate every frame.
3. The background, text, and player rectangle always appear as a complete frame.
4. Player movement remains smooth without obvious tearing or partial clears.
5. Pause and resume preserves complete frames, bounded delta time, and ongoing
   buffer alternation.

Until that check is completed, the native build, deterministic ELF,
framebuffer allocation logic, completion fencing, swap invariant, and static
PS2 suitability are verified; runtime presentation quality is not claimed
locally.

## Limitations

- Rendering uses one GS drawing context and conservative synchronous packet
  completion rather than pipelined command generation.
- Both framebuffers use more than half of available GS VRAM, leaving about
  1.8 MiB before alignment and future allocations.
- Diagnostics use the PS2SDK debug font on the active draw target.
- No textures, sprites, animation, cameras, depth buffer, render queue, or 3D
  rendering exists.
