# Milestone 006 Verification

Date: 2026-07-12

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
088d160722c1bf7854abf9b884978fa7f8c755e0a6b3a695c4e564db02f5860e
```

The resulting ELF is 1,325,836 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100d38`;
- Gameplay header, coordinate labels, controls, and marker strings present.

Code-path inspection confirms that newly pressed START requests Gameplay,
Gameplay owns one initialized player, held D-pad bits update both axes, and
position is clamped after every update.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. Runtime verification is
therefore external. Follow `docs/BUILDING.md` and confirm:

1. Splash transitions to Main Menu.
2. START transitions to Gameplay.
3. The `@` marker and coordinates move with all four D-pad directions.
4. X never leaves `1–78` and Y never leaves `10–27`.
5. Gameplay remains active indefinitely.

Until that check is completed, the native build, deterministic ELF, transition
path, movement calculations, clamping logic, and static PS2 suitability are
verified; visible movement is not claimed locally.

## Limitations

- Position is measured in debug-display character cells, not pixels or world
  units.
- Movement speed is one cell per rendered frame and is therefore frame-rate
  dependent.
- The `@` marker is diagnostic text, not a sprite.
- There is no animation, physics, collision, camera, world, or return transition.
