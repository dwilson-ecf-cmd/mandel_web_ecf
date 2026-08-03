# Socketable scalar computation contract

## Audit baseline

This milestone started from commit
`ed41df6d6bb0d18b5a23b8863d911675575a6c93` on the clean branch
`codex/deterministic-thread-pool-scheduler`. The authoritative catalog had 21
modules and identity `9e4be9abf4f83e79`. `FRACTAL_MODULE_COMPUTE` existed, but
its only installed implementation, `compute.conventional.scalar-c`, exposed a
point function rather than a bounded field-region contract. The serial
scheduler owned the complete coordinate loops. The thread-pool scheduler owned
a second copy of those loops and called the same point function with a different
cancellation boundary. Thus formulas and arithmetic were socketable, while
field generation was still scheduler knowledge.

The separate `server/computation` point-backend API predates the socketable
runtime and remains an established research/compatibility surface used by CDC
fixtures and the generic render manifest. It is not in the runtime scheduler
path and is unchanged. The new computation socket is the `fractal_core` module
contract used by both installed schedulers; it does not silently redirect the
older backend API.

The audited field is caller-owned `fractal_point_result_compact` storage with a
versioned `fractal_field_descriptor`. Runtime scope owns the allocation, the
scheduler mutably borrows it, analyzers borrow the completed immutable view,
and rasterization and encoding occur only after successful analysis.
Transactional artifact sinks begin before rendering, commit only after a full
successful encode, and abort on computation or analysis failure. Analyzer-chain
execution is sequential and deterministic. Concurrent scheduler failures were
already selected by the lowest sealed assignment sequence after every worker
joined.

The starting 32 by 24, 64-step frozen outputs were:

| Formula | Field | Pixels | BMP artifact | Bytes |
|---|---|---|---|---:|
| Mandelbrot | `99ec88c2a0f8bac3` | `4866aacc38290b5f` | `fb1a83bd5ca28e5f` | 2358 |
| Julia (-0.8, 0.156) | `0fb4458e08bad6e1` | `b272f08b0bbdca2b` | `4d4aa95bd137ec87` | 2358 |

