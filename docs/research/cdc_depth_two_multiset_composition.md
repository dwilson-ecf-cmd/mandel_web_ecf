# Depth-two multiset composition

Let the base order be natural `<` restricted to ranks `{0,1,2}`. Its strict part is well-founded. Its finite multiset replacement extension says `M > M'` when a nonempty submultiset X is removed, finite Y is inserted, every member of Y is below a removed member, and untouched members are identical. Consequently `{R}>{A,B}` and `{A,B}>{C,D,B}`; duplicate equal-rank children are permitted and B is unchanged.

The executable sufficient encoding is `W(M)=sum 3^rho(x)`. For replacement of one rank-r parent by at most `k=2` children of rank at most `r-1`, `W(children)<=2*3^(r-1)<3^r`; base 3 must exceed the registered branching bound 2. Equality/base 2 would not prove strict descent with two maximum-rank children. This is a proof for finite binary replacements with these bounded natural ranks, not CDC-native multiset semantics or a general partition theorem.

| State | ranks | W |
|---|---|---:|
| S0 | `{2}` | 9 |
| S1 | `{1,1}` | 6 |
| S2 | `{0,0,1}` | 5 |
| after C | `{0,1}` | 4 |
| after D | `{1}` | 3 |
| sequential/batch terminal | `{}` | 0 |

Discharge removes one or more independently verified obligations (empty replacement), so sequential and commuting batch discharge descend. An empty state is terminal only with all guard/discharge and coverage evidence. The C implementation rejects overflow while forming powers or sums (`weight > UINT64_MAX/3` and `sum > UINT64_MAX-weight`); its certificate admits only the small registered values.
