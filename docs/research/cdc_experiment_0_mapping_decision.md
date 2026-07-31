# Experiment 0 mapping decision

## Decision: `NEGATIVE_RESULT`

| Element | Candidate | Grounding | Evidence | Status |
|---|---|---|---|---|
| Configuration | orbit plus audit record | CDC Def. 5.1 only supplies required shape | precise fields | partial conventional record; no complete CDC configuration |
| Progress structure | magnitude, reciprocal, budgets, obligations, proof prefixes | Defs. 5.1–5.2; fixed-point/bisection patterns checked | per-state candidate values | rejected as increasing, circular, cosmetic, or unsupported |
| Certified step | recurrence plus potential inequality | Def. 5.2 | arithmetic proves recurrence only | rejected |
| Invariant | recurrence/bit-exact trace | Def. 20.3 | direct trace checks | conventional only; no CDC progress link |
| Terminal certificate | exact escape witness | §23.1 obligations | validated conventional fields | CDC terminal rule absent |
| Finite-precision rule | experiment-specific bit evidence | no CDC numeric rule | exact values and operations | supported only for conventional Experiment 0 |
| Escape implication | exact witness entails conventional escape | stipulated conventional semantics | `36>4` | supported conventionally; not CDC halt |

`NEGATIVE_RESULT` — The conventional trace and its finite-precision certificate are sound for this point, but the studied proposal cannot meet the essential CDC progress, certified-step, invariant-to-progress, and terminal-rule obligations without encoding the known two-step answer or relabeling ordinary iterations. Conventional escape is not proof of CDC validity.

This result would change only after a source-supported or independently reviewed mathematical construction supplies a nonnegative potential, guards and updates, non-circular local inequalities, a meaningful preserved invariant, and a CDC halt theorem soundly implying conventional escape, together with finite-precision proof rules. A merely different counter or monotone transform is insufficient.

Recommended next study: investigate, on paper and without implementation, whether an escape-region predicate can participate in a CDC rule whose potential and guard are defined independently of a precomputed witness. Until then the backend explicitly returns unresolved classification with `fallback_required=true`, zero CDC steps, and the negative mapping identity.
