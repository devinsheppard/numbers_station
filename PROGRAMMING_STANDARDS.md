# PROGRAMMING STANDARDS

## General Rules

- Use descriptive names.
- Keep functions reasonably small.
- Avoid unnecessary global variables.
- Separate rendering, input, gameplay, and world logic.
- Comment major systems.
- Document assumptions and limitations.
- Design systems for future expansion.
- Prefer deterministic state machines.
- Avoid hard-coded values whenever practical.
- Keep code readable.

## Repository Rules

- Preserve existing design decisions unless a future prompt explicitly changes them.
- Prefer extension over replacement.
- Document breaking changes before implementation.
- Keep the repository buildable before stopping work.
- Do not claim a successful build unless it actually compiled.

## C++ Direction

Use modern C++ only where the PS2SDK toolchain supports it reliably. Avoid language or library features that increase risk on PS2 hardware.

## Revision Log

### 2026-07-11 - Project 001

Created initial programming standards from the foundation prompt.

