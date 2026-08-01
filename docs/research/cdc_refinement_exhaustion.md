# Refinement-contract exhaustion

The states are intentionally disjoint:

* `UNRESOLVED`: no classification proof yet; search may be idle or no policy requested a stronger disposition.
* `CONTRACT_EXHAUSTED`: unresolved, rank zero, and this contract's permitted refinements are consumed.
* `FALLBACK_REQUIRED`: unresolved under this contract and application policy requests a distinct engine.
* `FAILED`: malformed/unsound certificate or violated proof premise; never used merely for exhaustion.
* `NUMERICALLY_UNCERTAIN`: required numeric proposition cannot be certified under the numeric policy; distinct from resource exhaustion.

Rank zero permits verified conventional discharge and produces `DISCHARGED`; it forbids another split. A guard may always be attempted at zero because discharge removes the obligation and does not consume refinement allowance. A fallback engine may continue only under its own contract/proof rules. The exhausted certificate remains valid evidence of all earlier transformations plus an explicitly incomplete open obligation; it is not a complete classification proof.

Exhaustion is operational/proof-search-contract output, not a mathematical claim that the region is inside, outside, or unclassifiable. Native policy maps a failed zero-rank guard to `UNRESOLVED`, `CONTRACT_EXHAUSTED`, or `FALLBACK_REQUIRED`. Numeric uncertainty and failed validation are produced by their own causes, never inferred from rank zero.
