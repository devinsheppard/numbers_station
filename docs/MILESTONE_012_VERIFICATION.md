# Milestone 012 Verification

Date: 2026-07-15

## Scope

Milestone 012 adds collision against four fixed axis-aligned world-space
rectangles. The implementation remains local to `src/gameplay_state.c`; there
is no collision engine, physics system, map format, dynamic obstacle support,
spatial partitioning, or generalized collision interface.

## Fixed obstacle data

One compile-time array supplies the position, size, display color, rendering,
and collision bounds for every solid rectangle:

| X | Y | Width | Height | Purpose |
| ---: | ---: | ---: | ---: | --- |
| 650 | 450 | 36 | 300 | Reachable vertical wall |
| 900 | 520 | 300 | 36 | Reachable horizontal wall |
| 1040 | 760 | 36 | 180 | L-corner vertical section |
| 1040 | 904 | 220 | 36 | L-corner horizontal section |

The player still begins at `(788,588)` with a 24×24 sprite, so it does not
overlap an obstacle at initialization. Bright obstacle colors distinguish them
from the existing non-solid landmarks.

The existing maximum speed is 180 pixels per second and frame delta is capped
at 100 ms. Maximum displacement is therefore 18 pixels per update. The minimum
obstacle thickness is 36 pixels.

## Collision rules

Player bounds are:

```text
left   = player.x
top    = player.y
right  = player.x + player.width
bottom = player.y + player.height
```

Bounds are half-open. Two one-dimensional ranges overlap only when the first
start is less than the second end and the first end is greater than the second
start. Equal edges touch without penetrating.

Gameplay saves the previous X/Y, invokes `player_update()` once, and retains
the proposed X/Y. Horizontal resolution uses the previous Y range. Moving
right selects the smallest crossed obstacle-left boundary; moving left selects
the largest crossed obstacle-right boundary. This selects the nearest blocker
without depending on obstacle array order.

Vertical resolution uses the resolved X range. Moving down selects the smallest
crossed obstacle-top boundary; moving up selects the largest crossed
obstacle-bottom boundary. This intentional X-then-Y order provides deterministic
corner behavior and preserves movement on an unobstructed axis for wall
sliding. Outer world clamping remains owned by the unchanged player update.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
8f6d67d97cbf5720ee8b4f2ccbe31172b9c3f819ab764f270b9573faeb37b2c5
8f6d67d97cbf5720ee8b4f2ccbe31172b9c3f819ab764f270b9573faeb37b2c5
8f6d67d97cbf5720ee8b4f2ccbe31172b9c3f819ab764f270b9573faeb37b2c5
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA route. It confirmed:

1. Splash, Main Menu, START transition, environment, texture, scrolling,
   D-pad movement, and analog movement retain their verified behavior.
2. Every obstacle stops the player flush from its left, right, top, and bottom
   sides wherever that side is reachable.
3. Cardinal, diagonal, analog, and mixed input cannot penetrate an obstacle.
4. Diagonal movement slides along the unobstructed axis without jitter.
5. Repeated pressure against a wall is stable and moving away works immediately.
6. The L-shaped corner produces repeatable X-then-Y behavior without embedding
   or trapping the player.
7. Maximum-speed movement and movement after an emulator pause cannot cross an
   obstacle.
8. Outer world and viewport boundaries remain correct at every edge and corner.
9. Obstacle visuals match collision boundaries and scroll with landmarks.
10. Texture rendering and double-buffered presentation remain correct.
11. No black screen, corruption, persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`8f6d67d97cbf5720ee8b4f2ccbe31172b9c3f819ab764f270b9573faeb37b2c5`.
Obstacle visuals and collision boundaries matched, wall sliding and the
L-shaped corner behaved correctly, and viewport, texture, and double-buffered
rendering remained stable. No black screen, corruption, lockup, or crash
occurred. The known libdebug framebuffer-zero text flicker remains outside this
milestone.

## Limitations

- Obstacles are fixed compile-time rectangles.
- Resolution is axis-aligned and always horizontal before vertical.
- There are no moving obstacles, triggers, collision layers, maps, terrain,
  physics properties, or interactions.
- Real PlayStation 2 hardware remains untested.