`CDC.pdf` started and remains at SHA-256
`5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

## Contract and immutable assignment

`FRACTAL_MODULE_COMPUTE` is the stable computation-module kind used by the
repository's module registry. Its authoritative field implementation is
`fractal.compute.scalar.v1`, module version 1, implementing computation contract
version 1. The descriptor advertises point-scalar formula compatibility,
iteration/classification output, caller-owned destination storage, contiguous
row assignments, and cooperative cancellation.

`fractal_computation_problem_v1` is an immutable borrowed problem view. It
contains ABI and contract versions, selected formula and numeric vtables,
typed formula parameters, iteration budget, source-view mapping, field
descriptor, and a deterministic identity over those values plus the selected
computation module. `fractal_computation_request_v1` adds the caller-owned
mutable destination, cancellation token, and one sealed assignment.
`fractal_computation_result_v1` reports the assignment identity and sequence,
status, result, and completed row/sample counters without allocating storage.

The extended `fractal_sealed_work_unit_v1` is the immutable compute assignment.
It snapshots the computation, numeric, and formula identities and versions; the
complete computation-problem identity; field format; dimensions; row and sample
bounds; worker count; deterministic sequence; cancellation boundary; ABI and
contract versions; and its own seal. The assignment plus computation problem is
sufficient for a computation module to validate and execute a region without
consulting scheduler state. Set validation remains scheduler-owned and rejects
wrong decomposition, gaps, overlaps, malformed seals, and out-of-range regions
before worker launch.

## Ownership boundaries

The scheduler owns only decomposition, worker lifecycle, result slots, ordered
failure selection, and progress replay. It does not map pixels to complex
coordinates, identify Mandelbrot or Julia, apply the quadratic recurrence,
classify escape, or invoke scalar arithmetic. The computation module owns those
field-generation operations and delegates arithmetic through the selected
numeric vtable and recurrence/state behavior through the selected formula
vtable.

The computation module borrows the problem, assignment, cancellation token,
and destination. It writes only `row_begin <= y < row_end`, respects descriptor
stride, and never allocates or publishes a complete field or artifact. Runtime
scope retains field ownership. No global mutable production state or new heap
allocation is introduced. Analyzers still receive only the successfully
completed immutable field and execute sequentially before rasterization.
Artifact publication remains transactional and outside computation.

## Scalar reference behavior

`fractal.compute.scalar.v1` wraps the exact former conventional point behavior:
pixel-center mapping, positive imaginary upward, Mandelbrot and Julia formula
dispatch, binary64 operations, quadratic recurrence ordering, escape comparison,
iteration counts, unresolved budget exhaustion, and compact element layout.
The established `fractal_compute_conventional.point` symbol and
`compute.conventional.scalar-c` ID remain installed as a documented low-level
compatibility surface. The compatibility vtable also implements the region
contract, but new registry-driven assemblies select `fractal.compute.scalar.v1`
explicitly.

Serial execution creates one complete point-cancellable assignment. The
thread-pool retains its fixed capacity of 16, exact contiguous-row formula,
stable empty assignments when height is smaller than worker count, and
row-boundary cancellation. Cancellation boundary is sealed in each assignment,
so the shared scalar module preserves both established scheduler behaviors
without either scheduler containing point logic.

## Cancellation, failure, and publication

Cancellation is checked before scheduler admission, before worker launch,
before an assignment, at every row, and at the assignment's sealed finer
boundary. Serial assignments preserve point-iteration cancellation; thread-pool
assignments preserve row-boundary cancellation. A cancelled or failed
assignment never makes the field complete. Analysis, rasterization, and encoding
do not run after incomplete field generation.

Each thread writes its result to its stable sequence slot. After every thread
joins, the scheduler scans slots in increasing sequence order and returns the
first non-success result. Failure selection therefore does not depend on host
timing. Thread-creation failure keeps the launch gate closed. Transactional
artifact rendering aborts the active sink exactly once and cannot commit a
partial aggregate result.

The scalar module validates the complete problem, module kinds and versions,
capabilities, formula parameters, numeric compatibility, field format, mutable
view extent, assignment identity, module snapshots, and destination bounds
before its first write. Runtime assembly rejects missing, unavailable, wrong-kind,
ABI-incompatible, or capability-incompatible computation selections before
scheduling.

## Runtime assembly, manifests, identity, and replay

Registry selection continues to use the source-compatible `compute` selection
field, whose documented meaning is now the computation-module ID. Runtime,
socketable render, artifact, and generic render manifests carry concise generic
computation metadata: module ID/version, computation or assignment identity,
assignment count, and execution status. They do not embed assignment arrays or
duplicate analyzer records.

The installed registry grows from 21 to 22 modules and intentionally changes
from `9e4be9abf4f83e79` to `3c99a301df6359d7`. This is solely registry
composition and metadata: `fractal.compute.scalar.v1` is added and the retained
conventional compatibility descriptor advertises the region contract it now
implements. Formula, numeric, analyzer, raster, encoder, scheduler, and artifact
module identities are otherwise unchanged.

Work-unit and assignment identities intentionally change because their v1 seal
now includes the problem identity, computation/numeric/formula module snapshots,
field format, contract version, and cancellation boundary. Current frozen
Mandelbrot assignment-set identities are `4258a35a8206a3d8` for serial and
`afec0821f63abdbb` for five-worker thread-pool execution; Julia identities are
`53b154689c9ca381` for serial and `30dd9c1c387d9962` for four workers. The
corresponding computation identities are `5e64dd353daa28be` and
`917e7c5c82198c29`. These intentional metadata identities support mismatch
rejection and replay verification; they do not change field bytes.

All frozen field, pixel, BMP, analyzer-record, and analyzer-chain identities are
unchanged. Scheduler module IDs and versions are unchanged. Runtime and artifact
manifest bytes intentionally change only because generic computation metadata is
now serialized. No artifact payload identity changes.

## Current limitations

This milestone does not implement a new numeric backend, arbitrary precision,
SIMD or GPU execution, adaptive tiling, dynamic work stealing, perturbation
methods, distance estimation, orbit storage, new formulas, CDC-driven
refinement, Ouro memory integration, or performance claims. The scalar module
supports only the existing iteration/classification field and contiguous row
assignments. Native threads are still created per invocation, and registry
loading remains static.

## Successor architecture

The completed numeric extraction and current forward boundary are recorded in
`binary64_numeric_socket.md`.
