# Milestone 005 Verification

Date: 2026-07-12

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
cd3346696b9c32fd48aa71b7abe1ba006624e056a7ebef198e9d5ac51e1a060c
```

The resulting ELF is 1,324,820 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100fe8`;
- Splash, Main Menu, START prompt, and gameplay-placeholder strings present.

Code-path inspection confirms that Splash uses PS2 system time, transitions
through the manager after three seconds, and Main Menu responds only to the
newly pressed START mask while remaining active.

## External PCSX2 verification

PCSX2 is not installed on the ARM64 build host. Runtime verification is
therefore external for this milestone. Follow `docs/BUILDING.md` and confirm:

1. Splash renders immediately.
2. Main Menu replaces Splash after approximately three seconds.
3. START displays `Gameplay not yet implemented.`
4. Main Menu remains active and the application continues running.

Until that check is completed, the native build, deterministic ELF, state
dispatch, timer comparison, input edge check, and static PS2 suitability are
verified; visual runtime behavior is not claimed locally.

## Limitations

- Only Splash and Main Menu exist by design.
- State transitions are explicit switch dispatch rather than a generalized
  registry or state stack.
- Main Menu has no selection model and START cannot launch gameplay.
- No input action exits the application.
