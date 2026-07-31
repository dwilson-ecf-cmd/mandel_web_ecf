# Two-child multiset progress

For this study only, an obligation has base rank `rho(R)=1` and `rho(R_left)=rho(R_right)=0`, derived from exact area units `area/q` with `q=1/32`. The base order is natural `<` restricted to `{0,1}`, hence well-founded.

The repository-facing Dershowitz–Manna-style rule is: multiset `M'` is below `M` when a nonempty submultiset `X` is removed from `M`, a finite multiset `Y` is inserted, every `y in Y` is strictly below at least one removed `x in X`, and all untouched elements are identical. Here `X={R}`, `Y={R_left,R_right}`. Duplicate rank-zero children are permitted because each is below rank one; identity remains separate from rank. Discharge uses `Y=empty`, so removing an obligation descends. The empty multiset is terminal only when all corresponding conventional claims have verified discharge evidence.

For executable token accounting, encode each rank by `w(r)=3^r` and sum weights. Then `{R}` has 3, `{left,right}` has 2, either singleton has 1, and empty has 0. Replacement decreases by at least one because the fixed branching bound is two and base 3 is greater than two. This numeric potential is nonnegative, uses no future discharge order or proof tree, and supplies the Definition-5.2 token inequality for the registered transitions. The finite rank domain (and equivalently the natural-valued token potential) is well-founded.

Unresolved numeric uncertainty is a status/evidence field, never a smaller rank: the multiset and potential remain unchanged. This implementation validates only `2 area units -> 1,1`; it does not expose a general multiset theorem, arbitrary depth, or arbitrary branching.
