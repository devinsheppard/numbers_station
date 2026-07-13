# Milestone 004 Verification

Date: 2026-07-12

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

The clean builds had the same SHA-256 digest:

```text
440e9106f9ae48df33b6e971b88cbab2fcc2e7c9fdda370d2d118f6c9fe5a97d
```

The resulting ELF is 1,321,892 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100fe8`;
- required screen and button-name strings present;
- PS2SDK pad, debug, graph, DMA, and C libraries linked successfully.

## PCSX2 scope

The Milestone 003 ELF was successfully runtime validated in PCSX2 before this
milestone. PCSX2 is not installed or otherwise available on the ARM64 build
host, so controller behavior could not be exercised locally. Follow the
procedure in `docs/BUILDING.md` to verify connection changes and each required
button in PCSX2. Until that external check is completed, compilation and static
ELF suitability are verified but runtime controller detection is not claimed.

## Expected disconnected behavior

An absent or temporarily unavailable controller does not enter a wait loop.
The frame counter should continue advancing, connection status should report
`Not connected`, current and pressed masks should be zero, and previously held
buttons should appear once in the released mask.
