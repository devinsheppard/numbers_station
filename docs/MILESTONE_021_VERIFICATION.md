# Milestone 021 Verification

Date: 2026-07-17

## Scope

Milestone 021 adds one fixed, repeatable inspection interaction at the existing
radio source. It introduces no inspected state, persistence, audio, interaction
framework, overlay framework, UI system, event system, scripting, or new module.

## Interaction boundary and input

The existing radio rectangle at `(1510,1120)` with dimensions `32×32` remains
the rendered marker and becomes the inspection boundary. The player and radio
use the established half-open overlap rule, so touching an edge is not overlap.
The marker remains non-solid.

While overlapping during normal Gameplay, the prompt is:

```text
Press CROSS to inspect
```

Opening checks only `pressed_buttons & PAD_CROSS`. Entering without pressing
Cross and holding Cross before entry cannot open the overlay. One transient
`radio_inspection_open` flag exists only while the fixed inspection is visible.

## Inspection overlay

The complete compile-time text is:

```text
TRANSMISSION SOURCE

The receiver is disconnected.

The numbers continue.

Press CIRCLE to close.
```

The overlay begins and presents through the established double-buffered frame
path, covers the display with a dark primitive, and draws only the fixed text.
While open, update still refreshes the frame timer, then returns before movement,
collision, interactions, viewport changes, extraction, radio timing, or radio
direction comparison. Only newly pressed Circle dismisses it.

No player, viewport, objective, terminal, barrier, document, extraction,
transmission, direction, or world value is changed. The inspection may be
opened repeatedly and records no completion state.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
3d5656117b628a878b37e73f2d6bb2410dbb7e683760e6b14444f7f3a43e4894
3d5656117b628a878b37e73f2d6bb2410dbb7e683760e6b14444f7f3a43e4894
3d5656117b628a878b37e73f2d6bb2410dbb7e683760e6b14444f7f3a43e4894
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, movement, collision, scrolling, landmarks,
   terminal, barrier, documents, objectives, extraction, radio transmissions,
   direction finder, rendering, textures, timing, and double buffering remain
   correct.
2. The fixed radio remains visible, viewport-relative, and non-solid.
3. No inspection prompt appears outside its half-open bounds.
4. Entering without pressing Cross does not open the inspection.
5. Holding Cross before entry does not open it.
6. Newly pressed Cross while overlapping opens the full-screen dark overlay.
7. Every required inspection line is visible.
8. Movement, world interactions, Cross, and unrelated buttons cannot alter or
   dismiss the overlay.
9. Radio timing and direction comparison remain paused.
10. Newly pressed Circle dismisses the overlay.
11. Player and viewport resume at their exact previous positions without a
    movement jump.
12. Transmission timing, index, and direction state resume safely.
13. The inspection can be opened repeatedly and changes no objective or world
    state.
14. Existing objectives, extraction, documents, terminal, barrier, direction
    finder, and every previously verified Gameplay behavior remain unchanged.
15. No crash, graphical corruption, black screen, lockup, or regression
    occurred.

The externally validated ELF SHA-256 is:

```text
3d5656117b628a878b37e73f2d6bb2410dbb7e683760e6b14444f7f3a43e4894
```

Milestone 021 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There is exactly one fixed inspection at the existing radio marker.
- Inspection text is fixed, non-scrolling, and repeatable.
- There is no audio, inspected state, persistence, interaction framework,
  overlay framework, UI system, event system, or scripting.
