# Technical Design

## Platform

Numbers Station targets original PlayStation 2 hardware and PCSX2. Development
uses a native AArch64 PS2 toolchain on Raspberry Pi 4 with Ubuntu 26.04.

## GS VRAM layout

Milestone 009 retains the 640×448, 32-bit PS2SDK display mode, immediate-mode
`libdraw` path, and two `GS_PSM_32` framebuffers. It adds one 32×32
`GS_PSM_32` RGBA texture allocated with `GRAPH_ALIGN_BLOCK` after both
page-aligned framebuffers. Its logical image is 32 pixels wide, while its GS
storage stride is the required minimum of 64 pixels.

PS2SDK's allocator reports addresses and sizes in 32-bit GS words. The current
deterministic allocation is:

| Allocation | Word range | Byte range | Size |
| --- | --- | --- | --- |
| Framebuffer 0 | `0x00000–0x45fff` | `0x000000–0x117fff` | 1,146,880 bytes |
| Framebuffer 1 | `0x46000–0x8bfff` | `0x118000–0x22ffff` | 1,146,880 bytes |
| Test texture | `0x8c000–0x8c7ff` | `0x230000–0x231fff` | 8,192 bytes |

The allocations consume 2,301,952 bytes of the GS's 4 MiB VRAM and leave
1,892,352 bytes. There is no z-buffer or other video allocation. Initialization
fails before the application loop if allocation fails, a range exceeds GS VRAM,
or the test texture overlaps either framebuffer.

At initialization:

- framebuffer 1 is attached to the GS read circuit for display;
- framebuffer 0 is selected as the drawing target;
- both framebuffers are cleared before the first presentation.

The indices describe roles, not fixed ownership. They alternate after every
presented frame.

## Test texture source and upload

The video module procedurally generates an aligned 64×32 upload surface during
initialization. Its left 32×32 pixels contain the logical image and its remaining
columns are cleared padding. Bytes are ordered red, green, blue, alpha for
`GS_PSM_32`; alpha uses the GS opaque value `0x80`. A 4×4 blue checkerboard,
pale border and N-shaped emblem make the image recognizable. Four uniquely
colored corners make its orientation testable. Generation is deterministic and
requires no runtime files, disc access, build generator, heap allocation, or
asset manager.

The GS `BITBLTBUF.DBW` and `TEX0.TBW` fields express buffer width in 64-pixel
units. The 32×32 logical image therefore uses a 64-pixel storage width. PS2SDK
receives 64×32 as the transfer rectangle and 64 as both the destination stride
and `texbuffer_t.width`. The source surface, cache-writeback range, transfer,
and reserved VRAM region are each exactly 8,192 bytes.

Upload occurs once:

1. Generate the CPU-side pixels and write back their data-cache range.
2. Build a `draw_texture_transfer` GIF DMA chain targeting the allocated
   texture buffer.
3. Append `draw_texture_flush`, submit the chain, and wait for GIF DMA.
4. Wait for GIF DMA completion before freeing the upload packet or beginning
   normal rendering.

The source pixel array remains statically owned by the video module. Texture
VRAM is owned from video initialization through shutdown.

## Textured-quad submission

`video_draw_test_sprite` is deliberately specific to the single milestone
texture. Immediately before each quad it selects nearest-neighbor sampling,
clamps U and V to the 32×32 texture, binds the RGBA texture with decal mode, and
submits a `draw_rect_textured` primitive in pixel coordinates. Rebinding at the
point of use is required because PS2SDK debug-font drawing also changes GS
texture state. The binding and textured primitive use separate completed
immediate-mode packets.

`draw_rect_textured` sets `FST=1` and expects texel-space UV inputs; libdraw
converts them to GS 12.4 fixed-point values. The quad therefore maps
`(0,0)–(32,32)` texels onto the player's existing 24×24 pixel rectangle without
changing its aspect ratio. The player retains 180
pixels/second delta-time movement and normalized diagonals. Its full-sprite
clamps remain X `[0,616]` and Y `[0,424]`.

## Frame lifecycle

All three application states use the same explicit lifecycle:

```text
video_begin_frame
draw primitives and diagnostic text
video_present_frame
```

`video_begin_frame` clears only the active draw buffer. Rectangle, clear, and
bitmap-text operations reuse one fixed 512-qword packet and complete before that
packet is reused. Text uses PS2SDK's existing 8x8 MSX bitmap data. Each occupied
horizontal glyph run becomes a one-pixel-high `draw_rect_filled` primitive,
batched with reserved packet space and submitted through the active libdraw
context.

