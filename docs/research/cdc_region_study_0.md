# CDC region study 0

## Deterministic region

`R=[2,9/4] + i[0,1/4]`, a non-degenerate closed rectangle. All endpoints are exact binary fractions with binary64 bit identities `4000000000000000`, `4002000000000000`, `0000000000000000`, and `3fd0000000000000`. The formula is `z0=0`, `z(n+1)=z(n)^2+c`; radius is 2; iteration bound is 2; enclosures contain every exact-real orbit for every `c in R`.

## Conventional whole-region proof

At `n=1`, `z1=c`; the point `(2,0)` prevents a strict whole-enclosure escape conclusion. Write `c=x+iy`. At `n=2`,
`Re(z2)=x^2-y^2+x >= 2^2-(1/4)^2+2 = 95/16 = 5.9375 > 2`.
Thus `|z2| >= Re(z2)>2` for every `c in R`, so `REGION_ESCAPES(R)`. All products and the subtraction in this particular endpoint calculation are exact binary64 numbers under the Experiment 0 assumptions. This is a hand-derived monotonic endpoint inequality, not a general interval engine and not CDC descent.

The narrow conventional oracle recognizes exactly this box; malformed boxes are numerically uncertain and all other valid boxes are unresolved. It cannot return bounded.
