# TOOLCHAIN VERIFICATION

Date: 2026-07-11

## Result

Verification failed. Required PS2 homebrew tooling is missing or unavailable.

## Commands Checked

Windows checks:

- `git status --short`
- `Get-ChildItem Env:PS2DEV,Env:PS2SDK,Env:GSKIT,Env:EE_BIN,Env:IOP_BIN`
- `Get-Command ee-gcc,ee-g++,ee-ld,make,git,wsl.exe`
- `wsl.exe --status`

WSL checks:

- `wsl.exe sh -lc 'printf "PS2DEV=%s\nPS2SDK=%s\nGSKIT=%s\n" "$PS2DEV" "$PS2SDK" "$GSKIT"; command -v ee-gcc; command -v ee-g++; command -v ee-ld; command -v make; uname -a'`
- `wsl.exe sh -lc 'test -d "$PS2DEV" && find "$PS2DEV" -maxdepth 3 -type d -name ps2sdk -o -name gsKit 2>/dev/null | head -20'`

## Findings

- The working folder was not a Git repository before Project 001.
- Windows has `git.exe` and `wsl.exe`.
- Windows PATH does not expose `ee-gcc`, `ee-g++`, `ee-ld`, or `make`.
- PS2DEV, PS2SDK, GSKIT, EE_BIN, and IOP_BIN environment variables are not set in Windows.
- WSL reports that no Linux distributions are installed.
- Because there is no WSL distribution, PS2DEV, PS2SDK, gsKit, controller libraries, and PS2SDK samples could not be verified inside WSL.

## Required Before Runtime Code

- Install a WSL2 Ubuntu distribution.
- Install the legal open-source PS2DEV toolchain.
- Install PS2SDK.
- Install or verify gsKit.
- Confirm `ee-gcc`, `ee-g++`, `ee-ld`, and `make` are available inside WSL.
- Confirm PS2SDK controller libraries are available.
- Build at least one PS2SDK sample.

## Decision

No PS2 runtime application code will be added until this verification passes.

