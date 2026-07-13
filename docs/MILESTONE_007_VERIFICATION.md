# Milestone 007 Verification

Date: 2026-07-12

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
97af74373fb6dc54073947086c870b65185ef98293275cc1420deb290f01bb8b
```

The resulting ELF is 1,401,004 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100d38`;
- pixel-coordinate, delta-time, controls, and rendering strings present;
- PS2SDK draw and packet libraries linked successfully.

Code-path inspection confirms cardinal direction construction, `1/√2`
diagonal normalization, speed × delta movement, a 100 ms delta cap, and
full-rectangle boundary clamps.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. Runtime verification is
therefore external. Follow `docs/BUILDING.md` and confirm:

1. Main Menu transitions to Gameplay on START.
2. A filled colored rectangle is visible over the cleared background.
3. Cardinal movement is smooth in all four directions.
4. Diagonal travel has the same effective speed as cardinal travel.
5. The rectangle remains fully inside X `0–616` and Y `0–424`.
6. After an emulator pause or stall, delta is at most 100 ms and the rectangle
   does not jump more than 18 pixels from that update.

Until that check is completed, the native build, deterministic ELF, timing and
movement calculations, primitive submission path, bounds, and static PS2
suitability are verified; visible runtime behavior is not claimed locally.

## Limitations

- Rendering is single-buffered and immediate-mode.
- Diagnostics still use the PS2SDK debug font.
- Player state uses single-precision floating point.
- The only primitive exposed is a filled solid-color rectangle.
- There are no textures, sprites, animation, collision, physics, cameras,
  maps, or 3D rendering.
