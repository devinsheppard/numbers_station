# Milestone 016 Verification

Date: 2026-07-16

## Scope

Milestone 016 expands the established reading mechanic to three fixed world
documents with unique compile-time text. There is no inventory, collection,
document-list UI, objective, ordering requirement, progress tracking, save
data, dialogue system, scripting, or event system.

## Fixed document data

All documents are 48×48 pixels, use RGB `(0xe0,0xd4,0xa8)`, remain non-solid,
and share their rectangle between rendering and interaction:

| Location | Title |
| --- | --- |
| `(980,680)` | `FIELD NOTE` |
| `(260,260)` | `MAINTENANCE LOG` |
| `(1320,940)` | `FREQUENCY RECORD` |

The three entries form one fixed compile-time table local to Gameplay. Each
contains only its world rectangle and text pointer. This is not exposed as a
document system or collection and stores no per-document runtime state.

## Compile-time narratives

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

```text
MAINTENANCE LOG

The backup receiver was missing
when the morning crew arrived.

Mud covered the floor, but no
tracks led back to the road.

At dusk, its carrier keyed once
from somewhere north of the creek.

R. Vale

Press CIRCLE to close
```

```text
FREQUENCY RECORD

The numbers changed tonight.

Between the groups, a woman said
my name in a voice I remember.

The transmitter was unplugged.
The speaker kept counting.

E. Mercer

Press CIRCLE to close
```

## Shared reading path

Normal rendering draws all fixed entries through the existing clipping helper.
One overlap helper is reused for prompting and opening. Newly pressed Cross
sets the existing `document_open` flag and stores only the selected compile-time
text pointer in `open_document_text`.

The established overlay remains singular: it refreshes frame timing, freezes
Gameplay, fills the frame with a dark primitive, and draws the selected text.
Newly pressed Circle dismisses it and clears the transient pointer. No read or
collection state survives dismissal. Every note remains visible and repeatable,
and discovery order has no effect.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
0e2efc4a4a506f3765454ec8a22f6a0a7bd9a4695efb23a7139a529ac3d6afaf
0e2efc4a4a506f3765454ec8a22f6a0a7bd9a4695efb23a7139a529ac3d6afaf
0e2efc4a4a506f3765454ec8a22f6a0a7bd9a4695efb23a7139a529ac3d6afaf
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, player, world, landmarks, viewport, obstacles,
   collision, terminal, barrier, texture, and double buffering remain correct.
2. All three notes render at their documented locations, scroll correctly, and
   remain non-solid.
3. Every note uses the same 48×48 visual and half-open interaction bounds.
4. Newly pressed Cross opens the correct unique text for each note.
5. The one overlay freezes Gameplay and newly pressed Circle dismisses it for
   every note without a movement jump.
6. Player, viewport, terminal, and barrier state survive each reading exactly.
7. Notes can be read in any order and repeatedly without collection or read
   state.
8. Holding Cross before entering does not open a note, and Cross does not
   dismiss the reading overlay.
9. Existing rendering, texture upload and sampling, viewport behavior, and
   double-buffered presentation remain correct.
10. No black screen, rendering corruption, lockup, or crash occurred.

The externally validated ELF SHA-256 is:

```text
0e2efc4a4a506f3765454ec8a22f6a0a7bd9a4695efb23a7139a529ac3d6afaf
```

Milestone 016 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There are exactly three fixed compile-time notes.
- The shared overlay has no scrolling, pagination, animation, or alternate font.
- There is no inventory, collection, list UI, progress, persistence, dialogue,
  objective, scripting, or generalized document architecture.
- Real PlayStation 2 hardware remains untested.
