# Two-child proof state

The composite remains `(region subject, unresolved typed obligations, verified evidence, proof-sheet/token state)`.

`S0` contains immutable `R`, `O={ESCAPES(R)}`, the verified Study-0 parent evidence, and potential 3. `S1` retains the parent only as an immutable structural/provenance node, removes `ESCAPES(R)`, and contains `O={ESCAPES(R_left), ESCAPES(R_right)}`, coverage/non-overlap/validity evidence, and potential 2. Removing the parent obligation means replacing it by semantically sufficient children, not solving it.

Endpoint evidence and the formula/radius assumptions are inherited. Each child's lower bound and escape guard are recomputed independently; neither inherits the parent's discharged status. Child identity is its canonical endpoint-bit tuple plus `left` or `right` role and the split-rule identity. Coverage validation requires matching outer endpoints, equal imaginary bounds, the shared exact split endpoint, the half-open ownership convention, and valid nondegenerate boxes; gaps, reversed bounds, or a different split are rejected.

Until both child claims are discharged, the parent claim is structurally decomposed but not fully resolved. Verified child evidence accumulates monotonically; unresolved claims remain explicit.
