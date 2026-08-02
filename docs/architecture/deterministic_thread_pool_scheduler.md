# Deterministic bounded thread-pool scheduler

## Baseline audit

This milestone started from commit `c4e6839c0a656db14989faa0e937358ce877fe00`
on branch `main` with a clean worktree. The installed scheduler was
`scheduler.serial.row-major`: its vtable accepted an immutable job, a mutable
scope-owned field, a cancellation token, and an optional progress callback. It
walked the field in row-major order and delegated each point to the conventional
compute vtable. Partial progress was represented by `completed_rows`; a field
became consumable only after the scheduler returned success.

There was no checked-in sealed work-unit representation. The authoritative
pipeline allocated field and pixel storage from a short system-memory scope,
computed the complete field, ran the bounded analyzer chain sequentially,
rasterized, encoded, and released the scope. Transactional artifact rendering
began a caller-selected sink before that pipeline, committed only after a fully
successful encode, and aborted computation or analysis failures. Registry
assembly selected scheduler vtables by exact module ID. Runtime and artifact
manifests recorded mathematical and artifact identities but no scheduler
execution metadata.

Cancellation was a plain boolean checked inside conventional point iteration
and within analyzers. The analyzer chain used at most eight analyzers and eight
records in declaration order. Registry capacity was 32 modules. Jobs were
limited to 4096 by 4096 samples. The active memory backend was system scoped;
CDC refinement and Ouro memory were registered as unavailable. Frozen 32 by 24
Mandelbrot and Julia identities were:

| Formula | Field | Pixels | BMP | Bytes |
|---|---|---|---|---:|
| Mandelbrot | `99ec88c2a0f8bac3` | `4866aacc38290b5f` | `fb1a83bd5ca28e5f` | 2358 |
| Julia (-0.8, 0.156) | `0fb4458e08bad6e1` | `b272f08b0bbdca2b` | `4d4aa95bd137ec87` | 2358 |

`CDC.pdf` remained authoritative with SHA-256
`5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

## Socket, ownership, and lifecycle

`fractal.scheduler.thread-pool.v1` is a second scheduler vtable. The original
`fractal_scheduler_serial` object, implementation, and compatibility ID remain
unchanged. `fractal.scheduler.serial.v1` is a registry alias that delegates to
that exact reference implementation. Registry selection carries the requested
worker count into fixed runtime scheduler options; thread-pool assembly rejects
zero or more than 16 workers before computation.

The caller owns the immutable job, cancellation token, output field, module
assembly, and artifact sink. The runtime owns field and pixel allocations for
the render scope. A thread-pool invocation owns fixed stack arrays for at most
16 sealed assignments, 16 worker contexts, and 16 native thread handles. It has
no queue and performs no `malloc`, scoped allocation, or other language-level
heap allocation. Native thread creation necessarily consumes opaque host OS
resources. Each worker borrows its job and module pointers and mutably borrows
only its prevalidated field rows. All threads join before the invocation
returns, so no worker retains borrowed state.

The pipeline order is unchanged:

1. compute the complete field and join every assignment;
2. run the bounded analyzer chain sequentially in declaration order;
3. rasterize the immutable field;
4. encode through the selected sink;
5. commit publication.

Analysis is not parallelized and does not influence assignment construction.

## Sealed work units and decomposition

The narrowly necessary backward-compatible contract addition is
`fractal_sealed_work_unit_v1`. It contains only canonical dimensions, worker
count, stable sequence, contiguous row and sample bounds, ABI version, and an
identity seal. It contains no mutable output pointer. Recomputing the identity
after changing a field does not make an assignment valid: validation also
recomputes the one exact decomposition and checks ordered coverage.

For height `H`, requested worker count `W`, and sequence `i` where
`0 <= i < W`, assignment `i` owns the half-open row interval

`[ floor(i * H / W), floor((i + 1) * H / W) )`.

Its sample interval is the row interval multiplied by width. This rule depends
only on immutable dimensions and worker count. Sequence identifiers are assigned
before native threads exist. When `H < W`, empty intervals are retained as
stable assignments; requested worker count, effective worker count, and
assignment count therefore all equal `W` after successful admission.

Validation rejects a changed seal, wrong ABI/sequence/count/dimensions, the
wrong formula-derived row bounds, gaps, overlap, out-of-range rows or samples,
or insufficient field stride before the launch gate opens. Workers derive row
pointers from field stride and can write only within their interval.

Each unit identity is FNV-1a-64 over canonical semantic text containing the
work-unit ID and every work-unit field except the identity itself. The sealed
work-unit-set identity is FNV-1a-64 over ordered little-endian unit identities.
These are reproducibility identities, not cryptographic hashes.

## Determinism, cancellation, and failure

Workers invoke the same region-level scalar computation module as the serial
reference. Field layout, whole-field
analysis, typed-record order, raster traversal, BMP serialization, checksums,
artifact bytes, and committed identity therefore depend on stable row-major
locations, never thread completion order. Progress callbacks are replayed in
row order only after a successful join. Runtime and artifact manifests add only
concise scheduler metadata: scheduler ID/version, work-unit contract and set
identity, requested/effective worker count, assignment count, decomposition
ID/version, execution result/status, and publication status.

Cancellation is cooperative, not instantaneous. The thread-pool checks the
atomic token at four defined boundaries:

- before scheduler admission;
- after decomposition and ownership validation, before worker launch;
- before an assignment starts and before each row work unit;
- after all workers join and before analysis, rasterization, encoding, or
  publication.

Once a row starts, it finishes without another thread-pool cancellation check.
The sealed assignment selects row-boundary cancellation, so the shared scalar
computation module does not pass the token into point iteration on this path.
This keeps the documented row boundary exact. On cancellation or failure, the
field is incomplete, analysis and encoding do not run, and an active artifact
sink is aborted exactly once. No partial artifact is committed.

Every worker stores its result in its stable assignment slot. After joining,
the scheduler scans slots in increasing sequence order and returns the first
non-success result. Thus simultaneous failures select the lowest assignment
sequence rather than the fastest reporter. Thread-creation failure keeps the
launch gate closed, releases already-created workers without computation, joins
them, and fails the invocation.

## Limitations

The maximum worker count is fixed at 16. There is no work stealing, adaptive
sizing, retry, priority, NUMA policy, GPU execution, analyzer parallelism,
benchmark claim, scheduling feedback, CDC refinement, or Ouro memory. Native
thread creation is per invocation rather than a persistent service pool. The
serial compatibility ID remains installed, so catalogs contain both the legacy
ID and the exact v1 reference ID.

## Recommended next milestone

Extract the numeric socket from the scalar computation implementation while
preserving the computation contract and every frozen output.
