# Milestone 017 Verification

Date: 2026-07-16

## Scope

Milestone 017 adds one fixed current objective to normal Gameplay. It progresses
through three compile-time strings using only the existing terminal state and a
Gameplay-local record of the three fixed documents opened during the current
Gameplay entry. There is no quest, mission, journal, inventory, collection,
save, scripting, event, dialogue, or generalized objective architecture.

## Objective sequence

1. Initial: `Activate the relay terminal.`
2. Existing terminal activated: `Search beyond the barrier.`
3. All three distinct documents opened: `Investigate the transmission source.`

One local unsigned mask assigns one bit to each entry in the existing fixed
document table. A bit is set only when newly pressed Cross successfully opens
that document. Reopening it sets the same bit and has no additional effect.
The final objective is selected when all three bits are set; otherwise terminal
activation selects the second objective, and the initial string is used before
activation.

Gameplay initialization clears the mask. Document text, placement, overlap,
opening, overlay freeze, Circle dismissal, repeatability, and lack of collected
state remain unchanged.

## Native build

Three independent clean builds completed without compiler or linker warnings.
Their SHA-256 values were:

```text
3fbf542bb89fff7a77f2749b957c2ba50a9941d0accbf84183dd2702893ee7dd
3fbf542bb89fff7a77f2749b957c2ba50a9941d0accbf84183dd2702893ee7dd
3fbf542bb89fff7a77f2749b957c2ba50a9941d0accbf84183dd2702893ee7dd
```

The three preserved ELF artifacts compared byte-for-byte and were identical.

## External PCSX2 verification

Runtime validation passed using PCSX2 through the Batocera
EmulationStation/BUA route. The following checks were confirmed:

1. Splash, Main Menu, START, player, world, viewport, collision, terminal,
   barrier, documents, textures, and double buffering remain correct.
2. Normal Gameplay initially displays `Activate the relay terminal.`
3. Reading an accessible note before terminal activation does not skip the
   initial objective.
4. Terminal activation changes the objective to `Search beyond the barrier.`
5. Reading one or two distinct documents does not advance to the final string.
6. Opening all three distinct documents in any order changes the objective to
   `Investigate the transmission source.`
7. Reopening notes leaves the final objective unchanged and every document
   remains readable and present in the world.
8. The reading overlay still freezes Gameplay, Circle dismisses it, and player
   and viewport positions resume exactly without a movement jump.
9. Starting a new Gameplay entry restores the initial objective.
10. Movement, analog controls, collision, wall sliding, terminal, barrier,
    viewport, rendering, texture handling, and double buffering remain correct.
11. No black screen, rendering corruption, lockup, or crash occurred.

The externally validated ELF SHA-256 is:

```text
3fbf542bb89fff7a77f2749b957c2ba50a9941d0accbf84183dd2702893ee7dd
```

Milestone 017 is runtime validated. The known libdebug framebuffer-zero text
flicker remains outside this milestone.

## Limitations

- There is exactly one current objective selected from three fixed strings.
- Progress exists only for the current Gameplay entry and is not saved.
- There is no objective list, journal, marker, reward, quest, mission, event,
  inventory, scripting, or generalized progression architecture.
