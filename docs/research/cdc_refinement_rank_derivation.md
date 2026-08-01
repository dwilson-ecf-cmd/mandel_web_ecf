# Contract-relative rank derivation

`rank = derive(contract, obligation)` reads only immutable contract fields and the obligation's current exact region/lineage.

| Surviving type | Root / child / terminal rank | Split and exhaustion rules | Invalid conditions |
|---|---|---|---|
| maximum depth `D` | `D`; child `D-(depth+1)`; zero at maximum lineage | equal-ranked siblings permitted; split iff unresolved and rank > 0 | depth > D, wrong target/identity, rewritten creation rank |
| minimum real width `w_min` under exact real halving | unique `k` such that `width/2^k=w_min`; child `k-1`; zero at floor | real split only; equal siblings permitted | ratio not an exact power of two, below/nonrepresentable floor, imaginary/anisotropic rule |
| fixed token allowance `T` | `T`; child `T-(depth+1)`; zero at allowance | same mechanics as depth, but status remains `PARTIAL` because token source lacks independent CDC-native meaning | depth > T or token/identity mismatch |

Zero rank forbids further splitting but permits a conventional guard to discharge. Rank is not recomputed from a final tree. Maximum-depth/token rank depends on current lineage depth; width rank depends on exact current geometry and does not require lineage depth for its value. All are stable under deterministic serialization/replay.

Rejected derivations include final tree height, future leaf count, later branch choices, retrospective reassignment, and anticipated guard success. An obligation is created only if derivation succeeds; its `creation_rank` is immutable and checked against replay. Carrying an obligation does not change its rank. The native types are explicitly bounded to the registered depth-two capability, not arbitrary recursion.
