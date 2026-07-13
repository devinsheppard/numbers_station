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

## Milestone 007 Boundary

Milestone 007 implements only a pixel-rendering and movement prototype.

The Gameplay prototype:

- Initializes the display
- Clears the screen
- Transitions from Main Menu to Gameplay when START is newly pressed
- Owns one player with floating-point pixel coordinates
- Draws a filled 24×24 rectangle with PS2SDK primitives
- Uses bounded delta time and normalized diagonal D-pad movement
- Clamps the entire rectangle inside the 640×448 framebuffer
- Remains running until reset or power off

It is not a level, final controller, or general gameplay architecture.

## Revision Log

### 2026-07-11 - Project 001

Initial design scope recorded from the project foundation prompt.
