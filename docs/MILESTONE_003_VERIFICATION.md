# Milestone 003 Verification

Date: 2026-07-12

## Native build

Two consecutive `make clean && make` runs completed successfully with no
compiler or linker warnings. Both produced:

```text
out/bin/numbers_station.elf
```

Both clean builds had the same SHA-256 digest:

```text
fe56d7818359c7d8208d1c36acd728ad8699bbda4101145ceee11397e494920c
```

The resulting ELF is 1,285,092 bytes.

## Static inspection

Inspection with the native toolchain reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100f10`;
- one loadable segment aligned to 128 bytes;
- all three required display strings present.

## PCSX2 scope

The Milestone 002 ELF was successfully executed in PCSX2 before this milestone,
confirming the established build and launch pipeline. PCSX2 is not installed
or otherwise available on the ARM64 build host, so the Milestone 003 ELF could
not be launched locally during this implementation. Its executable format and
link layout match the previously validated PS2 pipeline; visual confirmation
of the new loop remains an external runtime check.

After Milestone 003 was completed, the generated ELF was successfully runtime
validated in PCSX2. That result was supplied as the verified starting state for
Milestone 004.
