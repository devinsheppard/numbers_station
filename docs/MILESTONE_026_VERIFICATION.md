# Milestone 026 Verification

Date: 2026-07-21

## Scope

Milestone 026 adds one fixed controls screen to the existing Main Menu. It adds
no application state, generalized menu, selection cursor, reusable modal,
settings system, persistence, texture, or renderer change.

## State and input

One private `controls_open` boolean is owned by `main_menu_state.c` and cleared
during both Main Menu initialization and shutdown. Entry from Splash, Gameplay
completion, or pause exit therefore always shows the ordinary Main Menu.

When controls are closed, newly pressed Triangle opens them. Triangle is checked
before the existing newly pressed START path, so it wins when both edges occur
in one update. When controls are open, only newly pressed Circle is processed;
it closes the screen. START, Triangle, Cross, and unrelated buttons are ignored.
Held buttons do not repeat actions because only `pressed_buttons` is used.

## Presentation

The ordinary Main Menu displays:

```text
Numbers Station

Main Menu

Press START to begin
Press TRIANGLE for controls
```

The controls screen displays:

```text
CONTROLS

D-PAD / LEFT STICK   Move
CROSS                Interact
CIRCLE               Close overlays
START                Pause / Resume

Press CIRCLE to return
```

Both screens use the unchanged active-framebuffer text renderer and normal Main
Menu begin/draw/present lifecycle.

## Native build verification

Perform three independent clean native ARM64 builds. Each build must be
warning-free and error-free and produce a valid PS2/MIPS ELF. Preserve each ELF,
calculate its SHA-256, and compare all three files byte-for-byte.

The three clean builds produced:

```text
b729631cf058ff35e6a88bf23f767cf1baadd5fe47ec8b0fb449b7bf97cfc2ef
b729631cf058ff35e6a88bf23f767cf1baadd5fe47ec8b0fb449b7bf97cfc2ef
b729631cf058ff35e6a88bf23f767cf1baadd5fe47ec8b0fb449b7bf97cfc2ef
```

All builds were warning-free and error-free, produced valid PS2/MIPS ELF files,
and compared byte-for-byte identical.

Build verification complete.
External Batocera/PCSX2 runtime validation passed.

## External runtime result

Runtime validation used the established Batocera EmulationStation/BUA PCSX2
launch path. Splash reached Main Menu, the ordinary instructions remained stable,
START began fresh Gameplay, and Triangle opened the complete controls screen.
Controls input remained exclusive, held Triangle did not toggle it, Circle
returned only to the ordinary Main Menu, and simultaneous newly pressed START
and Triangle correctly prioritized controls.

Controls began closed after Splash, completion return, and pause return. Repeated
Main Menu, controls, Gameplay, and pause cycles retained no stale state. Gameplay,
renderer behavior, stable text, and double buffering remained unchanged.

## External validation checklist

1. Confirm Splash enters the ordinary Main Menu with stable START and Triangle
   instructions. Circle and unrelated buttons do nothing there.
2. Newly press START and confirm fresh Gameplay initialization remains correct.
3. Return to Main Menu, newly press Triangle, and confirm the complete fixed
   controls text fits, remains readable, and does not flicker.
4. Hold Triangle and confirm it neither toggles nor repeatedly opens controls.
5. While controls are open, confirm START cannot initialize Gameplay and
   Triangle, Cross, and unrelated buttons do nothing.
6. Newly press Circle and confirm only the ordinary Main Menu returns. Confirm a
   Circle held before controls open cannot close them without a new edge.
7. Newly press START and Triangle in the same Main Menu update and confirm
   Triangle takes priority without input leakage into the controls screen.
8. Confirm controls start closed after Splash, completion return, pause exit, and
   repeated Gameplay entries and exits.
9. Recheck Gameplay movement, interactions, documents, receiver, pause/resume,
   pause exit, completion exit, objectives, radio behavior, and fresh reset.
10. Repeat Main Menu -> Controls -> Main Menu -> Gameplay -> Pause -> Main Menu
    -> Controls -> Main Menu. Confirm no stale state, retained Gameplay state,
    crash, lockup, black screen, rendering corruption, text flicker, or
    double-buffering regression.

## Status

Implementation, deterministic native builds, and external runtime validation are
complete. Milestone 026 is approved for commit.
