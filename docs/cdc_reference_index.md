# CDC reference index

## Document identity

The authoritative file is `CDC.pdf`, SHA-256 `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`. It is an 88-PDF-page text, *From Trigonometry to Conditional Descent Calculus: Functions, Change, and Certified Progress*. No author, publication date, or revision is stated in the extracted title/front matter or metadata inspected; that absence is recorded rather than guessed. Printed pages comprise seven Roman-numbered front-matter pages and pages 1–80.

Structure: Acts I–IV (chapters 1–19) develop prerequisite trigonometry, functions, calculus, differential equations, series, and error control. CDC is introduced in chapter 5, revisited through connectors/examples (notably chapters 9 and 16), and developed in Act V: chapter 20 choice/parallelism/invariants; chapter 21 certified descent; chapter 22 schedules/fairness/amortization; chapter 23 templates/capstones. Printed pages 73–77 give a rationale and appendices A–C; printed pages 78–80 are symbol index and bibliography.

## Traceability entries

| ID / concept | Concise source definition | Source | Preconditions / conclusions | Renderer relevance and status |
|---|---|---|---|---|
| CDC-CONFIG-001 — configuration/state | Tuple `(x, Φ, G, U, H)`: state, nonnegative potential, Boolean guard, state update, and halt predicate. | §5.1, printed p.12 (PDF p.20) | `Φ: X→R≥0`; `G,H` predicates; `U:X→X`. | A possible research record shape; **inferred**, not a pixel-orbit mapping. |
| CDC-STEP-001 — certified step | When `G(x)` holds, `U` is certified by a token or factor inequality on `Φ`. | Def. 5.2, p.12 (PDF p.20) | Token: fixed `δ>0`; factor: `0<c<1`. | Would require a proved Mandelbrot potential and certificate; **speculative** today. |
| CDC-HALT-001 — stopping | Process halts when `H(x)` or no guard is implemented/enabled. | Def. 5.2, p.12; proof sheet §23.1, p.71 | Guard coverage matters; chapter 23 calls missing guard coverage a failure. | Useful boundary for unresolved/fallback reporting; **inferred**. |
| CDC-TOKEN-001 — token bound | A nonnegative potential dropping at least `δ` permits at most `ceil(Φ(x0)/δ)` steps. | Lemma 5.3, p.12 | Every legal step has the stated fixed drop. | No such Mandelbrot drop is supplied; **not directly applicable**. |
| CDC-FACTOR-001 — factor bound | If each legal step has `Φ'≤cΦ`, then `Φ(xk)≤c^kΦ(x0)` and a tolerance gives a logarithmic bound. | Lemma 5.4, p.12 | `0<c<1`, target tolerance positive. | Potentially testable only after a justified potential; **speculative**. |
| CDC-COMPOSE-001 — composition | Sequential token drops add and factor constants multiply. | Thm. 5.5, p.12 | Each component certificate is valid. | Could support reusable certified substructure, but not yet a fractal claim; **inferred**. |
| CDC-TRAJECTORY-001 — repeated states | The source writes repeated updates as `x_k` and composes their certified inequalities; it does not define “trajectory” as a CDC canonical term. | §§5.1, 9.4, 23.1, pp.12, 26, 71 | Valid update/certificates at each step. | Do not equate a Mandelbrot orbit with this sequence; **inferred terminology only**. |
| CDC-BRANCH-001 — choice | Any enabled guarded rule may be selected. | Def. 20.1, p.65 (PDF p.73) | At least one relevant enabled guard; scheduler assumptions may be needed. | Models conditional branching abstractly; **direct CDC, inferred renderer relevance**. |
| CDC-PARALLEL-001 — parallel batch | Logical batch requires disjoint writes or commuting updates. | Def. 20.2, p.65 | Disjointness or commutativity where both guards hold. | Relevant to future tiles only as an engineering analogy; CDC does not define image tiles. |
| CDC-INVARIANT-001 — invariant | Predicate true initially and preserved by every enabled update. | Def. 20.3 and Thm. 20.4, p.65 | Preservation proof per rule. | Possible correctness evidence; **inferred**. |
| CDC-SCHEDULE-001 — schedule/fairness | A schedule is a rule-index sequence; weak fairness schedules a perpetually enabled rule infinitely often. | Defs. 22.1–22.2, p.69 | Progress theorem also assumes per-component token drops. | Relevant only if multiple CDC rules are established; **inferred**. |
| CDC-AMORTIZED-001 — bank | Nonnegative bank augments a potential; if their sum drops by `δ`, total steps are bounded despite temporary potential increases. | Def. 22.4, p.69 | Nonnegative bank and certified combined drop. | Could describe cost accounting, not renderer correctness; **speculative**. |
| CDC-CERT-001 — proof sheet/evidence | Auditable obligations list state, potential, guards, updates, certificates, halt, invariants, parallel safety, schedule, and global bound. | §23.1, p.71 (PDF p.79) | Every invoked obligation must be supplied. | Direct basis for experiment evidence and traces; **direct**. |
| CDC-BISECT-001 — bisection | Sign-change interval width is halved until tolerance. | Ex. 16.13 pp.49; 23.1 p.71 | Continuity and initial sign change. | A canonical repeated transformation, but no direct Mandelbrot classification mapping. |
| CDC-FIXED-001 — fixed-point pattern | Cookbook uses `Φ=|x-x*|` and a local derivative factor for fixed-point iteration. | App. C.1 item 11, p.76 (PDF p.84) | Known fixed point and local contraction condition. | Nearest iterative-dynamics example, yet insufficient to certify `z²+c`; **speculative relevance**. |

## Requested concepts not supported as CDC renderer rules

The source discusses convergence/divergence for sequences and power series (chapters 9 and 19), but does not define a general CDC classification for bounded versus unbounded orbits. It does not define collapse, fractal regions, image partitions/tiles, recursive spatial structures, trace reuse, pruning, or a CDC computational-cost model. “Tiles” are visual/proof components (`Φ,G,U,C,H`), not spatial renderer tiles. Computational bounds in the source are conditional step bounds for specific certificates; they do not establish lower wall time, energy, or renderer cost.
