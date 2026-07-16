# Milestone 015 Verification

Date: 2026-07-16

## Scope

Milestone 015 adds one fixed readable field note. It introduces no inventory,
pickup, collection, read tracking, save data, dialogue system, UI framework,
document framework, scripting, objective, quest, or event system.

## Fixed document data

The document has one compile-time world rectangle:

| Property | Value |
| --- | --- |
| World position | `(980,680)` |
| Dimensions | `48×48` pixels |
| Color | RGB `(0xe0,0xd4,0xa8)` |

The same half-open rectangle supplies visual and interaction bounds. It is
beyond the signal barrier, remains non-solid, scrolls through the existing
viewport clipping path, and remains visible after every reading.

The compile-time ASCII narrative is:

```text
FIELD NOTE

The relay was powered down again
at 02:13.

Transmission resumed eleven
minutes later without input.

No external source found.

Recommend abandoning the site.

E. Mercer

Press CIRCLE to close
```

## Opening and dismissal

Normal Gameplay displays `Press CROSS to read.` only while the player overlaps
the note. Opening requires `pressed_buttons & PAD_CROSS`; held Cross before
entry cannot open it. `document_open` is the only new state value and is reset
during Gameplay initialization.

While `document_open` is nonzero, each update refreshes the existing frame
timer and returns without movement, collision, terminal interaction, barrier
changes, document interaction, or viewport calculation. Cross and movement are
ignored. Only `pressed_buttons & PAD_CIRCLE` clears the flag.

The overlay renders a full-screen dark primitive and only the fixed text through
the existing debug renderer. Dismissal resumes normal Gameplay immediately
with player position, viewport, terminal state, and barrier state unchanged.
Because timing remains refreshed, dismissal cannot produce a stored-delta
movement jump. The note can be opened repeatedly and stores no read state.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
2cab9168e5412fe1de88d1eb79ccf16bbd184c48d725cbabb10fcfdfbcfd3a0e
2cab9168e5412fe1de88d1eb79ccf16bbd184c48d725cbabb10fcfdfbcfd3a0e
2cab9168e5412fe1de88d1eb79ccf16bbd184c48d725cbabb10fcfdfbcfd3a0e
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA route. It confirmed:

1. Splash, Main Menu, START, player, world, landmarks, viewport, obstacles,
   collision, terminal, barrier, texture, and double buffering remain correct.
2. The note appears beyond the barrier, scrolls correctly, and is non-solid.
3. Its prompt appears only during half-open overlap; Cross outside and held
   Cross before entry do not open it.
4. A newly pressed Cross opens a complete, readable full-screen text overlay.
5. Movement, Cross, and gameplay state changes are frozen while reading.
6. Newly pressed Circle alone dismisses the overlay.
7. Player and viewport positions plus terminal and barrier states are identical
   before opening and after dismissal, with no timing jump.
8. The note remains visible and can be read repeatedly without read tracking.
9. Controller disconnection cannot accidentally open or dismiss the overlay.
10. No black screen, corruption, persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`2cab9168e5412fe1de88d1eb79ccf16bbd184c48d725cbabb10fcfdfbcfd3a0e`.
The note rendered at its expected location, remained non-solid and repeatable,
and opened and dismissed through the intended button transitions. Gameplay
resumed at the preserved player and viewport position without a timing jump.
Existing world, collision, terminal, barrier, texture, and double-buffered
rendering remained correct. No black screen, corruption, lockup, or crash
occurred. The known libdebug framebuffer-zero text flicker remains outside this
milestone.

## Limitations

- There is exactly one fixed document and one fixed text block.
- The overlay has no scrolling, pagination, animation, fade, or alternate font.
- There is no collection, inventory, persistence, dialogue, objective, or
  generalized document/UI architecture.
- Real PlayStation 2 hardware remains untested.
