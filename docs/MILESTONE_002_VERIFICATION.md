# Milestone 002 Verification

Date: 2026-07-12

## Environment

- Host architecture: AArch64
- Host OS: Ubuntu 26.04 on Raspberry Pi 4
- Native EE compiler: `mips64r5900el-ps2-elf-gcc` 15.2.0
- Compiler executable format: ELF 64-bit AArch64
- Build interface: GNU Make

## Build result

`make clean && make` completed successfully without compiler or linker
warnings and produced:

```text
out/bin/numbers_station.elf
```

The ELF size was 1,203,828 bytes. Static inspection reported:

- ELF32, little-endian;
- MIPS N32 executable;
- R5900 and MIPS III flags;
- statically linked;
- entry point `0x100f10`;
- one loadable segment aligned to 128 bytes;
- both required display strings present.

## Reproducibility

Two consecutive builds, each preceded by `make clean`, produced the same
SHA-256 digest:

```text
58b6f76fbf75ef38ef68a5ba59deb62da8085823d77240a89a1de9ef36944a9c
```

This verifies reproducibility for the recorded source, native toolchain,
PS2SDK installation, and environment.

## Runtime scope

PCSX2 was not installed on the build host, and no real-hardware launch was
performed during this milestone. The executable format, R5900 flags, link
layout, and static linkage are suitable for PS2 loading, but runtime behavior
is not claimed as tested until the ELF is launched in PCSX2 or on hardware.
