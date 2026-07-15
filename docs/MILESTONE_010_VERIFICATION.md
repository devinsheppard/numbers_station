# Milestone 010 Verification

Date: 2026-07-15

## Scope

Milestone 010 replaces screen-bounded movement with a fixed 1600×1200 world and
a clamped 640×448 viewport. Gameplay owns two viewport offsets and renders a
deterministic technical environment made from clipped colored rectangles. The
player remains the verified Milestone 009 textured sprite.

There is no camera or world module, collision, map loading, real geography,
imported asset, animation, interaction, entity system, scene graph, physics, or
generalized clipping facility.

## Native build

Three consecutive clean builds completed with no compiler or linker warnings.
Each produced `out/bin/numbers_station.elf` with SHA-256:

```text
104cb6b7c8bcdffe73447c6ac9ffe23fe85a30b6915294d319b6835970006bcb
104cb6b7c8bcdffe73447c6ac9ffe23fe85a30b6915294d319b6835970006bcb
104cb6b7c8bcdffe73447c6ac9ffe23fe85a30b6915294d319b6835970006bcb
```

The three ELF files were compared byte-for-byte and were identical.

## Spatial rules

- World dimensions: 1600×1200 pixels.
- Viewport dimensions: 640×448 pixels.
- Player dimensions: 24×24 pixels.
- Player bounds: X `[0,1576]`, Y `[0,1176]`.
- Viewport bounds: X `[0,960]`, Y `[0,752]`.
- Viewport target: player center minus half the viewport dimensions.
- Player screen position: world position minus viewport offset.

The environment begins with a base rectangle covering the full display. Nine
fixed regions cover the starting area, the four cardinal directions, and the
four world corners. Each region is intersected with the viewport before its
visible portion is drawn.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA launch route so controller mapping was active. It
confirmed:

1. Splash transitions to Main Menu and START enters Gameplay.
2. The environment fills the complete display with no uninitialized regions.
3. The textured player retains the verified texture colors and orientation.
4. Cardinal and diagonal movement remain smooth and frame-rate independent.
5. Diagonal movement remains normalized.
6. Player world coordinates exceed the viewport dimensions during traversal.
7. Viewport values follow the player and landmarks scroll relative to it.
8. Viewport X/Y clamp at all four world edges without exposing outside space.
9. Player X/Y clamp with the full sprite inside every world edge and corner.
10. Player screen placement remains correct while the viewport is clamped.
11. Primitive landmarks, diagnostic text, and the textured player coexist
    without texture corruption or stopped rendering.
12. Display/draw buffer alternation continues without a black-screen regression.
13. No persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`104cb6b7c8bcdffe73447c6ac9ffe23fe85a30b6915294d319b6835970006bcb`.
There was no texture corruption, persistent flicker, lockup, black-screen
regression, or crash. The known libdebug framebuffer-zero text flicker remains
outside Milestone 010 and was not treated as a regression.

## Limitations

- The colored rectangles are technical landmarks, not final world artwork.
- There is no collision or interaction; only the outer world boundary constrains
  the player.
- Real Irish Wilderness terrain, roads, structures, and content are not present.
- Real PlayStation 2 hardware remains untested.
