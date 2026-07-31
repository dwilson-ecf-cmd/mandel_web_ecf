# Experiment 0: inspection and candidate state

## Inspection record

`SOURCE_DEFINED` — The authoritative `CDC.pdf` was checksum-verified as `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`. The passages used are: configuration `(x, Φ, G, U, H)`, Definition 5.1, printed p.12 (PDF p.20); token/factor certified steps, Definition 5.2, p.12; finite token and factor bounds, Lemmas 5.3–5.4, p.12; composition, Theorem 5.5, p.12; bisection, Example 16.13, p.49; guarded choice and invariants, Definitions 20.1 and 20.3 and Theorem 20.4, p.65; proof-sheet obligations, §23.1, p.71; and the fixed-point cookbook pattern, Appendix C.1 item 11, p.76. `SOURCE_DEFINED` — These passages define CDC generally; none mentions Mandelbrot iteration.

`ENGINEERING_INTERPRETATION` — Before proposing the mapping, the reference index, glossary, interpretation ledger, computation interface, experimental backend, Experiment 0 specification, conventional oracle, and native tests were inspected. The existing separation between conventional iterations and CDC steps is retained.

## Candidate record

This is a research record, not an accepted CDC configuration.

| Field | Purpose | Grounding/category | Kind | Changes in proposed step? | Necessary? |
|---|---|---|---|---|---|
| `z_n` | conventional recurrence input | proposed use of CDC `x`; `ENGINEERING_INTERPRETATION` | mathematical | yes | yes for recurrence |
| `c=2+0i` | fixes recurrence | no CDC-specific rule; `ENGINEERING_INTERPRETATION` | mathematical constant | no | yes |
| `n` | identifies trace state | implementation bookkeeping; `ENGINEERING_INTERPRETATION` | metadata | yes | only for trace/evidence |
| radius `2` | defines conventional terminal comparison | conventional semantics; `ENGINEERING_INTERPRETATION` | mathematical constant | no | yes for escape claim |
| budget `4` | bounds executable work, not mathematics | `ENGINEERING_INTERPRETATION` | metadata | decreases | no for this witness; required operationally |
| binary64 and operation order | qualifies computed claims | `ENGINEERING_INTERPRETATION` | evidence metadata | no | yes for executable evidence |
| rounding/compiler assumptions | scope of exactness argument | `ENGINEERING_INTERPRETATION` | evidence | no | yes |
| evaluated values and bits | audit arithmetic | `ENGINEERING_INTERPRETATION` | evidence | accumulates | yes for verifier |
| unresolved CDC obligations | prevents false certification | ledger-backed; `ENGINEERING_INTERPRETATION` | evidence | no accepted discharge | yes for honesty, not CDC mathematics |
| terminal-claim state | distinguishes binary64/exact/escape/CDC claims | `ENGINEERING_INTERPRETATION` | evidence | advances conventional claims only | yes for certificate audit |
| `Φ,G,U,H` | required CDC structure | Definition 5.1; `SOURCE_DEFINED` | CDC mathematics | would vary by definition | necessary, but no valid instantiation found |

`NEGATIVE_RESULT` — Adding the conventional fields creates a precise conventional/evidence record, but it does not supply `Φ`, a certificate inequality, or a CDC halt rule. Thus it is not accepted as a complete CDC configuration.
