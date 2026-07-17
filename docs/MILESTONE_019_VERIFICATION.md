# Milestone 019 Verification

Date: 2026-07-16

## Scope

Milestone 019 adds one fixed world-space radio source that displays a looping,
deterministic sequence of compile-time number groups while the player is nearby.
There is no streamed audio, voice acting, sound effect, wall clock, random
generation, scripting, dialogue, event, inventory, save, or radio framework.

## Radio source

- Position: `(1510,1120)`
- Dimensions: `32×32`
- Color: RGB `(0xd8,0x48,0xd0)`
- Activation: center-to-center distance at most 180 pixels
- Behavior: visible, viewport-relative, clipped, and non-solid

Squared distance is compared with `180×180`; the radio does not participate in
collision. Outside the radius, no transmission text is rendered.

## Deterministic transmission

The fixed table is:

```text
41729 08314 55190
66302 19447 82016
09531 77208 43665
28144 00973 11852
55017 36490 70221
```

Inside the radius, the current entry is displayed near the top of normal
Gameplay. Existing `frame_timer.delta_seconds` accumulates locally and advances
the index every three seconds. The fifth entry wraps to the first. Leaving range
clears elapsed time and restores index zero, making every approach reproducible.
Document and completion overlays pause this local update along with Gameplay.

The transmission is informational and cannot alter objectives, world state,
documents, extraction, movement, collision, viewport, or rendering state.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
f122c27eb3c9f17339b6a184cea083484ef99c7596d596a50860ba33058aab24
f122c27eb3c9f17339b6a184cea083484ef99c7596d596a50860ba33058aab24
f122c27eb3c9f17339b6a184cea083484ef99c7596d596a50860ba33058aab24
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, movement, viewport, collision, terminal, barrier,
   documents, objectives, extraction, overlays, textures, and double buffering
   remain correct.
2. The magenta `32×32` marker renders at `(1510,1120)`, scrolls correctly, and
   remains non-solid.
3. No transmission appears outside the fixed radius.
4. The first number group appears immediately on entering the radius.
5. The five entries advance every three nearby Gameplay seconds in documented
   order and loop after the fifth.
6. Gameplay movement and interactions continue while radio text is visible.
7. Leaving hides the display; re-entering restarts at the first entry.
8. Document and completion overlays retain their established behavior.
9. Radio reception changes no objective or world state.
10. Existing movement, analog controls, collision, wall sliding, viewport,
    terminal, barrier, documents, objective progression, extraction, texture
    rendering, and double buffering remain correct.
11. No black screen, rendering corruption, lockup, or crash occurred.

The externally validated ELF SHA-256 is:

```text
f122c27eb3c9f17339b6a184cea083484ef99c7596d596a50860ba33058aab24
```

Milestone 019 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There is one fixed radio, one fixed radius, and five fixed text transmissions.
- There is no audio, wall clock, randomness, radio inventory, dialogue, event,
  scripting, save data, or generalized radio/message system.
