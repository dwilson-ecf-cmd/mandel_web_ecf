# CDC glossary

Only source-grounded canonical terms appear here.

| Term | Repository-facing meaning | Source | Caution / do not conflate |
|---|---|---|---|
| CDC configuration | `(x, Φ, G, U, H)`. | Def. 5.1, printed p.12 | Not a render job, pixel orbit, or optimizer by default. |
| state (`x`) | Member of the configuration's state space `X`. | Def. 5.1, p.12 | The PDF gives no universal storage representation. |
| potential (`Φ`) | Nonnegative function on state used to measure progress. | Def. 5.1; App. A, pp.12, 74 | Not arbitrary telemetry, energy consumption, escape magnitude, or objective without proof. |
| guard (`G`) | Predicate enabling a rule. | Def. 5.1, p.12 | Not a UI conditional or Mandelbrot escape test automatically. |
| update (`U`) | State-transition rule. | Def. 5.1, p.12 | Repetition alone does not make an update CDC-certified. |
| halt predicate (`H`) | Predicate representing a reached goal; no enabled guard can also stop the process. | Def. 5.2; App. A, pp.12, 74 | Not necessarily convergence, boundedness, or escape. |
| certificate | Local token/factor/affine guarantee on the potential. | Def. 5.2; §23.2, pp.12, 71 | A logged assertion is not evidence that the inequality holds. |
| token step | `Φ'≤Φ−δ`, `δ>0`. | Def. 5.2, p.12 | Do not conflate with an ordinary iteration counter. |
| factor step | `Φ'≤cΦ`, `0<c<1`. | Def. 5.2, p.12 | Requires the inequality and preconditions, not observed average shrinkage. |
| invariant | Predicate initially true and preserved by enabled rules. | Def. 20.3, p.65 | Safety evidence, distinct from progress potential. |
| nondeterministic choice | Selection of any guarded rule whose guard is true. | Def. 20.1, p.65 | Not randomness necessarily. |
| parallel batch | Updates in one logical tick, with disjoint-write or commutativity requirement. | Def. 20.2, p.65 | Not an image tile and not automatically thread-safe. |
| schedule / weak fairness | Rule-index sequence / condition ensuring perpetually enabled rules recur. | Defs. 22.1–22.2, p.69 | No direct computational interpretation until a renderer rule set exists. |
| bank (banker method) | Nonnegative amortization credit combined with a potential. | Def. 22.4, p.69 | Not memory allocation or an Ouro arena. |