`video_present_frame` performs:

1. Submit a `draw_finish` fence after all primitive and text commands.
2. Wait until the GS reports that fence complete.
3. Wait for vertical blank.
4. Point the filtered GS read circuit at the completed draw framebuffer.
5. Make that index the display index.
6. Make the previous display index the next draw index.
7. Submit `draw_framebuffer` and wait for completion so subsequent drawing
   targets the hidden buffer.

The next `video_begin_frame` then clears that new draw target. The display
buffer is never cleared or modified while it is being scanned out.

## Synchronization decisions

`submit_packet` calls `dma_wait_fast` before each send and `draw_wait_finish`
after the packet's finish tag. This deliberately favors correctness and clear
packet ownership over pipelined throughput. Packet memory is never modified
while DMA or the GS may still consume it.

The display-buffer switch occurs only after drawing completion and during
vertical blank. This is the minimum reliable swap sequence for the current
single-context, immediate-mode path.

## Runtime diagnostics

Gameplay displays the display and draw indices captured while building the
frame. Because those roles swap at presentation, both values alternate between
zero and one and remain opposite. The diagnostic text itself becomes visible
as part of the completed draw buffer on that presentation.

Existing delta-time movement, 100 ms stall clamp, diagonal normalization,
controller input, application states, and presentation diagnostics remain in
place.

## Minimal scrolling world

Milestone 010 defines a fixed 1600×1200 pixel world and a fixed 640×448 pixel
viewport. The player's existing floating-point position is now a world-space
position, initialized at the world center and clamped so its complete 24×24
sprite stays within X `[0,1576]` and Y `[0,1176]`.

Gameplay owns two scalar viewport offsets. Each update attempts to center the
viewport on the player's center:

```text
viewport_x = player_center_x - 320
viewport_y = player_center_y - 224
```

The results clamp to X `[0,960]` and Y `[0,752]`. Player rendering subtracts
those offsets from the world position before calling the verified textured-quad
path. There is no camera type or spatial framework.

The technical environment consists of a full-screen base color and nine fixed
world-space rectangles: a large central region, one landmark in each cardinal
direction, and four corner markers. Gameplay clips each rectangle against the
current viewport with local min/max calculations, then submits only its visible
screen-space portion through the existing filled-rectangle function. No map,
collision, terrain, asset, scene, or generalized clipping system is present.

Gameplay diagnostics report only player world X/Y and viewport X/Y.

## Left analog-stick movement

Milestone 011 extends the existing primary-controller input state with two
signed left-stick values. PS2SDK supplies `ljoy_h` and `ljoy_v` as unsigned
bytes with nominal center 128. Input polling subtracts 128, producing explicit
horizontal and vertical values in the range `-128–127`. Unstable,
disconnected, unreadable, or non-analog pads always publish zero for both axes.

When the connected pad advertises `PAD_TYPE_DUALSHOCK`, input requests
`PAD_MMODE_DUALSHOCK` with mode locking. The normal per-frame stable-state check
absorbs the asynchronous mode transition. There is no rumble, pressure input,
right-stick behavior, rebinding, or generalized mode manager.

Player movement maps negative centered values through division by 128 and
nonnegative values through division by 127, giving an approximate `-1–1`
range. It then applies a radial deadzone of 0.25. For raw normalized magnitude
`m`, the analog output magnitude is:

```text
0                              when m <= 0.25
clamp((m - 0.25) / 0.75, 0, 1) when m > 0.25
```

The original analog direction is preserved. D-pad axes remain `-1`, `0`, or
`1`; digital and processed analog vectors are added, then the combined vector
is length-clamped to one. Multiplying that final intent by the unchanged
180-pixel-per-second speed and frame delta guarantees that digital, analog,
diagonal, and mixed input cannot exceed the existing maximum speed. World and
viewport calculations are unchanged.

## Static obstacle collision

Milestone 012 keeps collision entirely in the Gameplay state. Four fixed
world-space rectangles are the single source of obstacle position, dimensions,
display color, rendering, and collision. Their `(x, y, width, height)` values
are `(650,450,36,300)`, `(900,520,300,36)`, `(1040,760,36,180)`, and
`(1040,904,220,36)`. The last two form a clear L-shaped corner test. Every
obstacle is at least 36 pixels thick, twice the maximum 18-pixel displacement
allowed by the existing 180-pixel-per-second speed and 100 ms frame-time cap.

