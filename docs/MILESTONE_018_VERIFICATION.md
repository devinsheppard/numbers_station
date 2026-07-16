# Milestone 018 Verification

Date: 2026-07-16

## Scope

Milestone 018 adds one fixed world exit and temporary completion overlay after
the existing objective chain. It introduces no mission, ending, scene-change,
checkpoint, score, save, event, scripting, or generalized overlay architecture.

## Extraction zone

The extraction zone is one compile-time `WorldRectangle`:

- Position: `(1360,1020)`
- Dimensions: `80×80`
- Color: RGB `(0x40,0xd8,0xd0)`
- Behavior: visible, viewport-relative, clipped, and non-solid

It is beyond the southeastern document and outside existing solid obstacle
bounds. The same rectangle drives rendering and half-open player overlap.

## Completion behavior

The existing all-documents-read bitmask is the only eligibility condition.
Entering the zone earlier does nothing. Once all three bits are set, crossing
from outside to inside opens a fixed overlay containing:

```text
MISSION COMPLETE

Transmission source located.

End of prototype.

Press CIRCLE to return.
```

The overlay refreshes frame timing and returns before normal Gameplay update,
freezing movement, collision, viewport, interaction, and world changes. Newly
pressed Circle dismisses it without changing player or world state. An inside
flag prevents immediate reopening while the player remains in the zone; leaving
and re-entering may show it again. No completed state persists.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
15cd9df13780163a5a61424a649df911be50caf20cef853f4fd91f25e7df6f91
15cd9df13780163a5a61424a649df911be50caf20cef853f4fd91f25e7df6f91
15cd9df13780163a5a61424a649df911be50caf20cef853f4fd91f25e7df6f91
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, movement, viewport, collision, terminal, barrier,
   documents, objective progression, textures, and double buffering remain
   correct.
2. The cyan 80×80 extraction zone renders at `(1360,1020)`, scrolls correctly,
   and remains non-solid.
3. Entering before all documents are read does nothing.
4. Entering after all documents are read shows the complete fixed overlay.
5. Movement and Cross cannot change or dismiss the overlay.
6. Newly pressed Circle dismisses it and restores the exact player, viewport,
   objective, terminal, barrier, document, and world state without a timing
   jump.
7. The overlay does not immediately reopen while the player remains inside.
8. Leaving and re-entering allows it to open again without persistent completion
   state.
9. The dark overlay covers the playfield and displays every required completion
   line correctly.
10. Existing texture rendering and double-buffered presentation remain correct.
11. No black-screen regression, rendering corruption, lockup, or crash occurred.

The externally validated ELF SHA-256 is:

```text
15cd9df13780163a5a61424a649df911be50caf20cef853f4fd91f25e7df6f91
```

Milestone 018 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There is exactly one fixed extraction zone and one fixed completion message.
- Completion is transient and never leaves Gameplay.
- There is no mission system, ending sequence, transition, checkpoint, score,
  save data, event system, scripting, or generalized overlay architecture.
