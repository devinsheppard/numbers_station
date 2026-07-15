# Milestone 011 Verification

Date: 2026-07-15

## Scope

Milestone 011 adds proportional left analog-stick movement to the existing
primary-controller input path. D-pad movement remains supported. There is no
right-stick behavior, rebinding, configurable deadzone, input action layer,
rumble, pressure input, multiple-controller support, acceleration, smoothing,
or generalized input framework.

## Input representation

PS2SDK reports `ljoy_h` and `ljoy_v` as unsigned bytes with nominal center 128.
The input state stores each value after subtracting 128, producing signed values
from `-128` through `127`. Horizontal negative is left; vertical negative is up.

If the primary pad advertises DualShock support, the existing port and slot
request locked DualShock mode. Analog axes remain neutral while that mode change
is pending. Disconnected, unstable, unreadable, and digital-only pads publish
zero for both axes while preserving the existing safe button behavior.

## Movement processing

Centered negative values divide by 128 and nonnegative values divide by 127.
For normalized analog vector magnitude `m`, the fixed radial deadzone is:

```text
analog magnitude = 0                         when m <= 0.25
analog magnitude = min((m - 0.25) / 0.75, 1) when m > 0.25
```

The original direction is preserved. The processed analog vector is added to
the existing D-pad vector. If the combined magnitude exceeds one, it is
normalized before multiplication by the unchanged 180-pixel-per-second speed
and frame delta.

World dimensions, player bounds, viewport calculation, environment rendering,
texture upload, texture binding, player drawing, and frame presentation remain
unchanged.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
9a3376ffc513a44c0e1369c17ccf1720be30d28ec9f250f2b13d94894f5da3ff
9a3376ffc513a44c0e1369c17ccf1720be30d28ec9f250f2b13d94894f5da3ff
9a3376ffc513a44c0e1369c17ccf1720be30d28ec9f250f2b13d94894f5da3ff
```

The three ELF artifacts were compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation was completed in PCSX2 through the Batocera
EmulationStation/BUA route so controller mapping was active. It confirmed:

1. Splash, Main Menu, START transition, environment, texture, and scrolling
   retain their verified behavior.
2. D-pad cardinal and diagonal movement still works at the existing speed.
3. Centered stick input and motion inside the deadzone produce no drift.
4. Motion just outside the deadzone produces slow movement.
5. Increasing deflection progressively increases speed.
6. Full cardinal and diagonal analog input cannot exceed maximum speed.
7. Mixed D-pad and analog input cannot exceed maximum speed.
8. Opposing digital and analog directions behave deterministically.
9. Movement remains frame-rate independent and viewport following remains
   correct.
10. Player/world/viewport bounds remain correct at every edge and corner.
11. Disconnecting or disabling the pad causes neutral axes without drift,
    lockup, or crash; reconnecting restores input.
12. Texture rendering and double-buffered presentation remain correct.
13. No black screen, corruption, persistent flicker, lockup, or crash occurs.

All checks passed with ELF SHA-256
`9a3376ffc513a44c0e1369c17ccf1720be30d28ec9f250f2b13d94894f5da3ff`.
The textured player and scrolling world remained correct, analog traversal and
viewport following worked as intended, and no rendering regression,
corruption, lockup, or crash occurred. The known libdebug framebuffer-zero
text flicker remains outside this milestone.

## Limitations

- Only the primary controller's left stick is used.
- The radial deadzone is a fixed compile-time value.
- There is no calibration or per-device configuration.
- Real PlayStation 2 hardware remains untested.
