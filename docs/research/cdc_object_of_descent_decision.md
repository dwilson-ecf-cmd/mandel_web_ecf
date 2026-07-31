# CDC object-of-descent decision

| Candidate | Configuration | Potential | Certified step | Invariant | Terminal meaning | Decision |
|---|---|---|---|---|---|---|
| Complex-plane region | definable | none intrinsic | classification conventional; subdivision unsupported | coverage possible | quantified region claim | `PARTIAL_OBJECT` |
| Interval orbit enclosure | definable | none | propagation conventional | containment | enables guard | `PARTIAL_OBJECT` |
| Proof obligation | typed Study-0 claim | fixed deficit | verified discharge, token 1 | sound claims only | claim discharged | `SUPPORTED_OBJECT` |
| Guarded partition | finite cells | candidates fail/generalize poorly | no certified subdivision | coverage | all cells classified | `UNRESOLVED_OBJECT` |
| Certificate state | fixed typed schema | missing verified claims | evidence installation | evidence/claim soundness | valid proof sheet | `PARTIAL_OBJECT` |
| Other | finite rule-work collection | no supplied ranks | none beyond Study 0 | semantic equivalence | empty work set | `REJECTED_OBJECT` |

## Overall conclusion: `COMPOSITE_OBJECT_REQUIRED`

The exact tuple is `(R, Z/evidence assumptions, O, E)`, where `R` is immutable claim subject, `Z` is conventional enclosure evidence, `O` is a finite typed unresolved-obligation set, and `E` is accumulated independently verified evidence. Neither region nor certificate storage alone changes meaningfully. In Study 0, conventional validation enables the update `(O={REGION_ESCAPES(R)},E=empty) -> (O=empty,E={verified witness})`; `Phi=|O|` drops `1 -> 0`, invariant “every discharged claim has verified sound evidence” is preserved, and terminality means the conventional region claim is proved. This is a source-compatible engineering instantiation, not a PDF theorem about fractals.

The direct mapping `raw Mandelbrot orbit advancement = CDC descent` remains an authoritative `NEGATIVE_RESULT`. Exact arithmetic does not imply CDC certification; conventional escape does not automatically imply descent; subdivision is not automatically certified; passive proof storage is not construction. Only the explicit obligation discharge counts as one CDC step, while the existing direct-orbit backend and certificate remain at zero.

## Limitations and next experiment

No boundedness, arbitrary region, recursion, scheduling, parallelism, performance, or generalized termination result follows. Next, test a two-child proof obligation where one child is discharged and one remains unresolved, pre-registering the schema and multiset potential before execution; reject it if branching cannot obtain a source-compatible strict decrease without encoding the intended proof.
