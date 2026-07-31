# Experiment 0: certified steps and invariants

## Candidate relation

`ENGINEERING_INTERPRETATION` — Ordinary evaluation computes `fl(z_n²+c)`. Conventional advancement appends that value and increments the conventional count. A **CDC-certified** transition would additionally require: precondition `G(x)`; an input CDC configuration; a defined `U`; output `U(x)`; an initially true and preserved invariant; a proved token/factor inequality for `Φ`; and auditable evidence of every premise. Failure to prove any item produces no CDC step.

| Transition | Arithmetic evidence | Conventional status | CDC status |
|---|---|---|---|
| `0→2` | exact binary64 operand/result bits and exact-real operation argument | valid advancement | `NEGATIVE_RESULT`: no accepted `Φ,G,U` certificate |
| `2→6` | exact binary64 operand/result bits and exact-real operation argument | valid advancement and escape witness | `NEGATIVE_RESULT`: escape comparison is not a CDC inequality or halt rule |

`SOURCE_DEFINED` — An invariant is a predicate true initially and preserved by every enabled update (Definition 20.3 and Theorem 20.4, printed p.65).

Candidate conventional invariant: `z_0=0` and `z_{k+1}=z_k²+2` for each recorded transition. It is initially vacuous and extended by exact arithmetic; it supports the exact conventional trace and terminal comparison. Candidate floating-point invariant: all recorded imaginary components have bits zero and the real components have the specified bits; direct per-step evaluation preserves it for these operands. Both are `ENGINEERING_INTERPRETATION` and depend on the finite-precision assumptions in the evidence study.

`NEGATIVE_RESULT` — Neither proposition is a meaningful CDC invariant connected to CDC progress: one restates the recurrence and the other audits its implementation. No CDC invariant is accepted. The failure condition remains explicit, and the CDC descent count is zero.
