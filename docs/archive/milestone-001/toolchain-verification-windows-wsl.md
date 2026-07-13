# Archived Toolchain Verification

This is the original Project 001 verification record. It is retained only as
historical context and does not describe the supported Numbers Station build
workflow.

Date: 2026-07-11

## Result at the time

Verification failed because the original Windows workstation had no WSL
distribution or PS2 development tools installed. Checks covered Windows and
WSL environment variables, `ee-gcc`, `ee-g++`, `ee-ld`, GNU Make, PS2SDK,
gsKit, and controller-library availability.

## Superseding decision

Milestone 002 replaced the Windows/WSL plan with a native AArch64 workflow on
Raspberry Pi 4 running Ubuntu 26.04. Current instructions are in the repository
README and `docs/BUILDING.md`.
