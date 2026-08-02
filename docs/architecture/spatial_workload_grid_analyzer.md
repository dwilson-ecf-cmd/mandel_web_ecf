# Fixed-schema spatial workload grid analyzer

`fractal.analyzer.spatial-workload-grid` version 1 is a read-only, descriptive
analyzer. It emits one `fractal.analysis.spatial-workload-grid.v1` record. The
schema is a compile-time **8 x 8** grid; cells are serialized in row-major order
(`cell_y * 8 + cell_x`). It neither changes rasterization nor feeds the renderer
or scheduler.

## Mapping and semantics

For a non-empty source field, coordinates use integer-only inclusive cell ranges:

```
cell_x = floor(x * 8 / source_width)
cell_y = floor(y * 8 / source_height)
```

Multiplication is performed after promotion to `uint64_t`, dimensions and
coordinates are validated, and the result is range checked. Thus every sample
maps once, non-divisible and sub-grid dimensions are valid, boundaries are
deterministic, and empty cells are permitted. Overflow or any failed checked
counter/sum operation rejects the analysis rather than wrapping.

Each cell counts escaped, bounded, unresolved, cancelled, and failed samples.
This exactly reuses the escape-summary interpretation: escaped, bounded, and
unresolved samples are iteration eligible; cancelled and failed partial samples
are excluded. An unresolved maximum-iteration sample has one classification and
is counted once. Iteration zero is valid. Eligible cells contain count, minimum,
maximum, and sum. An empty/non-eligible cell contains zeroes and a false validity
flag. Cell classifications conserve cell samples, and cells conserve the global
sample count. The record repeats the corresponding global eligible statistics,
source dimensions, conservation flag, and result status. Averages are not stored.

## Ownership, lifecycle, and serialization

The analyzer borrows and returns the immutable compact field, allocates no heap
memory, and requires exactly one caller-owned record slot. Compatibility and
capacity are rejected during preflight. Cancellation is checked before work and
at every source-row boundary; cancellation, unknown classifications, or failure
produces no completed record. Finish remains single-use.

Canonical JSON writes all semantic fields in stable order as decimal integers
and deterministic `true`/`false` values into a bounded caller buffer, reports the
required size, and never serializes C padding. FNV-1a is computed over that
canonical serialization only as a reproducibility identity, not a cryptographic
digest. The typed record stays with the analysis result; render and artifact
manifests retain generic analyzer/schema/result metadata rather than embedding
all 64 cells. Memory and file artifact sinks require no special ownership.

## Limitations

This observational milestone does **not** implement adaptive scheduling, dynamic
tiling, connected-component analysis, contour extraction, orbit analysis,
distance estimation, periodicity detection, boundedness proofs, CDC evidence,
renderer feedback, or multi-analyzer composition. The pipeline still selects
zero or one analyzer. The grid describes where work occurred; it makes no claim
of acceleration or performance benefit.
