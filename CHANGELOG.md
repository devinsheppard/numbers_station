# CHANGELOG

All notable project changes will be appended here. Do not delete previous entries.

## 2026-07-12 - Milestone 004 Controller Input and Runtime Diagnostics

- Added primary-controller initialization and non-blocking per-frame polling.
- Added current, newly pressed, and newly released button state tracking.
- Added disconnected-controller handling and last-pressed button names.
- Added controller status and frame counter runtime diagnostics.
- Documented input lifecycle and PCSX2 controller verification.

## 2026-07-12 - Milestone 003 Application Framework

- Split the demonstration entry point into application and video modules.
- Added explicit startup, update/render loop, and shutdown sequencing.
- Synchronized rendering to PS2 vertical blank through PS2SDK `libgraph`.
- Updated architecture, lifecycle, build, and verification documentation.

## 2026-07-12 - Milestone 002 Native ARM64 Build Foundation

- Replaced the blocked Windows/WSL build plan with a native AArch64 build.
- Added a minimal PS2SDK executable and reproducible GNU Make pipeline.
- Removed the staged Tyra submodule and all Docker-oriented dependency setup.
- Archived the obsolete Project 001 toolchain verification record.
- Updated documentation for the Raspberry Pi 4 Ubuntu 26.04 workflow.

## 2026-07-11 - Project 001 Foundation

- Created repository directory structure.
- Created core project documentation.
- Created GNU Make interface with toolchain verification gates.
- Recorded failed local PS2 toolchain verification.
- Did not create PS2 runtime application code because required tools are missing.
