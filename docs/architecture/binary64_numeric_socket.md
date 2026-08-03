# Socketable Binary64 numeric contract

## Audit baseline

This milestone started from exact commit
`d9c5353f4497550b081c140a554149b8c2b66d7e` on the clean branch
`codex/architecture/computation-socket`. `git status --short --branch` reported
only the branch/tracking line, and no merge, rebase, cherry-pick, revert,
submodule, staged, unstaged, or untracked state was present.

The current computation contract was `fractal.compute.scalar.v1`, computation
ABI 1, contract 1. Its immutable `fractal_computation_problem_v1` borrowed the
selected formula and numeric vtables, typed formula parameters, iteration
budget, Binary64 view values, and a field descriptor. A request added one
sealed assignment, a caller-owned mutable field view, and a cancellation token;
the result reported assignment identity/sequence, completed rows/samples,
status, and error. Both installed v1 schedulers invoked this same computation
contract.

The scheduler contract borrowed a runtime assembly, immutable job, mutable
field, cancellation token, and optional progress callback. The exact serial
implementation `fractal.scheduler.serial.v1` delegated to the retained
`scheduler.serial.row-major` reference and produced one point-cancellable
whole-field assignment. `fractal.scheduler.thread-pool.v1` produced a fixed,
ordered set of at most 16 contiguous-row assignments, launched one native
worker per assignment, joined every worker, selected failure by lowest
sequence, and preserved row-boundary cancellation. Neither scheduler owned the
quadratic recurrence after the computation-socket milestone.

The sealed work unit already captured its seal, problem identity, computation,
numeric, and formula module identities and versions, computation ABI/contract,
field format, cancellation boundary, sequence/worker count, dimensions, and
row/sample intervals. Decomposition used
`[floor(i * height / workers), floor((i + 1) * height / workers))`, retained
stable empty assignments, and validated ordered full coverage before launch.

Mandelbrot and Julia followed the same path: scheduler assignment, scalar
coordinate mapping, computation point loop, formula initialization/step/
classification, field write, analyzer chain, rasterizer, BMP encoder, and
transactional artifact sink. Mandelbrot initialized `z` to zero and `c` to the
sample coordinate; Julia initialized `z` to the coordinate and `c` to the
selected constant. Escape used strict squared-magnitude-greater-than-squared-
radius comparison. Budget exhaustion remained unresolved.

The pre-extraction numeric vtable and its `numeric.binary64` implementation
were embedded in `runtime/src/socketable_runtime.c`. They exposed only
double conversion, real add/subtract/multiply, finiteness, and bit
serialization. That same file still performed raw Binary64 coordinate add,
subtract, multiply, divide, integer conversion, quadratic component arithmetic,
radius squaring, squared magnitude, and the bailout comparison. The numeric
module therefore did not yet own all scalar arithmetic.

The authoritative installed registry had 22 modules and identity
`3c99a301df6359d7`: two formulas, the single `numeric.binary64` backend, two
computations, two refinements, three scheduler identities (compatibility,
serial v1, and thread-pool v1), raster, encoder, two memory modules, telemetry,
platform, two artifact sinks, and four analyzers. The old numeric module
identity was `ca9dd1f625e50d6d`.

The baseline configured with Ninja, completed a clean 39-step build, passed all
5 CTest targets, and passed `fractal_core_tests`,
`socketable_runtime_tests`, `thread_pool_scheduler_tests`, and
`computation_socket_tests` directly. PowerShell required
`C:\msys64\mingw64\bin` on `PATH` so compiler subprocesses could load the
MinGW runtime; after that environment correction the requested commands passed.
`git diff --check` passed and the worktree remained clean.

The frozen baseline was:

| Formula | Field | Pixels | BMP artifact | Bytes | Summary analyzer |
|---|---|---|---|---:|---|
| Mandelbrot | `99ec88c2a0f8bac3` | `4866aacc38290b5f` | `fb1a83bd5ca28e5f` | 2358 | `2820bd77cffceeef` |
| Julia (-0.8, 0.156) | `0fb4458e08bad6e1` | `b272f08b0bbdca2b` | `4d4aa95bd137ec87` | 2358 | `5eabac4db10ecc82` |

The three-analyzer chain identity was `0116ac94c4b64d99`; its independent
four-sample record identities were `901b1590589ec748`, `c63bf12147913290`, and
`c756206cdfd45a31`. Mandelbrot and Julia spatial-record identities were
`0229caad5ec52cc8` and `854c303315248bc8`. Module identities were
`25134ac865c30775` (Mandelbrot), `8d975e1d77a80d4a` (Julia),
`0f4e80b9ea57acc9` (scalar computation), `063fcb1076049dd8` (serial v1), and
`d55b1ea157b8ae6a` (thread-pool v1).

