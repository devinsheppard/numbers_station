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

- Preserve validated behavior and intentional product constraints.
- Replace obsolete foundations when evidence supports a simpler design.
- Document dependency and interface changes.
- Keep the repository buildable before stopping work.
- Do not claim a successful build unless it actually compiled.

## C++ Direction

Use C or C++ deliberately and declare the language standard in the build.
Avoid dynamic allocation in frame loops, make ownership explicit, and account
for PS2 alignment and bounded-memory requirements when those systems are
introduced.

## Revision Log

### 2026-07-11 - Project 001

Created initial programming standards from the foundation prompt.
