# Milestone 024 Verification

Date: 2026-07-20

## Scope

Milestone 024 adds one Gameplay-local pause overlay controlled by newly pressed
START. It adds no application state, state-manager change, generalized overlay,
menu framework, event bus, callback, scene stack, persistence, or save behavior.

## Pause input and ownership

`gameplay_paused` starts false and is cleared by both Gameplay initialization
and shutdown. During ordinary Gameplay, `pressed_buttons & PAD_START` sets it.
While paused, another newly pressed START clears it. Held START cannot repeatedly
toggle because held input is absent from `pressed_buttons` after its first frame.

Document, stationary-receiver, and completion overlays are checked before pause
input. START cannot open or toggle pause while any of them is active. Their
existing Circle behavior remains unchanged. While paused, Circle, Cross,
movement, and interaction inputs have no effect.

## Simulation and timing

Every update continues through `frame_timer_update()`. A paused update then
returns before movement, collision, viewport updates, terminal, documents,
receiver, source, objectives, radio timing, transmission progression, distance
sampling, and signal comparison. The refreshed timer baseline prevents the
paused interval from accumulating into the first resumed frame.

## Rendering

Normal Gameplay rendering continues while paused. After the world and textured
player are submitted, a fixed dark central panel displays:

```text
PAUSED

Press START to resume
```

The world remains visible around the panel.

The first external pause test exposed a pre-existing text-rendering mismatch:
PS2SDK `libdebug` writes its glyphs through a self-contained direct-GS packet
whose `FRAME_1` target is VRAM base zero. It cannot follow the renderer's
alternating hidden draw framebuffer. World primitives were redrawn into both
framebuffers, but text appeared only in framebuffer 0 and disappeared whenever
framebuffer 1 was displayed.

The first correction used PS2SDK `fontx_print_ascii` and mandatory
`fontx_load("rom0:KROM")`; its external test exited to the PS2 Browser before
Splash. A second correction used an embedded bitmap atlas and textured glyphs;
its external test remained completely black. Isolation Stage 0 compiled all
font-atlas execution out, but also remained black before its red Splash marker.

Restoring `src/video.c`, `src/video.h`, and `Makefile` exactly to committed
Milestone 023 produced a successful external diagnostic: red Splash, green Main
Menu, Gameplay, textured player, pause, and resume all worked. The temporary
markers have now been removed.

The remaining flicker is conclusively in `libdebug` glyph submission. Packet
inspection shows `init_scr` configuring framebuffer zero independently, while
every `scr_putchar` packet contains its own host-to-local transfer with
`BITBLTBUF` destination base zero and destination width 640. It cannot follow
the renderer's alternating `draw_buffer_index`, and no public target-selection
API exists. Restoring `FRAME_1` after a glyph would not redirect the transfer.

The minimal correction retains the known-working `init_scr` startup call and
the existing linked MSX 8x8 bitmap, but replaces only `scr_vprintf` glyph output.
`video_draw_text` formats into a fixed buffer, converts contiguous occupied bits
in each glyph row into one-pixel-high filled rectangles, and batches those
rectangles through libdraw. Text therefore uses the same active framebuffer as
the current world's primitives. It is completed before the unchanged vblank and
display/draw-buffer swap. There is no font atlas, font texture, added VRAM,
runtime file loading, BIOS font dependency, alpha state, or generalized text
system.

## Native build

The pre-correction pause implementation produced three identical clean builds
with this SHA-256:

```text
9b7f70e74710fc0a7a19ba15cbd38c4db854c08d4242e63ccb6681a50176ed11
9b7f70e74710fc0a7a19ba15cbd38c4db854c08d4242e63ccb6681a50176ed11
9b7f70e74710fc0a7a19ba15cbd38c4db854c08d4242e63ccb6681a50176ed11
```

All three preserved ELF files compared byte-for-byte and were identical.

The BIOS-dependent correction produced this deterministic SHA-256 and failed
external startup validation:

```text
c50bceda0fffd755153e0f418400718fa1fb5c3a0bc6a1a3250af8c4cc80c05a
```

The embedded-font correction produced this deterministic SHA-256 and failed
external validation with a black screen:

```text
41cd85bfefa08b12a2fc43341ea18fc079190323af12bb9cc0ab89c2676ac299
```

Isolation Stage 0 produced deterministic SHA-256
`910ea36cccf996149c85bf70d7581cbbae7228cd879de173a1c5bf5b8c95fb19`
and also remained black externally. The restored-baseline diagnostic must now
complete three warning-free, byte-identical builds before external testing.
Those builds produced:

```text
79776630937c4c51933695555ff054fe333b60a09d6535ec4f07986cf9235666
79776630937c4c51933695555ff054fe333b60a09d6535ec4f07986cf9235666
79776630937c4c51933695555ff054fe333b60a09d6535ec4f07986cf9235666
```

All three restored-baseline ELFs compared byte-for-byte and were identical.

The final active-framebuffer bitmap-run correction produced three warning-free
clean builds:

```text
e3e713e1ab35561dcd98fc9cc96bc7e26655c3fc3b35c15078f4070f7d2f2df0
e3e713e1ab35561dcd98fc9cc96bc7e26655c3fc3b35c15078f4070f7d2f2df0
e3e713e1ab35561dcd98fc9cc96bc7e26655c3fc3b35c15078f4070f7d2f2df0
```

All three final ELF files compared byte-for-byte and were identical.

Build verification complete.
External Batocera/PCSX2 runtime validation passed.

## External runtime result

External validation used the established Batocera EmulationStation/BUA PCSX2
launch path with the final ELF SHA-256
`e3e713e1ab35561dcd98fc9cc96bc7e26655c3fc3b35c15078f4070f7d2f2df0`.
Normal Splash, Main Menu, Gameplay, and textured-player presentation remained
correct. All existing text and overlays remained continuously visible without
alternating-frame flicker. Pause input, simulation and radio freeze, exact
resume behavior, overlay isolation, completion return to Main Menu, and fresh
Gameplay reset passed. Repeated full gameplay cycles produced no stale state,
black screen, Browser fallback, corruption, crash, or lockup.

## External PCSX2 checklist

1. Confirm no red Splash or green Main Menu diagnostic remains. Verify normal
   Splash, normal Main Menu, Gameplay, and the textured player.
2. Confirm Splash, Main Menu, Gameplay objective, document, receiver,
   completion, and pause text remain continuously visible. No text may disappear
   on alternating frames, become stale, or corrupt GS state.
3. Pause ordinary Gameplay with newly pressed START, verify simulation and radio
   processing freeze, then resume once with newly pressed START and confirm no
   timing, movement, viewport, or radio jump.
4. Confirm START cannot alter document, receiver, or completion overlays and
   Circle on completion still returns to Main Menu.
5. Complete at least two Main Menu → Gameplay → completion → Main Menu cycles.
   Confirm fresh-session reset, no stale pause, no Browser fallback, black
   screen, corruption, crash, or lockup.

## Limitations

- Pause is available only during ordinary Gameplay.
- The overlay is one fixed dark panel with compile-time text.
- There is no pause menu, settings, Main Menu option, new application state,
  generalized overlay system, or persistence.
