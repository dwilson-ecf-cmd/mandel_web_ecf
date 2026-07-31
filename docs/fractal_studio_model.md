# Neutral Fractal Studio model

The dependency-free C11 records in `shared/include/fractal/` formalize data shape, not
transport, database, renderer, or Ouro internals. They use bounded inline storage and
fixed-width counters, so basic initialization and validation perform no allocation.
Ownership remains with the caller; application code should publish snapshots rather
than share concurrently mutated structures.

## RenderSpec

`fractal_render_spec` identifies the fractal and palette; decimal center coordinates; a
positive scale (the width of the mathematical view, with zoom representable by its
reciprocal/conversion at an adapter); positive image width and height; positive
maximum iterations and bailout radius; numerical precision; positive tile width and
height no larger than the image; and a positive supersampling factor. Supported
precision identifiers are `float32`, `float64`, `long-double`, and
`arbitrary-decimal`.

Optional animation data carries positive frame count, frames per second, and a zoom
multiplier per frame. More animation paths should be versioned additions rather than
overloaded strings. Coordinates and other decimals use validated, bounded canonical
decimal text; this preserves their digits without claiming arbitrary-precision math.
Accepted text has no exponent, redundant leading zero, trailing fractional zero,
positive sign, or negative zero. Consequently equivalent valid values have one byte
representation. Canonical JSON uses fixed key order, decimal strings, compact
separators, and explicit escaping. Serialization writes to a caller-provided buffer,
reports the required byte length, and returns `FRACTAL_ERROR_BUFFER_TOO_SMALL` rather
than allocating. It does not yet define a manifest hash/version.

## Application records

* **`fractal_render_job`** has a unique job ID, copied spec, submission time, state, progress
  snapshot, cancellation-request flag, and output destination. State vocabulary and
  legal transitions remain a scheduler design decision.
* **`fractal_render_progress`** has total/completed frame and tile counts, active workers,
  elapsed duration, an `estimate_available` flag and evidence-based completion time,
  retries, and failures. A false flag means there is not enough evidence to estimate.
* **`fractal_worker_status`** has worker ID, health, optional assignment, completed-work count,
  heartbeat time, and optional recent error.
* **`fractal_render_artifact`** has artifact and job IDs, a file path/storage key, media type,
  dimensions, checksum, creation time, and optional associated manifest key.
* **`fractal_render_failure`** has a stable code, human summary, subsystem, recoverability,
  underlying diagnostic key/value data, and optional job/frame/tile/worker context.

Identifiers, timestamps, checksum algorithms, media types, state enums, coordinate
precision semantics, and protocol versioning need author/team agreement before a
wire format is frozen.
