# Milestone 020 Verification

Date: 2026-07-16

## Scope

Milestone 020 adds a deterministic direction indication to the existing ambient
radio. It introduces no audio, compass, minimap, HUD framework, navigation or
radio subsystem, scripting, event, quest, inventory, NPC, or save behavior.

## Direction comparison

The implementation retains the existing radio position, marker, 180-pixel
radius, five compile-time transmissions, three-second interval, loop, and
out-of-range sequence reset.

During normal in-range Gameplay, it compares the current squared player-to-radio
center distance with the previous frame. The fixed tolerance is 16
squared-pixel units:

- Current distance more than 16 below previous: `SIGNAL STRENGTHENING`
- Current distance more than 16 above previous: `SIGNAL WEAKENING`
- Difference within 16: `SIGNAL STABLE`

The first in-range sample is stable. Leaving range invalidates the previous
sample and resets the state. The selected status is rendered beneath the current
number group and has no Gameplay effect.

Document and completion overlays return before radio and direction updates, so
both remain paused through those established overlay paths. No extra timer or
wall-clock source is used.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
1c800fdd11e80939a2985cceeaa8451c19082edc2b4c250010d9bb07f0f986f9
1c800fdd11e80939a2985cceeaa8451c19082edc2b4c250010d9bb07f0f986f9
1c800fdd11e80939a2985cceeaa8451c19082edc2b4c250010d9bb07f0f986f9
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, movement, viewport, collision, terminal, barrier,
   documents, objectives, extraction, overlays, textures, double buffering, and
   the existing radio sequence remain correct.
2. No radio or direction text appears outside the fixed radius.
3. Entry displays the first transmission and `SIGNAL STABLE`.
4. Remaining stationary or changing squared distance by no more than 16 retains
   `SIGNAL STABLE`.
5. Moving measurably toward the source displays `SIGNAL STRENGTHENING`.
6. Moving measurably away while still in range displays `SIGNAL WEAKENING`.
7. Direction status responds while normal movement and interaction continue.
8. Document and completion overlays pause radio timing and direction updates.
9. Leaving hides both lines; re-entry restarts the transmission and stable
   direction state deterministically.
10. Direction status changes no objective or world state.
11. Existing movement, collision, wall sliding, viewport, terminal, barrier,
    documents, extraction, rendering, texture handling, and double buffering
    remain correct.
12. No black screen, rendering corruption, lockup, or crash occurred.

The externally validated ELF SHA-256 is:

```text
1c800fdd11e80939a2985cceeaa8451c19082edc2b4c250010d9bb07f0f986f9
```

Milestone 020 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- Direction is derived only from consecutive squared-distance samples.
- There is one fixed tolerance and no configurable sensitivity.
- There is no audio, compass, minimap, HUD framework, navigation system,
  scripting, event system, inventory, NPC, or persistence.
