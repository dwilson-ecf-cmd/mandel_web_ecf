# Fractal classification predicates

For `z0(c)=0`, `z(n+1,c)=z(n,c)^2+c`, and escape radius `B=2`:

| Predicate | Exact meaning and strength |
|---|---|
| `POINT_ESCAPES(c)` | `exists n>=0: |z_n(c)|>B`. This finite witness implies divergence by the conventional escape theorem. |
| `REGION_ESCAPES(R)` | `forall c in R, POINT_ESCAPES(c)`. A single common `n` is sufficient but not required by the definition. Sampling cannot prove it. |
| `POINT_PROVEN_BOUNDED(c)` | There is an identified sound theorem/certificate proving `forall n, |z_n(c)|<=B`; budget exhaustion is insufficient. |
| `REGION_PROVEN_BOUNDED(R)` | `forall c in R`, boundedness is proved by identified sound evidence. It is stronger than any finite sample and is outside Study 0. |
| `UNRESOLVED(X)` | Available rules establish neither the applicable escape nor proven-bounded predicate. It is epistemic, not a third mathematical orbit behavior. |
| `NUMERICALLY_UNCERTAIN(X)` | The requested sound arithmetic interpretation failed (non-finite value, unsupported rounding/enclosure, or invalid representation); it implies no classification. |

A finite region escape witness consists of a sound enclosure `Z_n` containing every `z_n(c)` for `c in R`, plus proof `inf{|z|:z in Z_n}>B`. Strict `>` matters. Binary64 values are evidence only under declared operation and rounding assumptions. A conventional predicate can enable a CDC guard, but it is not itself a CDC token/factor certificate; CDC certification separately requires configuration, potential inequality, invariant, update, and halt meaning.
