# Experiment 0: progress investigation

`SOURCE_DEFINED` — CDC progress requires a nonnegative potential and, under a guard, a token drop `Φ(U(x))≤Φ(x)−δ` with `δ>0`, or factor drop `Φ(U(x))≤qΦ(x)` with `0<q<1` (Definitions 5.1–5.2, printed p.12). Finite bounds follow only under those hypotheses (Lemmas 5.3–5.4).

| Candidate definition/domain→codomain | Values on `0→2→6` | Descent / well-foundedness | Connection and failure | Status |
|---|---|---|---|---|
| magnitude `Φ(z)=|z|`, `C→R≥0` | 0, 2, 6 | increases | fails either CDC inequality | `NEGATIVE_RESULT` |
| reciprocal `1/(1+|z|)`, `C→(0,1]` | 1, 1/3, 1/7 | decreases on this trace | no source establishes a uniform legal-step factor, terminal meaning, or general guard; chosen after observing the witness | `EXPERIMENTAL_HYPOTHESIS`, rejected |
| remaining budget `Φ=B−n`, finite states→`N` | 4, 3, 2 | token drop 1, lower bound 0 | merely runs a counter backward; reaching zero means exhaustion, not escape or proof | `ENGINEERING_INTERPRETATION`, rejected |
| remaining known transitions to witness `Φ=2−n`, `{0,1,2}→N` | 2, 1, 0 | token drop 1, well-founded `N` | circular: embeds the already-known witness and renames iterations | `ENGINEERING_INTERPRETATION`, rejected |
| unresolved obligations cardinality | fixed set, unless ordinary arithmetic is declared a discharge | no justified strict drop | no proof rule says arithmetic discharges CDC obligations | `UNRESOLVED`, rejected |
| terminal uncertainty ordered false/true | unknown, unknown, escape-known | one evidence refinement | an information order is not the source's numerical potential; it certifies conventional evidence only | `ENGINEERING_INTERPRETATION`, rejected as CDC |
| guarded finite witness construction | trace prefixes of lengths 1,2,3 | prefix order grows; reverse length is circular | guarded choice (Def. 20.1, p.65) supplies choice semantics, not descent | `EXPERIMENTAL_HYPOTHESIS`, rejected |
| fixed-point potential `|z−z*|` | no applicable known attracting `z*` and contraction | none established | Appendix C.1 item 11 requires a known fixed point and local contraction; orbit escapes | `SOURCE_DEFINED` pattern, inapplicable |
| bisection interval width | no interval/sign-change problem | no values | Example 16.13 needs continuity, a sign change, and interval halving | `SOURCE_DEFINED` pattern, inapplicable |
| amortized potential plus bank | no nonnegative bank with proved combined drop | none | Definition 22.4 cannot manufacture the missing inequality | `UNRESOLVED`, rejected |

`NEGATIVE_RESULT` — For this studied mapping, every decreasing candidate is circular, cosmetic, or lacks a source-supported connection to escape. No meaningful CDC progress structure is accepted. A different mathematical transformation with an independently proved potential and terminal rule could change this result.
