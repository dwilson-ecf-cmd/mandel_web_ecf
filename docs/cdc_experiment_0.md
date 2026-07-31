# CDC Experiment 0 — computation-substrate audit

Experiment 0 tests one backend-neutral point problem without a renderer. Successful conventional escape does not prove that a valid CDC mapping exists.

## Fixed problem

Formula `zₙ₊₁=zₙ²+c`; `z₀=0+0i`; `c=2+0i`; IEEE-754 binary64 in the implementation's stated operation order; escape radius `2`; iteration budget `4`. Escape is the strict post-update condition `|z|>2`.

## Conventional path and oracle

The `conventional-c` backend records `z₀=(0,0)`, `z₁=(2,0)`, and `z₂=(6,0)`. The first state equals, but does not exceed, the escape radius. The second update supplies escape evidence, so classification is `ESCAPED` and `conventional_iteration_count=2`. CDC work remains zero. Budget exhaustion in any conventional case is `UNRESOLVED`, not `PROVEN_BOUNDED`.

## CDC path

The proposed configuration draws only on CDC-CONFIG-001 and CDC-CERT-001. Treating a conventional complex orbit as proposed CDC state is an `ENGINEERING_INTERPRETATION`. A nonnegative potential, certified step relation, invariant, terminal equivalence, and finite-precision evidence rule are all `UNRESOLVED`; the PDF does not provide them for `z²+c`. Therefore the backend returns `UNRESOLVED`, zero CDC descent steps, `fallback_required=true`, and ledger-linked evidence. No conventional fallback is silently executed inside that backend.

## Evidence and required outcomes

Record problem/backend metadata, each conventional state, strict escape decision, both work counters, precision, classification, unresolved/fallback reason, trace identity, and the CDC PDF checksum only for the CDC path.

The study conclusion must be exactly one of:

- `SUPPORTED_MAPPING`: every CDC obligation and finite-precision rule is justified and terminal equivalence is reviewed.
- `PARTIAL_MAPPING`: some source-grounded obligations exist, with missing ones named and no excess claim.
- `UNRESOLVED_MAPPING`: required obligations cannot yet be stated or justified.
- `NEGATIVE_RESULT`: review establishes that the proposed mapping is invalid or mere relabeling.

The completed focused study records `NEGATIVE_RESULT`: the conventional trace and its finite-precision evidence are justified, but all studied progress structures are cosmetic, circular, or unsupported as CDC progress. The CDC backend therefore still returns conventional classification `UNRESOLVED`, requires fallback, and reports zero CDC descent steps, now with an explicit negative-result identity. Moving to a point corpus requires the gates in `computation_architecture.md`; performance is outside this experiment.
