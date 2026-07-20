# Milestone 023 Verification

Date: 2026-07-17

## Scope

Milestone 023 returns from the existing completion overlay to Main Menu through
the existing state manager. It adds one narrow request flag and query, with no
new application state, transition framework, event bus, callback, queue, scene
stack, persistence, or save behavior.

## Transition interface

Gameplay owns one `bool main_menu_requested`. Initialization and shutdown clear
it. Only `pressed_buttons & PAD_CIRCLE` while `completion_overlay_open` sets the
request. `gameplay_state_is_main_menu_requested()` exposes it to the existing
state manager.

After `gameplay_state_update()`, the Gameplay branch of `state_manager_update()`
checks the request and calls the existing `state_change()` path. That path runs
`gameplay_state_shutdown()`, selects Main Menu, and runs
`main_menu_state_initialize()`. The application then renders Main Menu in the
same frame; completed Gameplay does not resume.

## Overlay isolation and reset

Circle on a document clears only the document overlay. Circle on the stationary
receiver clears only the receiver overlay. Circle in ordinary Gameplay remains
unassigned. The completion request is therefore exclusive to the completion
overlay.

START uses the unchanged Main Menu input path. A new Gameplay initialization
resets player and viewport position, terminal and barrier state, document mask
and overlay, receiver confirmation and overlay, transmission-source overlap,
completion overlay and return request, and radio timing and direction state.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
4ce530df075a09b22245e348d94531dc51bbb131a7910de732eea419d123c57b
4ce530df075a09b22245e348d94531dc51bbb131a7910de732eea419d123c57b
4ce530df075a09b22245e348d94531dc51bbb131a7910de732eea419d123c57b
```

All three preserved ELF files compared byte-for-byte and were identical.

Build verification complete.
Runtime validation passed.

## External PCSX2 checklist

1. Confirm Splash, Main Menu, START, movement, collision, wall sliding,
   scrolling, landmarks, terminal, barrier, documents, receiver, broadcasts,
   direction finder, source, objectives, rendering, textures, timing, and
   double buffering remain correct.
2. Open every document type, press Circle, and confirm only the document closes.
3. Inspect the stationary receiver, press Circle, and confirm only that overlay
   closes and Gameplay continues.
4. Press Circle during ordinary Gameplay and confirm no state transition.
5. Complete the Milestone 022 sequence and confirm the completion overlay opens
   with movement and Gameplay paused.
6. Newly press Circle and confirm Main Menu renders without an intervening
   completed Gameplay frame and the transition occurs once.
7. Hold Circle through the transition and confirm no additional action.
8. Press START and confirm a fresh player position, initial objective, inactive
   terminal, restored barrier, unread documents, unconfirmed receiver,
   unavailable source completion, closed overlays, and reset radio state.
9. Complete at least two full Main Menu → Gameplay → completion → Main Menu →
   Gameplay cycles and confirm no stale request, retained progress, input lock,
   timing jump, black screen, corruption, crash, or lockup.

## External PCSX2 result

Runtime validation passed through the established Batocera/PCSX2 workflow. A
new Gameplay entry began completely reset, completion remained unavailable
until every report had been read, newly pressed Circle on the transmission-
source completion returned to Main Menu, and starting again produced a fresh
playthrough with no retained progress.

The externally validated ELF SHA-256 is:

```text
4ce530df075a09b22245e348d94531dc51bbb131a7910de732eea419d123c57b
```

Milestone 023 is runtime validated.

## Limitations

- The request supports only the existing Gameplay-to-Main-Menu transition.
- Completion progress is not persisted after Gameplay shutdown.
- There is no transition framework, event framework, scene stack, save system,
  or additional application state.
