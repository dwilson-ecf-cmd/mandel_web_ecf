# Counterfactual futures

| Future at S1 | State valid? / progress | Result and descent | Certificate consequences |
|---|---|---|---|
| A splits; B does not | yes; registered split_2 | `6->5`; B may remain `UNRESOLVED` | split_1 unchanged |
| B splits; A does not | S1/ranks valid; rank rule is symmetric, but no B geometry is executable | no accepted transition; fallback or `UNRESOLVED` | parent unchanged |
| A discharges first | yes with conventional guard | `6->3`; no split required | split_1 unchanged |
| B discharges; A later splits | yes | `6->3->2`, then leaf discharges | A rank remains 1; parent unchanged |
| one branch stalls numerically | yes | no CDC step for that branch; `UNRESOLVED`, potential unchanged | all certificates unchanged; fallback optional |
| both unresolved | yes | no progress, S1 remains potential 6 | no false CDC transition |

The system encodes no preferred guard outcome or discharge ordering. Its executable split geometry deliberately recognizes A-only, so B refinement is an honest unsupported operation rather than an inferred mirror tree. Remaining unresolved is valid proof state, not terminal success.
