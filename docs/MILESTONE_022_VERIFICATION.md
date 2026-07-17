# Milestone 022 Verification

Date: 2026-07-17

## Scope

Milestone 022 distinguishes the stationary radio receiver from the existing
cyan transmission-source area. It adds one session-local receiver-confirmation
flag and explicit newly pressed Cross inspection at the source. It adds no new
world object, return journey, quest, event, objective, interaction, mission,
save, or generalized progression system.

## World locations

- Stationary radio receiver: `(1510,1120)`, `32×32`, magenta and non-solid.
- Transmission source: `(1360,1020)`, `80×80`, cyan and non-solid.

The radio's fixed 180-pixel proximity radius continues to control transmission
and direction-finder display. The cyan rectangle does not emit broadcasts and
does not modify radio timing or direction state.

## Receiver confirmation and objectives

Newly pressed Cross while overlapping the radio opens the repeatable
`RADIO RECEIVER` overlay and sets `radio_source_confirmed`. Initialization and
shutdown clear the flag. Repeat inspection cannot toggle it.

Objective priority is:

1. Before terminal activation: `Activate the relay terminal.`
2. After activation and before all notes: `Search beyond the barrier.`
3. All notes read, receiver unconfirmed: `Inspect the stationary receiver.`
4. All notes read, receiver confirmed: `Investigate the transmission source.`

Early receiver inspection records confirmation but does not replace the earlier
terminal or document objective.

## Transmission-source inspection

The former `extraction_zone` is named `transmission_source`; its overlap helper
and inside flag use the same terminology. Before both the complete document mask
and receiver confirmation are present, overlap and Cross have no effect.

When eligible and overlapping, normal Gameplay displays:

```text
Press CROSS to inspect
```

Overlap alone never opens completion. Only `pressed_buttons & PAD_CROSS` opens
the existing completion overlay. Holding Cross before entry cannot inspect the
source. Newly pressed Circle retains the established dismissal and frame-timing
behavior. No return to another location is required.

## Native build

Three corrected independent clean builds completed without compiler or linker
warnings. Their SHA-256 values were:

```text
c8b200af84d380ec7822ecd2c3040ec98913efea6842c719466801f11c8bca91
c8b200af84d380ec7822ecd2c3040ec98913efea6842c719466801f11c8bca91
c8b200af84d380ec7822ecd2c3040ec98913efea6842c719466801f11c8bca91
```

All three preserved ELF files compared byte-for-byte and were identical.

Build verification complete.
Runtime validation passed.

## External PCSX2 checklist

1. Enter the cyan source area early; confirm no prompt or completion, including
   after pressing Cross.
2. Read all documents without inspecting the receiver and confirm
   `Inspect the stationary receiver.`
3. Revisit the source and confirm it remains unavailable.
4. Inspect the stationary radio, verify the `RADIO RECEIVER` overlay, and
   confirm `Investigate the transmission source.`
5. Confirm radio broadcasts and direction status remain proximity-bound and
   disappear or reset according to established behavior after leaving range.
6. Enter the cyan source area, confirm the prompt, and verify entry alone does
   not complete the sequence.
7. Newly press Cross and confirm the existing completion overlay opens; newly
   press Circle and confirm safe dismissal without a movement jump.
8. In a fresh entry, inspect the radio first, verify earlier objectives retain
   priority, read all notes, then inspect the source without reinspecting radio.
9. Hold Cross while entering the source and confirm no activation; release and
   newly press Cross to verify inspection.
10. Confirm reset clears document and receiver confirmation and that movement,
    collision, scrolling, landmarks, terminal, barrier, documents, radio,
    direction finder, rendering, timing, texture, and double buffering remain
    correct without crash, corruption, black screen, or lockup.

## External PCSX2 result

Runtime validation passed through the established Batocera
EmulationStation/BUA PCSX2 launch path. The stationary receiver and cyan
transmission source remained distinct, objective priority and early-discovery
gating behaved correctly, source completion required newly pressed Cross, and
the existing Gameplay, radio, rendering, timing, texture, and presentation
systems showed no regression.

The externally validated ELF SHA-256 is:

```text
c8b200af84d380ec7822ecd2c3040ec98913efea6842c719466801f11c8bca91
```

Milestone 022 is runtime validated.

## Limitations

- Both locations and all text are fixed compile-time data.
- Confirmation exists only for the current Gameplay entry.
- There is no persistence, audio, quest framework, event framework, objective
  framework, interaction framework, or additional world content.
