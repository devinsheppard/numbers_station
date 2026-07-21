# Milestone 027 Verification

Date: 2026-07-21

## Scope

Milestone 027 adds one optional fixed mission briefing to Main Menu. It adds no
Gameplay objective, story canon, persistence, application state, generalized
menu, reusable modal, selection cursor, scrolling text, or renderer change.

## State and mutual exclusion

One new private `briefing_open` boolean is owned by `main_menu_state.c` beside
the existing private `controls_open`. Initialization and shutdown clear both.
Opening controls explicitly clears briefing; opening briefing explicitly clears
controls. The screens therefore cannot be open simultaneously and both begin
closed after Splash, completion return, and pause return.

## Input priority

An open controls or briefing screen processes only newly pressed Circle, closes
itself, and returns. START, Triangle, Square, Cross, and unrelated buttons are
ignored. Neither open input is a toggle, and held buttons cannot repeat because
only `pressed_buttons` is used.

With both screens closed, the deterministic newly pressed priority is:

1. Triangle opens controls.
2. Square opens the briefing.
3. START requests Gameplay.

Triangle plus Square and Triangle plus START therefore open controls. Square
plus START opens briefing. Triangle plus Square plus START opens controls.

## Presentation

The ordinary Main Menu displays:

```text
Numbers Station

Main Menu

Press START to begin
Press TRIANGLE for controls
Press SQUARE for briefing
```

The briefing displays:

```text
MISSION BRIEFING

Restore access to the relay.

Review the field records beyond
the barrier.

Locate the transmission source.

Press CIRCLE to return
```

The briefing uses the unchanged stable active-framebuffer text path and normal
Main Menu frame lifecycle. Its wording only summarizes existing progression.

## Native build verification

Perform three independent clean native ARM64 builds. Each must be warning-free,
linker-warning-free, and error-free and produce a valid PS2/MIPS ELF. Preserve
each ELF, calculate its SHA-256, and compare all three files byte-for-byte.

The three clean builds produced:

```text
450a0b5909567b71bdde6526741a9df7a126917d7a0b9b78d11366905b27cae1
450a0b5909567b71bdde6526741a9df7a126917d7a0b9b78d11366905b27cae1
450a0b5909567b71bdde6526741a9df7a126917d7a0b9b78d11366905b27cae1
```

All builds were warning-free, linker-warning-free, and error-free, produced
valid PS2/MIPS ELF files, and compared byte-for-byte identical.

Build verification complete.
External Batocera/PCSX2 runtime validation passed.

## External runtime result

Runtime validation used the established Batocera EmulationStation/BUA PCSX2
launch path. Splash reached Main Menu, all ordinary instructions remained
stable, START began fresh Gameplay, Triangle opened controls, and Square opened
the fixed mission briefing. Held Square did not toggle briefing, only newly
pressed Circle acted within either modal, and controls and briefing never
coexisted. The documented ordinary-menu input priorities behaved correctly.

Both modal flags reset after Splash, pause return, and completion return.
Gameplay progression, objectives, renderer behavior, stable text, and double
buffering remained unchanged.

## External validation checklist

1. Confirm Splash reaches an ordinary Main Menu showing stable START, Triangle,
   and Square instructions. Circle, Cross, and unrelated buttons do nothing.
2. Confirm START begins fresh Gameplay and Triangle still opens unchanged
   controls.
3. Newly press Square and confirm only the complete fixed briefing appears,
   safely fitted and flicker-free.
4. Hold Square and confirm it neither toggles nor repeatedly opens briefing.
5. While briefing is open, confirm only newly pressed Circle acts. Confirm no
   Gameplay initialization or progress occurs.
6. While controls are open, confirm Square does nothing and briefing does not
   appear. Confirm controls and briefing never coexist.
7. Verify every documented Triangle, Square, and START same-update priority.
8. Confirm both modals start closed after Splash, completion return, pause return,
   and repeated Gameplay entries and exits.
9. Recheck movement, collision, terminal, barrier, documents, receiver,
   transmission source, objectives, radio, pause, completion, and fresh reset.
10. Repeat Main Menu -> Controls -> Main Menu -> Briefing -> Main Menu ->
    Gameplay -> Pause -> Main Menu -> Briefing -> Main Menu. Confirm no stale
    state, input leakage, retained Gameplay state, crash, lockup, black screen,
    corruption, text flicker, or double-buffering regression.

## Status

Implementation, deterministic native builds, and external runtime validation are
complete. Milestone 027 is approved for commit.
