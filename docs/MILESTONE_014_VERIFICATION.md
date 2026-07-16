# Milestone 014 Verification

Date: 2026-07-16

## Scope

Milestone 014 adds one fixed world-space barrier controlled directly by the
existing signal-terminal activation flag. It introduces no second state flag,
barrier array, identifier, door system, trigger system, event system, or
generalized conditional-collision architecture.

## Fixed barrier data

The barrier has one compile-time definition:

| Property | Value |
| --- | --- |
| World position | `(900,620)` |
| Dimensions | `36×220` pixels |
| Active color | RGB `(0xf0,0x20,0x20)` |
| Disabled color | RGB `(0x38,0x48,0x50)` |

These bounds drive active rendering, disabled rendering, and active collision.
The barrier is separate from the player start, terminal, and all four existing
obstacles. It is reachable and has clear approach space on both horizontal
sides.

## State and rendering

There is no barrier-state variable:

```text
signal_terminal_activated == 0  -> barrier active
signal_terminal_activated != 0  -> barrier disabled
```

The active barrier is bright red and displays `Barrier: ACTIVE`. The disabled
barrier retains the same world bounds, renders dark gray, and displays
`Barrier: DISABLED`. Both states use the existing viewport-relative clipped
rectangle renderer.

Gameplay initialization clears the existing terminal flag, which restores the
active barrier. A valid newly pressed Cross while overlapping the terminal sets
that flag. The same frame renders the activated terminal and disabled barrier;
subsequent updates exclude the barrier from collision. There is no toggle,
delay, additional press, or persistence beyond Gameplay.

## Collision participation

The four existing obstacle definitions and barrier remain separate fixed data,
but all active candidates feed the same resolution result. A local candidate
helper applies the existing half-open tests and updates one resolved boundary.
The barrier uses that helper only while active.

Horizontal resolution chooses the smallest valid boundary when moving right
and the largest when moving left. Vertical resolution uses the horizontally
resolved X and chooses the smallest valid boundary when moving down and the
largest when moving up. Therefore the nearest boundary across the existing
obstacles and active barrier wins regardless of evaluation order. Resolution
still runs exactly once in the existing X-then-Y sequence.

When disabled, the barrier is never passed to either candidate helper. Its
former bounds impose no collision while its dark-gray visual remains visible.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
361adbb0445bec24f0aa807023cb49304ba7b50e165060d3a90fba3333ddbd82
361adbb0445bec24f0aa807023cb49304ba7b50e165060d3a90fba3333ddbd82
361adbb0445bec24f0aa807023cb49304ba7b50e165060d3a90fba3333ddbd82
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA route. It confirmed:

1. Splash, Main Menu, START, player, world, landmarks, viewport, texture, and
   double buffering retain their verified behavior.
2. All four existing obstacles retain collision and wall sliding.
3. The terminal retains its visual, non-solid, prompt, Cross, and latch behavior.
4. Before activation, the bright-red barrier reports `Barrier: ACTIVE`, blocks
   from both sides, stops flush, and supports D-pad and analog wall sliding.
5. Mixed input and repeated pressure cannot penetrate or jitter against it.
6. Terminal activation turns the terminal yellow, preserves its message, turns
   the barrier dark gray, and reports `Barrier: DISABLED` in the same frame.
7. The disabled barrier can be crossed from both sides without invisible
   collision.
8. Returning to the terminal and pressing Cross cannot restore or toggle it.
9. Reinitializing Gameplay restores the inactive terminal and active barrier.
10. Controller disconnection cannot accidentally activate the terminal.
11. No black screen, corruption, persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`361adbb0445bec24f0aa807023cb49304ba7b50e165060d3a90fba3333ddbd82`.
The active barrier rendered and blocked traversal correctly, terminal activation
disabled it immediately, and its former bounds became traversable without
invisible collision. Existing movement, obstacles, viewport, texture, and
double-buffered rendering remained correct. No rendering corruption, black
screen, lockup, or crash occurred. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There is exactly one fixed barrier controlled by one fixed terminal.
- The disabled barrier remains visible as a dark-gray technical marker.
- There are no doors, keys, multiple controls, power systems, puzzles, events,
  transitions, persistence, or generalized conditional collision.
- Real PlayStation 2 hardware remains untested.
