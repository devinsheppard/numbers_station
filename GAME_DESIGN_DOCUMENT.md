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

## Milestone 004 Boundary

No gameplay is implemented in Milestone 004.

The controller diagnostic framework:

- Initializes the display
- Clears the screen
- Polls the primary PS2 controller once per frame
- Tracks current, pressed, and released button states
- Displays connection state, frame count, and the last pressed button
- Remains running until reset or power off

It deliberately contains no gameplay or speculative engine systems.

## Revision Log

### 2026-07-11 - Project 001

Initial design scope recorded from the project foundation prompt.
