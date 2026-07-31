# Two-child candidate base orders

Parent dimensions are `(width,height)=(1/4,1/4)` and area `1/16`; each child is `(1/8,1/4)` with area `1/32`. All geometric orders below are engineering candidates, not CDC-defined Mandelbrot measures.

| Candidate (domain) | Parent; children | Strict for both? | Well-founded / degeneracy | Proof meaning, counterexample, source status |
|---|---|---|---|---|
| width (`R>=0`) | `1/4`; `1/8,1/8` | yes | `<` on positive reals is not well-founded; zero boxes invalid | geometry only; imaginary split leaves width unchanged; bisection analogy, inapplicable premises |
| height (`R>=0`) | `1/4`; `1/4,1/4` | no | same real-order problem; zero invalid | immediately fails this split; unsupported |
| max side (`R>=0`) | `1/4`; `1/4,1/4` | no | same | split of a square along one axis is a counterexample; unsupported |
| area (`R>=0`) | `1/16`; `1/32,1/32` | yes | arbitrary halving gives infinite descent; degenerates reach zero | geometry only without a discrete representation rank; unsupported alone |
| diameter (`R>=0`) | `sqrt(1/8)`; `sqrt(5/64)` | yes | real order not well-founded; zero invalid | geometry only; rounding square roots adds uncertainty; unsupported |
| lexicographic `(max_side,area)` | `(1/4,1/16)`; `(1/4,1/32)` | yes by area | real lexicographic order not well-founded | geometry only; repeated shrinking counterexample; unsupported |
| proof-sheet deficit (`N`) | `1`; `2,2` if counted per child | no | naturals are well-founded; no degenerates issue | increases/encodes schema; rejected as cosmetic |
| unresolved evidence deficit (`N`) | `1`; `2,2` | no | well-founded | evidence need is not reduced by split; rejected |
| dyadic area rank (`{0,1}` here) | `1`; `0,0` | yes | finite registered domain is well-founded; degenerate boxes rejected | rank is defined before evidence from `area/q-1`, `q=1/32`; meaningful only as structural decomposition, engineering interpretation |

The selected base order is the final, narrow dyadic rank. It does not prove escape: it proves only that this registered parent replacement is structurally smaller. The conventional guards remain necessary.
