# GAME DESIGN DOCUMENT

## Project

Title: NUMBERS STATION

Genre:

- Psychological horror
- Survival horror
- Investigation
- Puzzle
- Exploration

Platform:

- Sony PlayStation 2
- Original PS2 hardware
- Current PCSX2

## Design Intent

NUMBERS STATION will be developed as if it were a commercial PlayStation 2 title released in late 2003 or early 2004.

The game must respect PS2 hardware limitations and favor stable frame rate, predictable memory use, short loading times, and maintainable systems.

## Milestone 006 Boundary

Milestone 006 implements only a player-movement prototype.

The Gameplay prototype:

- Initializes the display
- Clears the screen
- Transitions from Main Menu to Gameplay when START is newly pressed
- Owns one player with two-dimensional diagnostic-screen coordinates
- Moves a visible marker with held D-pad directions
- Clamps the marker to conservative visible text-grid bounds
- Remains running until reset or power off

It is not a level, final controller, or general gameplay architecture.

## Revision Log

### 2026-07-11 - Project 001

Initial design scope recorded from the project foundation prompt.
