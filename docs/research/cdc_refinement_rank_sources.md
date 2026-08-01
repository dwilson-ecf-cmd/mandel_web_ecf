# Refinement rank sources

All decisions concern rank *sources*, independently of whether a particular depth-two trace happens to pass.

| Candidate | Exact definition / supplier / required information | Local, future-independent, well-founded | Correctness and proof progress | CDC relation | Failure cases | Decision |
|---|---|---|---|---|---|---|
| A: explicit maximum depth | Contract owner supplies `D`; `rank=D-lineage_depth`, requiring immutable contract and current lineage | yes; naturals; no final shape or guard outcome | bounds certified splits and each split consumes one; does not prove terminal truth | engineering resource contract instantiating Definition 5.2 after split proof | a value selected after seeing a desired tree is disguised experiment depth; scheduler-only depth is not proof input | **SUPPORTED** as a bounded proof-search contract, not CDC-native or geometric potential |
| B: minimum width | Contract owner supplies exact `w_min`; for registered real dyadic halving, rank is the unique `k` with `current_width / 2^k = w_min` | local from exact endpoints; future-independent; finite only because floor is supplied | geometric capacity for real-axis splits, not proof-theoretic evidence | engineering potential compatible with CDC inequalities | height needs a separate floor for imaginary splits; anisotropic or nonhalving rules need a vector/rule-specific derivation; inexact ratios invalidate rank | **SUPPORTED** only for exact registered real halving to `1/16` |
| C: area/diameter | remaining exact reductions to a declared floor | potentially local, but no single rule works for allowed geometries | geometric, not evidence progress | no source-native status | vertical-only shrink leaves width/diameter; area can shrink while maximum side does not; alternate axes consume scale differently; floating comparison may be undecidable | **REJECTED** for v1; a future rule-specific vector measure is unresolved |
| D: evidence deficit | number of absent units in a fixed evidence schema | local only if units are immutable and independent | discharge can decrease it, but splitting one claim into two can increase it | proof-sheet inspired, engineering instantiation | arbitrary “units,” dependencies, or schema written around final proof are circular; branching deficit may rise legitimately | **REJECTED** as refinement rank; preserved single-obligation discharge remains supported |
| E: CDC token/factor allowance | initial finite `Phi` or multiplicative capacity supplied with certified updates | local if supplied and immutable; naturals well-founded | proves accounting decrease, not guard truth | token/factor inequality and derived bounds are source-defined; choosing fractal tokens is not | renaming depth as tokens adds no independence; PDF supplies no allowance | **PARTIAL**: fixed tokens are a resource-contract engineering adaptation, never claimed CDC-native |
| F: conventional analytic bound | independently proved enclosure/guard-distance/precision theorem supplies finite required refinements | local use is possible after theorem; future-independent | can connect geometry to proof completeness | conventional premise may feed CDC contract | no general theorem exists here; boundary distance may be zero/unknown; finite precision may not establish guard | **UNRESOLVED** generally; no new analytic bound claimed |
| G: compute budget | user/system supplies maximum nodes, splits, proof steps, or time | counters are local and finite | resource depletion is not necessarily proof progress; time is replay/platform dependent | scheduling/proof-sheet metadata only | a split may spend budget while increasing obligations; wall time varies; queue policy changes node use | **REJECTED** as CDC rank; allowed operational limit outside proof validity |

## Candidate details

### A — legitimate but conditional

A depth limit is a legitimate contract precisely when declared before execution, identified immutably, enforced on every branch, and exhaustion is reported as incomplete. It remains conventional bounded proof-search policy, not a theorem that depth is sufficient. Thus `D=2` can justify the old ranks as contract-relative ranks, while an after-the-fact `D=2 because this tree has depth two` cannot. Independence is procedural/provenance-based and must be audited; the numeric value alone cannot distinguish the two histories.

### B/C — dimensions and representation

For a real split, `rank_real=k` needs no height calculation. If both axes may split, separate `(rank_real, rank_imaginary)` or a declared lexicographic/multiset order is necessary; using width alone would permit unlimited imaginary splitting. Diameter can remain constant when only the shorter side shrinks. Area decreases under either axis but does not ensure every required dimension reaches its floor. Native v1 uses exact binary64 dyadics whose subtraction and halving are exact; any nonexact minimum or ratio is `INVALID_SPEC`, not rounded into a rank.

### D/E — obligations versus accounting

Evidence is not fungible: coverage, guard soundness, and terminal truth are different propositions. Splitting removes one parent claim but creates two child claims plus coverage evidence, so “missing fields” can increase without regress. Token allowance is usable accounting only because the contract supplies it and the certified rule consumes it; it is unsupported analogy to call that allowance CDC-native.

### F/G — completeness versus operation

A separately proved analytic threshold would be the strongest independent source because it could relate allowance to completeness, but this repository has no such general result. Node/time budgets remain useful cancellation policies. They must not appear as obligation rank or turn exhaustion into failure.
