# Two-child transition trace

| Transition | Obligations before → after | Potential | Guard/evidence | Ownership | Strict? |
|---|---|---:|---|---|---|
| split | `{R}` → `{left,right}` | `3 -> 2` | exact split, coverage, validity, rank comparison | conventional geometry + CDC rewrite (mixed) | yes |
| left discharge | `{left,right}` → `{right}` | `2 -> 1` | left lower bound `95/16>2`; verified guard installed | conventional truth + CDC discharge (mixed) | yes |
| right discharge | `{right}` → `{}` | `1 -> 0` | right lower bound `207/32>2`; verified guard installed | mixed | yes |

The symmetric right-first trace is equally valid; child order is not part of the potential. Terminality combines the empty obligation multiset, both independently verified child theorems, and exact coverage to conclude the parent theorem. The sequential certificate counts three CDC transformations; its orbit counts remain zero CDC steps.

Batch path: `{R}` `3 -> 2` `{left,right}` by the same split, then `{left,right}` `2 -> 0` `{}`. Both guards and evidence must validate. The logical batch is permitted because the two discharge updates affect distinct obligation/evidence identities and therefore commute (Definition 20.2); it counts one CDC batch transformation after the split, not two. No threads or scheduling are implied.
