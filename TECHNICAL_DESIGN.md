# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## Application architecture

Milestone 005 retains the existing application, input, and video modules and
adds only the state responsibilities currently required:

- `state_manager` owns the identity and lifecycle of the single active state.
- `splash_state` owns its timer and launch-screen rendering.
- `main_menu_state` owns START handling, menu rendering, and the placeholder
  message flag.

The manager uses an explicit two-value state enum and switch dispatch. It does
not implement a stack, registry, event bus, dynamic allocation, factories, or
support for hypothetical states.

## Startup sequence

1. `main` enters the application.
2. The application initializes SIF RPC, video, and controller input.
3. The state manager selects and initializes Splash.
4. The application enters its continuous update/render loop.

## State lifecycle

Each concrete state owns four operations:

```text
initialize
update
render
shutdown
```

For a transition, the manager calls the active state's shutdown operation,
changes the active enum, and initializes the new state. Update and render are
then dispatched only to that state. Shutdown dispatches once to the current
state before application subsystems are stopped.

## Splash state

Splash initialization records `GetTimerSystemTime`. Update compares elapsed
PS2 bus-clock time against three seconds, avoiding PAL/NTSC frame-rate
assumptions. Render displays the project title and milestone identifier. Once
the duration expires, the manager transitions automatically to Main Menu.

## Main Menu state

Main Menu initialization clears its placeholder flag. Update checks the input
module's newly pressed mask for `PAD_START`; current/held input is intentionally
not used. Pressing START sets the flag, and render adds:

```text
Gameplay not yet implemented.
```

No state transition or gameplay startup occurs. Main Menu remains active.

## Application loop integration

Each frame performs:

```text
poll controller
update active state
render active state
```

Each state begins its frame through the video module, which waits for vertical
blank and clears the display before state-owned text is drawn.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libgraph`, `libdma`, and C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