The player and obstacles use world-space axis-aligned bounding boxes with
half-open boundaries. For each axis, two ranges overlap only when:

```text
player_start < obstacle_end && player_end > obstacle_start
```

Touching edges are therefore valid and do not penetrate. Gameplay saves the
previous player position, calls the existing player update exactly once, and
then resolves the proposed position. Horizontal movement is resolved first
using the previous Y range. Rightward motion selects the smallest crossed
obstacle-left boundary; leftward motion selects the largest crossed
obstacle-right boundary. This chooses the nearest blocker independently of
array order.

Vertical movement is then resolved using the horizontally resolved X range.
Downward motion selects the smallest crossed obstacle-top boundary; upward
motion selects the largest crossed obstacle-bottom boundary. The resulting
X-then-Y order is deterministic and allows movement along the unobstructed axis
at walls and corners. The player remains flush with a blocking edge and can
move away on the next update.

The player module still performs outer world clamping before this local
resolution. Viewport calculation, rendering conversion, landmark clipping,
input processing, timing, and player drawing are unchanged. There is no
collision module, map representation, dynamic obstacle, physics response, or
generalized collision API.

## Fixed world interaction

Milestone 013 defines one signal terminal at world position `(760,660)` with
dimensions `96×64`. Its single compile-time definition owns the bounds,
inactive RGB color `(0x48,0xd0,0x70)`, and activated RGB color
`(0xf0,0xd0,0x40)`. The terminal is near the player start, outside the initial
player bounds, and separated from every solid obstacle. It is non-solid and is
never included in collision resolution.

The same terminal rectangle drives rendering and proximity. Player/terminal
overlap reuses the existing half-open one-dimensional rule on both axes:

```text
player_start < terminal_end && player_end > terminal_start
```

Touching an edge does not count as overlap. After the unchanged movement and
collision update, Gameplay checks the existing primary input state's newly
pressed mask. Activation occurs only when the player overlaps the terminal and
`pressed_buttons & PAD_CROSS` is nonzero. Held state is not consulted, so Cross
pressed before entry cannot activate the terminal until it is released and
newly pressed while overlapping. Unavailable or disconnected input publishes no
press transition and cannot activate it.

One Gameplay-local integer stores activation. Initialization clears it; a valid
Cross transition sets it once, and it remains latched until Gameplay is next
initialized. Before activation, overlap displays `Press CROSS to activate.`
After activation, the rectangle changes color and
`Signal terminal activated.` remains visible. Rendering uses the existing
viewport-relative clipping path. There is no interaction array, identifier,
callback, event, trigger, persistence, or generalized interaction interface.

## Signal-controlled barrier

Milestone 014 defines one barrier at world position `(900,620)` with dimensions
`36×220`. Its single compile-time definition owns its bounds, bright-red active
RGB color `(0xf0,0x20,0x20)`, and dark-gray disabled RGB color
`(0x38,0x48,0x50)`. The same bounds drive both rendering states and active
collision. The barrier is outside the player start, terminal, and existing
obstacles, and can be approached from both sides.

Barrier state derives directly from the existing terminal flag. When
`signal_terminal_activated == 0`, the barrier renders active and participates
in collision. When the flag is nonzero, it renders disabled and is omitted from
collision. No separate barrier state exists. Gameplay initialization already
clears the terminal flag, which restores the active barrier on every new entry.

The horizontal and vertical resolvers each retain one proposed result while a
small local helper evaluates a collision candidate. Every existing obstacle is
evaluated, and the barrier is evaluated through the same helper only while
active. Right and downward movement retain the smallest crossed boundary;
left and upward movement retain the largest. The resulting nearest blocker is
therefore selected across all active candidates independently of evaluation
order. Collision is still resolved once, horizontal then vertical, with the
same half-open comparisons and wall sliding.

Terminal activation is evaluated after movement and collision in the existing
update. The valid newly pressed Cross sets the terminal flag, so that frame
renders the terminal activated and barrier disabled. Every subsequent update
omits the barrier from both collision axes. There is no toggle, delay, door,
trigger, callback, event, conditional-collision framework, or persistence.

## Fixed readable document

Milestone 015 defines one pale field note at world position `(980,680)` with
dimensions `48×48` and RGB color `(0xe0,0xd4,0xa8)`. The same compile-time
rectangle drives world rendering and player proximity through the existing
half-open overlap rule. It is beyond the signal barrier, outside all solid
geometry, and never participates in collision.

