# Two-child failure and stalling cases

| Case | Required behavior |
|---|---|
| one child guard true, other unresolved | accept only the certified child's discharge; retain the other obligation and return `UNRESOLVED`; parent unresolved |
| neither child certifiable | retain both and return `UNRESOLVED`; no CDC discharge |
| rounding/numeric uncertainty blocks a guard | do not guess; leave that obligation and potential unchanged |
| invalid or degenerate child | reject split and retain parent |
| gap, wrong outer endpoint, or wrong boundary ownership | reject split and retain parent |
| closed claim domains overlap at split contrary to convention | reject; closed evidence enclosures may share the boundary, claims may not |
| child not strictly lower-ranked or potential does not drop | reject split even when geometric union is correct |
| proof sheet grows with notes but no registered evidence/rule | no transition and zero CDC steps |
| valid split but unavailable child proof | split may remain at potential 2, then stalls; fallback is allowed but not fabricated progress |

A partially discharged state is valid and auditable, not terminal. Retrying an uncertain guard is conventional work and is not counted. The system never removes the structural parent provenance, but it retains the active parent obligation whenever split certification fails.
