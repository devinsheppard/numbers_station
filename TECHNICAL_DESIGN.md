# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## State architecture

Milestone 006 extends the explicit state manager with Gameplay. The manager
knows exactly three states:

- Splash
- Main Menu
- Gameplay

Each owns initialization, update, rendering, and shutdown. The manager still
uses one enum and switch dispatch; there is no stack, registry, factory, scene
graph, entity system, or support for unspecified future states.

## Main Menu to Gameplay transition

Main Menu checks the controller's newly pressed mask for `PAD_START`. It raises
a transition request rather than changing state directly. After updating Main
Menu, the manager observes the request and performs its normal sequence:

```text
shut down Main Menu
select Gameplay
initialize Gameplay
```

Gameplay is rendered immediately after that update. Holding START does not
retrigger the transition because Main Menu uses the pressed edge, not the held
button mask.

## Gameplay lifecycle

Gameplay owns one static `Player` value. Initialization initializes that value,
update delegates to the player once, rendering draws diagnostics and the player
once, and shutdown clears the player fields. Gameplay requests no further
transition and remains active until reset or power-off.

## Player module

The player contains only:

```text
x character-cell position
y character-cell position
movement speed in cells per frame
```

It starts at `(40, 18)` with speed `1`. Update reads the input module's current
button mask so movement continues while a D-pad direction is held:

- Up decreases Y.
- Down increases Y.
- Left decreases X.
- Right increases X.

Diagonal movement is allowed. Opposing directions cancel on their axis. After
movement, X is clamped to `1–78` and Y to `10–27`. These conservative text-grid
bounds keep the `@` marker visible and reserve the upper rows for diagnostics.

The positions are diagnostic screen cells, not world coordinates or pixels.
There are no sprites, textures, animation, physics, collision, camera, or 3D
rendering.

## Frame flow

Each application frame performs:

```text
poll controller
update active state
render active state
```

Gameplay rendering begins a vertical-blank-synchronized frame, writes the
milestone header, player coordinates, and controls, then asks the player to draw
its `@` marker.

## Video responsibility

The video module remains a thin PS2SDK debug-display wrapper. Its text function
now accepts formatted text because Gameplay must display changing coordinates.
It is not a general renderer.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libgraph`, `libdma`, and C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
