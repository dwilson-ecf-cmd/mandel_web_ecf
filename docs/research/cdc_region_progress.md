# CDC region progress study

| Potential | Definition/effect and difficult cases | Source relation | Decision |
|---|---|---|---|
| unresolved count | cardinality; subdivision `1 -> 4` increases it | possible nonnegative `Phi`, not source-provided | reject for partitions |
| maximum diameter | max cell diameter; subdividing one non-maximal cell is non-strict and guards may fail | bisection analogy lacks sign-change premises | reject |
| ordered multiset diameter | multiset extension can decrease when a parent is replaced by finitely many strictly smaller children, but says nothing about classification or finite depth | well-founded order is external mathematics | partial structural measure; reject as CDC classification potential |
| certificate depth | construction increases depth | none | reject |
| unresolved guard complexity | no non-circular rank for arbitrary failed guards | none | reject |
| amortized cells/measure | a bank might absorb branching, but no nonnegative bank with fixed drop was proved | Definition 22.4 | unresolved/reject |
| proof-sheet deficit | number of required, typed, independently verified claims absent from a **fixed schema**; Study 0 `1 -> 0` | instantiates token certificate and proof-sheet obligations | support only for the one discharge |

The supported deficit has domain natural numbers, initial value 1, lower bound 0, and strict token drop 1. There is no branching. Failed or numerically uncertain guards leave the state unchanged and are not CDC steps. It does not require the future step count: the fixed schema names the sole terminal claim before execution. Counterexample: counting arbitrary notes or pre-encoding a multi-step proof makes the measure cosmetic; therefore this result does not generalize beyond the independently checkable Study-0 rule.
