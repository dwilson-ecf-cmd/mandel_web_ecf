# Socketable runtime audit

This audit was completed before implementation of the socketable runtime.

## Existing seams

The portable C11 spine already separated result codes, render specifications,
manifests, progress/jobs, point computation, renderer lifecycle, and scoped memory.
`fractal_computation` has conventional and unavailable CDC identities; the renderer
has legacy C++ and transitional unavailable CDC identities; system memory is usable
and Ouro explicitly returns `NOT_IMPLEMENTED`.  CMake builds these without Python
or executing the checked-in Android binaries.

## Embedded Mandelbrot assumptions

* `fractal_render_spec.fractal_type`, bailout radius, iteration count, view, palette,
  tiling, animation, and backend-adjacent precision are one presentation-era record.
* `fractal_computation_problem` has a one-value formula enum, binary64 fields named
  `c` and `z`, and conventional compute contains the quadratic recurrence and a
  `mandelbrot-binary64` identity.
* CDC certificates intentionally name Mandelbrot and binary64; these are historical
  research evidence and must not be generalized or changed.
* The legacy renderer combines lifecycle, scheduling-shaped frame/tile calls,
  computation, and pixel production. Legacy sources and artifacts also use
  Mandelbrot-specific names.
* Existing manifests identify computation, renderer and memory, but not formula,
  numeric, scheduler, raster, encoder, telemetry, refinement, or platform modules.
* Tests predominantly exercise the Mandelbrot oracle and historical preservation.

## Migration decisions

The existing `fractal_render_spec`, `fractal_computation_*`, and
`fractal_renderer_*` APIs remain compatibility interfaces. They are not deleted or
silently reinterpreted. A new, statically assembled runtime uses independent module
descriptors, a generic formula state machine, a numeric socket, compact field and
pixel buffers, and separate scheduling/raster/encoding. The old computation API
remains the CDC research boundary and the old renderer remains the legacy adapter
boundary.

Mutable module state is unnecessary in the first stateless modules. Runtime assembly
therefore retains only caller-owned immutable vtable pointers plus the existing
caller-owned memory backend. The initial platform socket is an identity contract and
the refinement socket is explicitly `none`; neither claims speculative operations.

## Ownership shortcomings addressed

Legacy renderer calls accept raw buffers but do not describe a complete intermediate
field or pixel-buffer ownership model. The new path allocates field and pixel storage
from an explicit memory scope, releases both when that scope ends, and writes encoded
bytes through a caller-owned bounded sink. Formula parameter payloads are tagged by
formula module ID and exact byte size rather than being unchecked blobs.
