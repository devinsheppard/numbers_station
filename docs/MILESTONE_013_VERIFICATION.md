# Milestone 013 Verification

Date: 2026-07-15

## Scope

Milestone 013 adds one fixed world-space signal terminal. The player can
activate it by newly pressing Cross while overlapping it. One Gameplay-local
flag records the result. There is no interaction array, identifier, type,
callback, trigger system, event system, dialogue, inventory, save data, or
generalized interaction architecture.

## Fixed terminal data

The terminal has one compile-time definition:

| Property | Value |
| --- | --- |
| World position | `(760,660)` |
| Dimensions | `96×64` pixels |
| Inactive color | RGB `(0x48,0xd0,0x70)` |
| Activated color | RGB `(0xf0,0xd0,0x40)` |

The same rectangle supplies its visual and activation bounds. It is near but
does not overlap the player's `(788,588)` initial 24×24 bounds. It is separated
from all four solid obstacles and remains reachable without changing the world.
The terminal is not part of either collision-resolution loop and never changes
player position.

## Proximity and input

Player bounds remain:

```text
left   = player.x
top    = player.y
right  = player.x + player.width
bottom = player.y + player.height
```

Both axes reuse the existing half-open overlap rule:

```text
first_start < second_end && first_end > second_start
```

Touching an edge is not sufficient. Gameplay tests the existing button
transition only after movement and collision resolution:

```text
pressed_buttons & PAD_CROSS
```

Pressing or holding Cross outside the region does nothing. Holding it while
entering produces no new press transition. The player must release and newly
press Cross while overlapping. Disconnected or unavailable input cannot create
a valid press transition.

## Activation and rendering

`signal_terminal_activated` is the only new state value. Gameplay
initialization clears it. A valid Cross transition sets it once, and it stays
latched for the remainder of that Gameplay entry. A future Gameplay
initialization resets it; there is no persistence beyond the state lifecycle.

The inactive terminal is green and displays `Press CROSS to activate.` only
while the player overlaps it. The activated terminal is yellow and displays
`Signal terminal activated.` throughout the remainder of Gameplay. Both states
use the existing viewport-relative rectangle clipping and primitive renderer.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
4d7502ee4df2dc9e2dcec221baef773126bbb9e46320e430165281e54f161153
4d7502ee4df2dc9e2dcec221baef773126bbb9e46320e430165281e54f161153
4d7502ee4df2dc9e2dcec221baef773126bbb9e46320e430165281e54f161153
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA route. It confirmed:

1. Splash, Main Menu, START, movement, world, obstacles, collision, texture,
   viewport, and double buffering retain their verified behavior.
2. The terminal is visible, scrolls with the world, and does not block movement.
3. No prompt appears and Cross has no effect outside the terminal.
4. Entering without a new Cross press does not activate the terminal.
5. Holding Cross before entry does not activate it.
6. While overlapping, `Press CROSS to activate.` appears.
7. A newly pressed Cross activates exactly once, changes the terminal color,
   and displays `Signal terminal activated.`
8. Leaving, returning, and pressing Cross again do not reset or duplicate the
   activation.
9. Controller disconnection cannot accidentally activate the terminal.
10. Texture rendering remains correct after terminal primitives and text.
11. No black screen, corruption, persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`4d7502ee4df2dc9e2dcec221baef773126bbb9e46320e430165281e54f161153`.
The rendered terminal matched its interaction bounds, remained non-solid, and
activated exactly through the intended Cross transition. Its state remained
latched while movement, collision, viewport, texture, and double-buffered
rendering stayed correct. No black screen, corruption, lockup, or crash
occurred. The known libdebug framebuffer-zero text flicker remains outside this
milestone.

## Limitations

- There is exactly one fixed interaction location.
- Activation lasts only for the current Gameplay-state lifetime.
- There is no interaction framework, trigger system, event dispatch, dialogue,
  item, objective, audio, transition, or save behavior.
- Real PlayStation 2 hardware remains untested.
