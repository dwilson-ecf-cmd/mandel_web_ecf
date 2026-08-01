# Analysis socket audit

Starting commit: `1f4d3a6` (`Merge pull request #10 from
dwilson-ecf-cmd/codex/establish-runtime-registry-and-artifact-sinks`).  The
checkout had a clean `work` branch, no configured remote, and contained the
authoritative registry, the `ARTIFACT_SINK` kind, bounded-memory and host-file
sinks, and registry-driven runtime assembly.

## Existing handoff

The serial scheduler is the field producer.  It writes a row-major allocation
of `fractal_point_result_compact`, advances `completed_rows` after each row,
and sets `complete` only after every row succeeds.  Each compact sample retains
only a 32-bit conventional iteration count and an escaped, bounded,
unresolved, cancelled, or failed classification.  Formula state, orbit points,
coordinates, intermediate arithmetic, and per-step evidence are lost before
rasterization.

The native rasterizer is the only direct field consumer in the authoritative
render path.  It requires `complete`, assumes tightly packed row-major samples
(`width`, rather than `stride`, indexes samples), and reads but does not mutate
the field.  The runtime also reads the scheduler allocation to calculate the
historical field checksum.  No partial field is rasterizable, although the
`completed_rows` member represents scheduler progress after a failure or
cancellation.

## Ownership and lifetime

The runtime opens one short memory scope, allocates field and pixel storage,
and owns both allocations until encoding finishes.  The scheduler borrows a
mutable field, while raster and checksum code borrow it immutably.  The scope
is ended exactly once and output pointers are cleared; only checksums and byte
counts survive.  There is no hidden ownership transfer at the scheduler/raster
boundary.

Cancellation is polled by point computation.  A cancellation result stops the
scheduler and therefore prevents rasterization and encoding.  Telemetry is a
structured vtable/event seam but the render path currently supplies no
progress callback and the installed backend is a no-op.  Checksums are owned
by runtime orchestration, not by scheduler, raster, encoder, or sink.

## Registry, assembly, compatibility, and manifests

The static registry sorts by module-kind enum then module ID and hashes kind,
ID, ABI/version, availability, and capabilities.  Assembly resolves each
selection by kind plus ID and rejects unavailable implementations.  Runtime
validation checks module ABI/kind and adjacent capability compatibility before
the job allocates or computes.  CDC refinement and Ouro memory are registered
but unavailable and do not appear in render manifests unless selected (and
selection fails).

The runtime manifest records one `field_checksum`, plus pixel and artifact
checksums and selected module identities.  The artifact manifest repeats the
single field checksum and adds registry and sink metadata.  The checked-in
goldens are Mandelbrot `99ec88c2a0f8bac3 / 4866aacc38290b5f /
fb1a83bd5ca28e5f` and Julia `0fb4458e08bad6e1 / b272f08b0bbdca2b /
4d4aa95bd137ec87` (field / pixel / BMP artifact).

## Safe insertion point

Analysis can be inserted after successful scheduler completion and source
checksum calculation, but before rasterization.  Immutable descriptor/view
types can expose the allocation without transferring it.  A pass-through
analyzer can return an aliasing borrowed view; a copying mode can use storage
owned by the same runtime memory scope.  Raster should consume the final view,
never switch on analyzer identity, and analysis failure or cancellation must
prevent rasterization.  The historical checksum maps explicitly to the source
field checksum; a separate analyzed checksum describes the final view.

The compact representation is sufficient for a future escape/classification
summary (counts and conventional-iteration aggregates/histogram), but not for
orbit, distance, periodicity, or proof-oriented analysis without computation
changes.  Future analyzers must not infer absent mathematical evidence.

## Milestone outcome

The seam was implemented with zero/one bounded composition. Source and analyzed identities are now distinct while `field_checksum` remains an explicit compatibility alias for source. The pass-through route preserves the existing Mandelbrot and Julia golden triples and memory/file parity. The registry identity intentionally changed because the authoritative installed catalog gained an analyzer.
