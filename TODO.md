# TODO

## Project 001 Blockers

- Install a WSL2 Ubuntu distribution.
- Install PS2DEV.
- Install PS2SDK.
- Install or verify gsKit.
- Verify `ee-gcc`, `ee-g++`, `ee-ld`, and GNU Make inside WSL.
- Verify controller library availability through PS2SDK.
- Verify PS2SDK sample builds.

## Project 001 After Toolchain Verification

- Add the minimal PS2 application entry point.
- Initialize video.
- Clear the screen.
- Display `NUMBERS STATION` and `PROJECT INITIALIZED`.
- Initialize controller input.
- Exit cleanly when START is pressed.
- Compile to `numbers_station.elf`.
- Report warnings and errors from the actual compiler output.

## Project 002 Recommendations

- Complete Project 001 only after toolchain verification succeeds.
- Keep Project 002 focused on one narrow system, preferably a renderer/input foundation pass after the minimal ELF exists.

