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

`video_begin_frame` clears only the active draw buffer. Rectangle and clear
operations reuse one 64-qword packet and complete before that packet is reused.
PS2SDK debug text draws through the currently selected GS drawing context, so
it lands in the same hidden buffer as the primitives.

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

Gameplay diagnostics report only player world X/Y and viewport X/Y. The known
libdebug framebuffer-zero flicker remains outside this milestone.

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