The note's complete narrative is one compile-time ASCII string. While normal
Gameplay is active, overlap displays `Press CROSS to read.` A newly pressed
Cross opens the note; held state is not used, so holding Cross before entry
cannot open it. The note remains in the world and has no read, collected,
inventory, or persistence state.

One Gameplay-local integer, `document_open`, represents the temporary reading
overlay and is cleared during Gameplay initialization. While it is nonzero,
the update still calls `frame_timer_update` so elapsed time cannot accumulate,
then skips player movement, collision, terminal interaction, barrier changes,
document opening, and viewport calculation. A newly pressed Circle clears the
flag; Cross and movement controls have no effect while reading.

The reading render path begins and presents a normal double-buffered frame,
fills the complete 640×448 draw buffer with a dark rectangle, and draws only
the fixed text through the existing debug renderer. Dismissal returns directly
to normal rendering with player, viewport, terminal, and barrier values
unchanged. There is no application state, document framework, text scrolling,
UI system, collection, or save behavior.

## Multiple fixed readable documents

Milestone 016 replaces the single note definition with one local, fixed
three-entry compile-time table. Each entry contains only a 48×48
`WorldRectangle` and a pointer to its unique compile-time ASCII text. The notes
are located at `(980,680)`, `(260,260)`, and `(1320,940)` and all use the proven
pale RGB color `(0xe0,0xd4,0xa8)`. They remain non-solid and use their own
rectangle for both clipped rendering and half-open player overlap.

Normal rendering iterates the fixed entries to draw all three notes. The same
overlap helper identifies the note under the player and displays the existing
prompt. A newly pressed Cross stores only that entry's text pointer in
`open_document_text` and sets the existing `document_open` flag. This transient
selection exists only while the overlay is visible; Circle clears both values.
There is no per-note read flag, collection, ordering, progression, document-list
UI, inventory, or persistence.

The reading overlay, frame-timer refresh, frozen gameplay update, Circle
dismissal, and double-buffered render lifecycle remain single shared code paths.
Only the selected compile-time text differs. All notes remain available and may
be opened repeatedly in any order.

## Fixed objective progression

Milestone 017 adds three compile-time objective strings and one Gameplay-local
three-bit read mask. Normal Gameplay renders the current string unobtrusively;
the reading overlay remains unchanged and continues to display only the selected
document text.

The initial objective is `Activate the relay terminal.` Activating the existing
terminal advances it to `Search beyond the barrier.` Opening a document through
the proven newly pressed Cross path sets that document's bit. Once all three
bits have been set, the objective becomes `Investigate the transmission source.`
The final condition takes precedence over the terminal condition, while repeat
readings simply set an already-set bit and cause no further change.

Gameplay initialization clears the mask, restoring the initial sequence for a
new Gameplay entry. The mask is not saved and is not a collection, inventory,
journal, event, quest, or generalized objective system. Movement, collision,
viewport, terminal, barrier, document overlay, frame timing, rendering, texture,
and double-buffer behavior remain unchanged.

## Fixed transmission-source completion trigger

Milestone 018 added one non-solid `80×80` cyan rectangle at world position
`(1360,1020)`. It was originally named as an extraction rectangle; the current
Gameplay meaning is the separate transmission-source location. The marker is
rendered through the existing clipped world-rectangle path, scrolls with the
unchanged viewport, and does not enter collision resolution.

In current Gameplay, overlap alone never completes the sequence. Once all three
document bits are set and the stationary radio has been inspected, overlap
displays `Press CROSS to inspect`. Newly pressed Cross inside the same cyan
bounds sets the existing transient `completion_overlay_open` flag. Early
physical discovery, held Cross on entry, and Cross before eligibility have no
effect.

While open, the completion overlay follows the proven document-overlay frame
path: frame timing continues to refresh, Gameplay update returns before movement
or world changes, the frame is filled with a dark primitive, and only fixed
completion text is drawn. Newly pressed Circle dismisses it. Player position,
viewport, terminal, barrier, documents, objective progress, collision, texture,
and presentation state remain unchanged. There is no completion persistence,
mission system, ending system, transition, checkpoint, or save behavior.

## Ambient radio

Milestone 019 adds one non-solid `32×32` stationary radio marker at world
position `(1510,1120)`, near the transmission-source area. The fixed magenta marker uses the
existing clipped world-rectangle renderer and unchanged viewport. Player
proximity compares squared center-to-center distance against a fixed 180-pixel
radius, avoiding a square root and introducing no collision behavior.

