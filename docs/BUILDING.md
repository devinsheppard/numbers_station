# Native ARM64 Build

## Supported workflow

Numbers Station builds directly on AArch64 Linux. Docker, Windows, WSL, and
Tyra are deliberately excluded.

The build has two explicit dependencies:

1. `NATIVE_PS2DEV` supplies the native AArch64 EE cross-compiler produced by
   `ps2_arm64_toolchain` Version 1.0.
2. `PS2SDK` supplies PS2 target headers, startup files, and libraries.

The installed PS2 startup object is resolved beside `PS2SDK` and passed to the
linker explicitly because the native compiler and SDK intentionally live under
different installation roots.

Keeping these inputs separate prevents an older PS2DEV directory from
silently selecting an x86-64 compiler on the ARM64 host.

## Verification

Run:

```sh
make verify
```

The target checks:

- the host reports `aarch64`;
- the EE compiler exists and executes;
- `file` identifies the compiler as an AArch64 executable;
- the PS2SDK linkfile, debug header, debug library, graph library, and pad
  library exist;
- the installed PS2 startup object exists.

The compiler is invoked by its canonical name:

```text
mips64r5900el-ps2-elf-gcc
```

## Reproducible build procedure

```sh
make clean
make
sha256sum out/bin/numbers_station.elf
make clean
make
sha256sum out/bin/numbers_station.elf
```

Matching hashes demonstrate that repeated clean builds from the same source,
toolchain, PS2SDK, and environment produce the same ELF.

Generated files stay below `out/` and are never used as source inputs.

## Runtime verification

The Milestone 004 ELF should display:

```text
Numbers Station
Milestone 004
Controller Input Initialized
Controller: Connected
Frame: <current frame>
Last pressed: <button name>
```

`Controller: Not connected` is expected when port 1 has no available pad. The
application continues its vertical-blank-synchronized loop in either state.

## PCSX2 controller verification

1. Configure a controller or keyboard mapping for PCSX2 controller port 1.
2. Launch `out/bin/numbers_station.elf` through PCSX2's ELF loader.
3. Confirm the screen reports `Controller: Connected` and the frame counter
   advances.
4. Press D-pad Up, Down, Left, Right, Cross, Circle, Square, Triangle, Start,
   Select, L1, and R1 separately. Confirm `Last pressed` changes to each name.
5. Hold and release a button and confirm the application continues normally.
6. Disable or disconnect the mapped controller. Confirm the status becomes
   `Not connected`, the frame counter continues, and the application does not
   hang or crash.

Static ELF inspection confirms the PS2 MIPS executable format. Runtime claims
must be recorded separately for PCSX2 and real hardware; a successful build is
not itself a hardware execution test.
