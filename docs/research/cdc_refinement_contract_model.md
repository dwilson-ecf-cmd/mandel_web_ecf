# CDC refinement-contract model

## Source ledger and preserved findings

`CDC.pdf` was verified before editing at SHA-256 `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`. The passages used are recorded exactly by role (printed page; PDF page where previously established):

| Passage | Source content used here | Limit on interpretation |
|---|---|---|
| Definition 5.1, p.12 (PDF p.20) | configuration `(x, Phi, G, U, H)` and nonnegative potential | does not define a fractal rank |
| Definition 5.2, p.12 | a guarded certified step proves token decrease `Phi(U(x)) <= Phi(x)-delta`, `delta>0`, or factor decrease `Phi(U(x)) <= q Phi(x)`, `0<q<1` | allowance is not source-native merely because it can satisfy the inequality |
| Lemmas 5.3–5.4, p.12 | finite token/factor step bounds follow from the stated decrease premises | do not supply an initial allowance or fractal bound |
| Theorem 5.5, p.12 | component certificates may be composed once their premises/certificates match | does not create missing component ranks |
| Example 16.13, p.49 | bisection descent uses continuity and sign-change premises | not a general region-refinement theorem |
| Definitions 20.1–20.3 and Theorem 20.4, p.65 | guarded choice, commuting/disjoint parallel updates, invariants | choice/scheduling supplies no descent |
| Definition 22.4, p.69 | amortization requires a nonnegative bank and proved combined decrease | a bank cannot manufacture an absent inequality |
| section 23.1, p.71 | proof sheet records state, potential, guards, updates, certificates, halt, invariants, parallel safety, schedule, and bound | record structure is not proof truth |
| Appendix C.1 item 11, p.76 | fixed-point cookbook pattern requires its analytic premises | not applicable without a contraction/fixed point proof |

The source contains no Mandelbrot subdivision rule, multiset order, minimum cell scale, refinement allowance, or theorem bounding fractal proof search. Direct orbit advancement therefore remains `NEGATIVE_RESULT`; the supported descending object remains `(region, unresolved proof obligation, verified evidence)`. The one-level and depth-two certificates are not reinterpreted or rewritten.

## Neutral model

A **refinement contract** is immutable, independently declared input that limits which semantics-preserving proof-obligation refinements a verifier may certify. It bounds proof search; it neither asserts that a guard will succeed nor converts exhaustion into a mathematical classification. A contract becomes a CDC-compatible engineering instantiation only when every accepted transition separately proves the CDC decrease and conventional correctness conditions.

| Field | Meaning | Owner | Required? | Affects correctness? | Affects rank? |
|---|---|---|---|---|---|
| target region | subject to which the contract applies | mathematical problem/application | yes in v1 | yes, through scope/mismatch only | minimum-scale rank uses its geometry |
| classification goal | typed terminal claim (escaped, bounded, either, etc.) | mathematical problem | conceptually yes; implicit `ESCAPES` in v1 | yes | no |
| maximum refinement allowance | maximum lineage refinements | contract owner | one possible limiter | not theorem truth; affects completeness | yes for depth contract |
| minimum cell scale | exact admissible geometric floor | contract owner plus conventional justification | one possible limiter | not theorem truth; split below floor invalid | yes for scale contract |
| numeric precision policy | representation/rounding requirements | conventional mathematics | required when numeric evidence is used; registered binary64 dyadics in v1 | yes | only if rank comparison relies on it |
| allowed split rules | certified decomposition schemas | conventional mathematics + contract | yes for splitting | yes | controls whether rank decrease is usable |
| fallback policy | disposition after bounded search exhausts | application/contract | optional | no effect on proved claims | no |
| termination policy | terminal versus incomplete outcomes | contract + proof specification | yes | yes for interpreting completion | no |
| evidence requirements | guard, coverage, lineage, and certificate fields | conventional mathematics/verifier | yes | yes | no, except rank evidence validation |

### Separation of concerns

* **Problem specification:** target and typed classification theorem.
* **Proof-search resource contract:** depth/scale/token limiter, permitted refinements, exhaustion/fallback.
* **Implementation metadata:** schema version, encoding, stable identifier; never a potential.
* **Scheduler limits:** time, queue order, node/time ceilings; operational only.
* **User preference:** latency/fallback preference; becomes proof input only through a separately issued contract.

The registered v1 native model deliberately supports only the existing exact target, maximum depths 0–2, exact minimum real width `1/16`, and fixed allowances 0–2. Registration is a verifier capability restriction, not evidence that the values are mathematically necessary.

## Preliminary terminology inventory

The pre-edit repository used the relevant terms in these exact places:

* `docs/research/cdc_depth_two_local_ranks.md`: names `REGISTERED-LOCAL-REFINEMENT-BUDGET-V1`, calls the root value a refinement allowance and the study input/registered rule.
* `docs/research/cdc_depth_two_future_independence.md`: calls allowance 2 a current policy/capability bound and limits the result to the declared depth-two capability.
* `docs/research/cdc_depth_two_decision.md`: calls allowance 2 the current registered study capability and says the new rule was declared before S0.
* `docs/research/cdc_depth_two_composition_rules.md`: refers to rank 2 assigned at creation and consuming the allowance.
* `docs/research/cdc_depth_two_certificate.md` and the matching native serializer/header: record the rank-rule identifier and immutable ranks.

These statements remain historical findings. The present contract model explains what would independently justify such a capability; it does not retroactively assert provenance that those documents did not record.