Five compile-time number-group strings form the complete deterministic sequence.
While the player is within range, one string is shown near the top of normal
Gameplay and `frame_timer.delta_seconds` accumulates locally. Every three
accumulated seconds the index advances, wrapping after the fifth entry. Leaving
range clears elapsed time and restores index zero, so every approach starts with
the same transmission and no wall-clock or random state is involved.

Document and completion overlays retain their exclusive text rendering and
freeze behavior; radio timing therefore pauses while either overlay is active.
Radio proximity does not alter objectives, documents, source completion,
terminal, barrier,
movement, collision, viewport, rendering, texture state, or presentation. There
is no audio playback, radio inventory, event, dialogue, timing subsystem, or
generalized message architecture.

## Radio direction finder

Milestone 020 retains the fixed radio marker, 180-pixel activation radius, five
transmissions, three-second sequence timing, and deterministic reset behavior.
It adds only the previous squared center-distance sample, a validity flag, and
one local status index.

During normal in-range Gameplay, the current squared distance is compared with
the previous frame using a fixed tolerance of 16 squared-pixel units. A decrease
greater than the tolerance selects `SIGNAL STRENGTHENING`; an increase greater
than the tolerance selects `SIGNAL WEAKENING`; otherwise the display reads
`SIGNAL STABLE`. The first in-range sample is stable. Leaving range clears the
sample and resets the status, so re-entry is deterministic.

Squared distance avoids a square root and new math-library dependency. The
status is informational only and is drawn beneath the existing transmission.
Document and completion overlays return before direction updates, preserving
their established freeze behavior. No compass, minimap, HUD framework, radio or
navigation subsystem, event, objective change, or persistent state is added.

## Fixed stationary-radio inspection

Milestone 021 reuses the existing `32×32` stationary-radio rectangle as the sole visual and
half-open interaction boundary. While the player overlaps that rectangle during
normal Gameplay, the existing debug text path displays `Press CROSS to inspect`.
A newly pressed Cross sets one transient `radio_inspection_open` flag; held
Cross state does not open it.

The inspection update path continues to call `frame_timer_update`, then returns
before movement, collision, interactions, source inspection, radio timing, or radio
direction comparison. This preserves all Gameplay and radio values and prevents
elapsed time from accumulating. Newly pressed Circle clears the flag; Cross and
all other Gameplay controls have no effect while the overlay is open.

Rendering follows the established fixed-overlay approach: a normal
double-buffered frame is begun, a dark rectangle covers the 640×448 display,
and only compile-time inspection text is drawn before presentation. Dismissal
returns to the exact player and viewport position with the prior transmission
index, elapsed radio time, distance sample, and signal status unchanged. The
inspection is repeatable and stores no inspected or persistent state. No radio,
interaction, overlay, UI, event, or navigation framework is introduced.

## Radio-confirmed transmission-source inspection

Milestone 022 adds one Gameplay-local `radio_source_confirmed` flag. A valid
newly pressed Cross that opens the existing radio inspection sets the flag. It
is monotonic for the current Gameplay entry: repeat inspections leave it set,
and Gameplay initialization and shutdown clear it with the existing transient
document and overlay state.

Objective selection extends the existing direct conditionals. The inactive
terminal remains highest priority, followed by the incomplete document mask.
Early radio inspection cannot replace either objective. Once the terminal is
active and all three document bits are set, an unconfirmed radio selects
`Inspect the stationary receiver.`; a confirmed radio selects `Investigate the
transmission source.` This makes document reading and radio inspection
order-independent without adding an objective state machine.

The cyan rectangle is now named `transmission_source`. The single source
inspection condition requires the complete document mask, radio confirmation,
half-open player overlap, and newly pressed Cross. Merely entering it has no
effect, and no return journey is required. The existing completion overlay opens
only from that explicit source inspection. No quest, event, interaction,
mission, persistence, or generalized progression system is introduced.

## Completion return to Main Menu

Milestone 023 adds one Gameplay-local `main_menu_requested` boolean and the
narrow `gameplay_state_is_main_menu_requested()` query. Initialization and
shutdown clear the request. Only newly pressed Circle while the existing
completion overlay is open sets it; Circle on document and stationary-radio
overlays retains its established local dismissal behavior, and Circle during
normal Gameplay has no effect.