Baseline computation-problem identities were `5e64dd353daa28be` for the
32-by-24 Mandelbrot fixture and `917e7c5c82198c29` for Julia. Serial/five-worker
Mandelbrot assignment-set identities were `4258a35a8206a3d8` and
`afec0821f63abdbb`; serial/four-worker Julia identities were
`53b154689c9ca381` and `30dd9c1c387d9962`. The padded 4-by-4 computation fixture
had problem identity `19eb35bcead3ca1c`, assignment identities
`3981f76e91eafa2e` and `2f6022387a83e9f4`, and set identity
`f5b2ed79a24b438e`. `CDC.pdf` SHA-256 was
`5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

## Numeric ABI and ownership

`fractal.numeric.binary64.v1` is the sole installed numeric implementation.
`fractal_numeric_vtable` has numeric ABI 1, contract 1, explicit capability
flags, precision/storage metadata, and operations for:

- canonical zero, one, two, and one-half;
- Binary64 and unsigned-integer conversion plus canonical real serialization;
- real add, subtract, multiply, divide, comparison, equality, and finiteness;
- complex construction, add, subtract, multiply, and square;
- squared magnitude and strict bailout comparison.

The interface describes arithmetic semantics only. It contains no Mandelbrot,
Julia, field, scheduler, pixel, analyzer, encoder, or publication knowledge.
`fractal_numeric_validate_v1` verifies descriptor kind/module ABI,
availability, numeric ABI/contract, capabilities, storage metadata, and every
required operation. Runtime validation applies the union of formula and
computation requirements before scheduler execution. Unknown and unavailable
registry selections return `NOT_IMPLEMENTED`; malformed ABI or unsupported
capabilities return `INVALID_SPEC` before computation begins.

The Binary64 backend uses the prior sequential C `double` operations without
fusion, reordering, approximation, or optimization. Compile-time checks require
8-byte, radix-2, 53-bit-mantissa, exponent-1024 `double`. Every recurrence and
coordinate operation retains the former operation ordering, finite-result
handling, strict `>` bailout, iteration count, unresolved behavior, and
canonical raw-bit serialization.

## Computation and scheduler boundary

The ownership path is:

```text
sealed work unit
        |
scheduler
        |
computation socket
        |
numeric socket
        |
formula socket (future)
```

The scheduler still owns deterministic decomposition, worker lifecycle,
ordered failure selection, and progress replay. The scalar computation module
still owns iteration order, recurrence sequencing, coordinate-operation order,
assignment boundaries, destination writes, and cancellation checks. It performs
only loop/index/address arithmetic directly; every real or complex operation is
invoked through the selected numeric vtable. The numeric backend owns arithmetic
only. Formula behavior remains co-located with scalar computation for the next
extraction milestone.

Serial point-iteration cancellation and thread-pool row-boundary cancellation
are unchanged. A failed or cancelled computation does not mark the field
complete; analyzers, rasterization, and encoding remain suppressed. An active
transactional artifact sink aborts and cannot publish partial bytes. Scheduler
failure ordering is unchanged.

## Work-unit, manifest, and replay identity

Each sealed work unit now additionally binds the numeric ABI version and exact
numeric capability flags. Its existing numeric identity remains the numeric
module identity. The computation-problem identity also includes numeric module
ID/version, ABI, capabilities, and numeric execution identity. Replaying a
sealed assignment therefore requires the same validated numeric contract;
changing only an ABI or capability bit changes the seal and fails validation.

Runtime, artifact, and generic render manifests carry only concise numeric
metadata: numeric module ID, module version, ABI version, execution identity,
and compatibility status. They do not duplicate operation tables or Binary64
implementation details. Manifest bytes intentionally change; BMP payload bytes
do not.

The authoritative registry remains at 22 modules but changes identity to
`d73d9e545afa7735` because `numeric.binary64` is replaced by
`fractal.numeric.binary64.v1`. The new numeric module identity is
`f554b96377ce0dbb`, and its ABI/capability execution identity is
`2f300bf0f7ea5244`. Formula, computation, scheduler, analyzer, raster, encoder,
and artifact-sink module identities are unchanged.

The new frozen computation and assignment identities are:

| Fixture | Computation problem | Serial assignment set | Thread-pool assignment set |
|---|---|---|---|
| Mandelbrot 32 by 24 | `94f552e7f207b40c` | `620256268dda7e56` | `2a0f4cd0c8e2c7ad` (5 workers) |
| Julia 32 by 24 | `41759bab6ee892b3` | `bf0e5edc2606d17c` | `58eb7ae4f27c9fa4` (4 workers) |

The padded 4-by-4 computation fixture is now `2ba1bf582aa6ea82`, with assignment
identities `380b6409f268c139` and `13bb870b5cc81b51`, and set identity
`8b3dca5188141c05`. The tightly packed numeric socket fixture is
`78c49f1d7a64168f`, with assignments `c5383a8c1f9c3629` and
`4541a2b20bee7441`, and set `16b934b9611d7134`.

All frozen field, pixel, artifact, analyzer record, analyzer-chain, formula,
computation-module, and scheduler-module identities listed in the baseline are
unchanged. Serial and thread-pool outputs remain byte-equivalent and repeated
executions reproduce identical fields, analyzer records, artifacts, work-unit
sets, and manifests.

## Deliberate exclusions

This milestone does **not** introduce arbitrary precision, interval arithmetic,
SIMD, GPU arithmetic, fixed point, CDC arithmetic, formula extraction, or
performance optimization. Binary64 is the only installed backend, and no
performance claim is made.

## Recommended next milestone

**Extract the formula socket from the computation module while preserving the Binary64 numeric contract.**
