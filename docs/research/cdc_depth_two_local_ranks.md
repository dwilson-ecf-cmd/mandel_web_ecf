# Local-only rank candidates

Grounding labels follow the ledger: `SOURCE_DEFINED` means CDC.pdf states it; `ENGINEERING_INTERPRETATION` means external mathematics supplies it; `UNSUPPORTED` means it does not justify this study. CDC Definitions 5.1–5.2 (printed p.12) require a nonnegative potential and certified token/factor decrease, but do not supply fractal ranks. The selected rule is `REGISTERED-LOCAL-REFINEMENT-BUDGET-V1`: an obligation is created with a refinement allowance; a split consumes one. The study input gives R allowance 2; any locally selected child of a split inherits `parent allowance - 1`. Thus ranks are `R=2`, `A=B=1`, `C=D=0`. The allowance bounds capability, not a promised tree: at S1 either child may split, discharge, or stall.

| Candidate / exact definition | Creation-time information | R; A,B; C,D | Child smaller? / siblings | Well-founded | Future knowledge / counterexample | Class |
|---|---|---|---|---|---|---|
| inherited rank minus one, `rho(child)=rho(parent)-1` | parent certificate and current rule | `2;1,1;0,0` | yes; equal allowed | naturals | no; rejects a split at rank zero | engineering, **selected** |
| remaining allowed depth `budget-depth` | registered budget and current lineage | `2;1,1;0,0` | yes | finite naturals | no final shape, but requires an independently declared bound | engineering; equivalent selected formulation |
| exact dyadic level `-level` | endpoint denominators | `-2;-3,-3;-4,-4` (one convention) | numerically smaller but not nonnegative | integers are not well-founded downward | repeated bisection descends forever | unsupported |
| width exponent `log2(width)` | current bounds | `-2;-3,-3;-4,-4` | yes numerically | not well-founded over arbitrary dyadics | indefinite halves | unsupported alone |
| area exponent `log2(area)` | current bounds | `-4;-5,-5;-6,-6` | yes | same infinite-halving counterexample | no future tree, but no lower bound | unsupported alone |
| structural depth `depth` | lineage | `0;1,1;2,2` | children are larger, not smaller | natural `<` is wrong orientation | negating recreates non-well-founded integers | unsupported |
| locally generated token rank `parent token factor - 1` | certified parent token | `2;1,1;0,0` | yes | naturals | no, provided zero refuses split; same rule under another name | engineering |
| lexicographic `(allowance,width exponent)` | allowance plus geometry | `(2,-2);(1,-3);(1,-3);(0,-4);(0,-4)` | yes on first coordinate | finite first coordinate controls registered steps | no; second coordinate is unnecessary | engineering |
| source potential `Phi(configuration)` | current composite state | unassigned | not established | source theorem applies only after Phi is supplied | CDC.pdf supplies schema, not this Phi | source-defined schema, unsupported instantiation |

The selected rank is assigned and serialized at creation, siblings may share it, and no assignment is rewritten. It supports only allowances 2, 1, and 0 and the two exact registered rules—not arbitrary recursion.