The existing state manager queries the request immediately after the Gameplay
update. When set, its existing `state_change()` path shuts Gameplay down,
selects Main Menu, and initializes Main Menu before the same application frame
renders. No completed Gameplay frame resumes after the request. Holding Circle
cannot generate another edge, and the Gameplay shutdown clears the request.

START follows the unchanged Main Menu request path back to Gameplay. The normal
Gameplay initialization resets player and viewport position, terminal, barrier,
documents, receiver confirmation and overlay, source overlap, completion
overlay, radio timing and direction samples, and the return request. No new
application state, transition queue, callback, event bus, scene stack,
persistence, or generalized transition infrastructure is introduced.

## Gameplay pause overlay

Milestone 024 adds one Gameplay-local `gameplay_paused` boolean. Initialization
and shutdown explicitly clear it. After the document, completion, and stationary
receiver overlay branches have handled input, newly pressed START toggles pause
during ordinary Gameplay. Those existing overlays therefore retain exclusive
input ownership and cannot open or toggle pause.

Every paused update still calls `frame_timer_update()`, then handles only newly
pressed START and returns before player movement, collision, viewport changes,
terminal and document interactions, receiver and source interactions, objective
progression, radio timing, transmission advancement, distance sampling, or
signal comparison. Refreshing the existing timer baseline prevents elapsed
pause time from becoming a resume-frame delta.

Rendering continues through the normal world path. After the textured player is
drawn, a fixed dark central panel and compile-time pause text are submitted before
the normal frame presentation. The surrounding scene remains visible, and no
texture, video, framebuffer, state-manager, input, timing, overlay, or UI
architecture is added.

Renderer experiments intended to address the pre-existing text flicker caused
startup and black-screen regressions and were removed before the final
correction. The externally verified Milestone 023 renderer was the baseline.
Temporary red Splash and green Main Menu markers were also removed.

Packet inspection confirms that `init_scr` independently configures a
framebuffer-zero drawing environment. More importantly, each `scr_putchar`
packet performs a host-to-local glyph transfer whose `BITBLTBUF` destination
base is fixed at VRAM word zero with a 640-pixel destination width. The transfer
therefore ignores `draw_buffer_index`, even if `FRAME_1` is restored afterward.
World primitives were drawn into alternating framebuffer 0 and framebuffer 1,
while text was redrawn only into framebuffer 0. Display alternation made that
text disappear on every framebuffer-1 frame.

The final correction retains `init_scr` in the known-working startup sequence
but does not call `scr_vprintf` or `scr_putchar`. `video_draw_text` formats into
a fixed buffer, reads the same linked MSX bitmap, and submits only filled
horizontal glyph runs through libdraw. Those primitives inherit the currently
selected framebuffer, so text and world geometry share the draw target before
the normal GS completion, vertical blank, and swap. No texture, font VRAM,
runtime file, BIOS font, blending state, or additional draw context is added.

## Pause return to Main Menu

Milestone 025 extends only the existing `gameplay_paused` input branch. Newly
pressed Circle sets the established `main_menu_requested` flag and returns from
the Gameplay update without resuming simulation. The unchanged state manager
observes that request immediately after the update, shuts Gameplay down through
its normal lifecycle, and initializes Main Menu before rendering the frame.

Newly pressed START still clears `gameplay_paused` and resumes Gameplay. Circle
is checked first while paused, so a simultaneous newly pressed Circle and START
requests Main Menu rather than briefly resuming. Held buttons do not repeat
because both actions use `pressed_buttons`. Document, completion, and receiver
overlay branches remain ahead of pause handling and retain their existing Circle
semantics.

The pause overlay now shows both controls. No transition interface, reset logic,
confirmation dialog, retained progress, save behavior, application state, or
menu framework is added. Gameplay initialization and shutdown continue clearing
both `main_menu_requested` and `gameplay_paused`, so START from Main Menu enters
the same fully reset Gameplay session established by Milestone 023.

## Scope

The video module exposes only frame begin, filled rectangle, the single test
sprite, diagnostic text, frame present, small diagnostic queries, and shutdown.
It has no texture handles, runtime asset loading, registry, cache, sprite batch,
render queue, materials, animation, cameras, scene graph, or generalized engine.

## Dependencies

- Native EE cross-compiler from `ps2_arm64_toolchain` Version 1.0
- PS2SDK target headers and startup files
- PS2SDK `libpad`, `libdebug`, `libdraw`, `libpacket`, `libgraph`, `libdma`, and
  C library

The toolchain remains an external, read-only dependency. Tyra and gsKit are
not dependencies.
