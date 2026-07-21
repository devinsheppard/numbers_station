# Milestone 025 Verification

Date: 2026-07-21

## Scope

Milestone 025 allows newly pressed Circle on the existing Gameplay pause overlay
to request Main Menu through the established `main_menu_requested` path. It adds
no transition mechanism, application state, reset path, confirmation dialog,
retained progress, save system, or menu framework.

## Input and lifecycle behavior

During ordinary Gameplay, newly pressed START opens pause and Circle remains
inactive. While paused, newly pressed START resumes and newly pressed Circle
sets `main_menu_requested`. Circle is checked first if both are newly pressed.
Held input cannot repeat either action because only `pressed_buttons` is used.

The paused update returns immediately after input handling, so movement,
collision, viewport, interactions, objectives, and radio processing remain
frozen. The unchanged state manager observes the request after the Gameplay
update, performs normal Gameplay shutdown, initializes Main Menu, and renders no
additional Gameplay frame. START from Main Menu uses normal Gameplay
initialization, which clears all session-local progress and request state.

Document and receiver overlays still consume Circle as local dismissal before
pause handling. The completion overlay still uses Circle to request Main Menu.
START cannot toggle pause while any of those modal overlays owns input.

## Pause presentation

The existing dark pause panel displays:

```text
PAUSED

Press START to resume
Press CIRCLE for Main Menu
```

The stable active-framebuffer bitmap-run text path and double-buffered renderer
are unchanged.

## Native build verification

Run three independent clean native ARM64 builds. Each build must complete with
no compiler warnings, linker warnings, or errors. Preserve each ELF, calculate
its SHA-256, and compare all three files byte-for-byte.

The three clean builds produced:

```text
295a04586d7c3eaec4fe265b8e8a70e5525c4e12c9c4a8a233718886d2ca938c
295a04586d7c3eaec4fe265b8e8a70e5525c4e12c9c4a8a233718886d2ca938c
295a04586d7c3eaec4fe265b8e8a70e5525c4e12c9c4a8a233718886d2ca938c
```

All builds were warning-free and error-free. All three preserved ELF files
compared byte-for-byte and were identical.

Build verification complete.
External Batocera/PCSX2 runtime validation passed.

## External runtime result

Runtime validation used the established Batocera EmulationStation/BUA PCSX2
launch path. START opened and resumed pause correctly, Circle returned from pause
to Main Menu, and a subsequent Gameplay entry reset all session state. Ordinary
Gameplay Circle input, document and receiver dismissal, and completion-overlay
Main Menu return remained correct. Rendering, stable text, and double buffering
showed no regression; no stale state or crash occurred.

A deliberate simultaneous START-and-Circle stress test occasionally resumed
Gameplay. Circle-only behavior was consistently correct, and this was attributed
to the two button edges being sampled on different frames rather than a failure
of the same-update Circle priority. It is recorded as a non-blocking input-timing
limitation.

## External validation checklist

1. Enter ordinary Gameplay, newly press START, and confirm the stable pause
   overlay opens with both controls visible.
2. Verify movement, collision, interactions, viewport, objectives, radio timing,
   transmission progression, and direction sampling remain frozen while paused.
3. Newly press START and confirm exact resume position and state with no timing,
   movement, viewport, or radio jump.
4. Pause again, newly press Circle, and confirm Main Menu appears exactly once
   without a visibly resumed or additional Gameplay frame.
5. Hold Circle before pausing and through a transition. Confirm it causes no
   unintended exit, repeated action, or input leakage.
6. Press START from Main Menu and confirm player, viewport, terminal, barrier,
   documents, receiver, transmission source, completion, overlays, pause,
   request, radio, signal, and objective state are fully reset.
7. Confirm Circle remains inactive during ordinary Gameplay, locally dismisses
   document and receiver overlays, and still returns to Main Menu from the
   completion overlay. Confirm START does not affect those modal overlays.
8. Repeat Main Menu -> Gameplay -> Pause -> Main Menu -> Gameplay several times.
   Confirm no stale state, crash, lockup, black screen, rendering corruption,
   text flicker, or double-buffering regression.

## Status

Implementation, deterministic native builds, and external runtime validation are
complete. Milestone 025 is approved for commit.
