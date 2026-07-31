# CDC interpretation ledger

| ID | Statement | Classification | Source | Confidence | Validation needed |
|---|---|---|---|---|---|
| CDC-LEDGER-001 | A CDC configuration contains state, nonnegative potential, guard, update, and halt predicate. | `SOURCE_DEFINED` | CDC-CONFIG-001 | High | Check exact source edition by PDF checksum. |
| CDC-LEDGER-002 | A certified step meets a token or factor inequality under its guard. | `SOURCE_DEFINED` | CDC-STEP-001 | High | Preserve all preconditions. |
| CDC-LEDGER-003 | A C record may hold proof-sheet observations without claiming a mathematical implementation. | `ENGINEERING_INTERPRETATION` | CDC-CERT-001 | Medium | Review trace schema against Experiment 0. |
| CDC-LEDGER-004 | A Mandelbrot orbit state might be proposed as CDC state. | `ENGINEERING_INTERPRETATION` | No direct source mapping | Low | Identify a nonnegative potential, guard, update, certificate, and halt meaning. |
| CDC-LEDGER-005 | CDC might reduce conventional work. | `EXPERIMENTAL_HYPOTHESIS` | None | Unknown | Controlled correctness-first experiments; never infer from terminology. |
| CDC-LEDGER-006 | CDC step/branch/classification counters record experimental proof activity and remain semantically separate from conventional iterations. | `ENGINEERING_INTERPRETATION` | CDC-CERT-001 | Medium | Define each counter before Experiment 0 execution. |
| CDC-LEDGER-007 | The PDF does not supply a CDC potential or descent certificate for `z↦z²+c`. | `UNRESOLVED` | Reference-wide inspection | High | Mathematical review; a negative finding is acceptable. |
| CDC-LEDGER-008 | Precision/rounding rules for computational CDC certificates are not stated. | `UNRESOLVED` | Reference-wide inspection | High | Define sound finite-precision evidence before implementation. |
| CDC-LEDGER-009 | No source rule equates CDC halt with Mandelbrot escape/bounded classification. | `UNRESOLVED` | CDC-HALT-001 | High | Prove equivalence or require conventional fallback. |
| CDC-LEDGER-010 | Spatial renderer tiles are unrelated to the PDF's CDC “tiles” unless separately demonstrated. | `ENGINEERING_INTERPRETATION` | Def. 5.1 / visual legend | High | Maintain naming distinction in APIs/docs. |

| CDC-LEDGER-011 | CDC is a computation concern; a renderer may only consume neutral results. | `ENGINEERING_INTERPRETATION` | CDC-CONFIG-001 | High | Validate future result-to-raster contract. |
| CDC-LEDGER-012 | The Experiment 0 CDC backend must return unresolved and request fallback until all proof and precision obligations exist. | `ENGINEERING_INTERPRETATION` | CDC-LEDGER-007–009 | High | Mathematical and finite-precision review. |
| CDC-LEDGER-013 | A sound finite-precision CDC certificate rule for the Mandelbrot proposal is absent. | `UNRESOLVED` | Reference-wide inspection | High | Supply reviewed rounding/interval evidence or retain unresolved. |
