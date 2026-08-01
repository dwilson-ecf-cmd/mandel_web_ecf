# Fixed-schema iteration histogram analyzer

`fractal.analyzer.iteration-histogram` version 1 emits one
`fractal.analysis.iteration-histogram.v1` record containing exactly 64 unsigned
integer counts. The fixed schema makes records comparable across image sizes and
iteration budgets and avoids budget-dependent bin definitions.

## Exact v1 bins

Bounds are inclusive. Bin 0 contains iteration 0, bin 1 iteration 1, and bin 2
iteration 2. For every bin `b` from 3 through 62, the range is
`2^(b-2) + 1` through `2^(b-1)`. Thus bin 3 is 3–4, bin 4 is 5–8, bin 5 is
9–16, and the last ordinary bin (62) is `2^60 + 1`–`2^61`. Bin 63 is the
overflow bin and contains every value greater than `2^61`. The public bucket
function accepts a 64-bit iteration so the overflow rule remains testable even
though the current compact field stores 32-bit iterations.

Escaped samples contribute their stored one-based escape iteration. Bounded
samples contribute their stored iteration, including zero. Unresolved samples
contribute their stored consumed maximum-iteration budget. Cancelled and failed
partial samples are excluded. Any unknown classification rejects the entire
field, and no record is completed. The eligible and excluded counts conserve
the total sample count. An empty eligible set has zero min, max, and sum and a
false validity flag.

Registration changes the authoritative installed-module registry identity to `be502d689109b3cb`.

The analyzer reads in deterministic row-major order, checks cancellation before
work and at every row boundary, borrows and preserves the original field, uses
caller-owned fixed-capacity record storage, and performs no heap allocation.
Canonical JSON uses a fixed field order and fixed bin order. The record stores
integer counts, min/max/sum, and validity—not floating-point averages or
percentages, which are losslessly derived presentation data.

## Scope and non-goals

This analyzer is descriptive evidence only and does not influence scheduling.
This milestone does not implement spatial workload maps, tile scheduling,
thread pools, adaptive scheduling, connected components, orbit capture,
periodicity detection, distance estimation, CDC evidence, Ouro integration, or
Android UI. The pipeline remains zero-or-one analyzer.
