# Composition rules

## `split_1: obligation(R) -> obligation(A), obligation(B)`

Preconditions are the exact R identity/bounds, active unresolved claim, rank 2 assigned at creation, rule identity, and current evidence. The real split is exactly `17/8`; closed boxes use left-owns-split-excluded/right-owns-split boundary bookkeeping. Conventional checks prove valid nondegenerate children, exact coverage, and disjoint interiors. The local rule consumes allowance 2 and creates A/B at rank 1; `9 > 3+3`. It emits endpoint, coverage, rank, lineage, and token evidence and retires R. Any identity, bound, evidence, rank, potential, or split mismatch rejects atomically and leaves state unchanged.

## `split_2: obligation(A) -> obligation(C), obligation(D)`

Preconditions are exact A, its unchanged rank-1 creation record, active claim, and split_1 lineage. Split at `33/16`; the same convention and conventional proofs establish C/D cover A. The local rule creates both at rank 0; the unchanged B is carried byte-for-byte. Locally `3 > 1+1`, and globally `{A,B}>{C,D,B}` because only A is replaced. Evidence records split_2 and lineage. A rank-zero parent, wrong parent, wrong split, overlap/gap, changed B, absent evidence, or child rank not below A rejects without mutation.

## Composition obligation

Passing rules separately is insufficient. Composition additionally requires matching S1 A identity, immutable earlier ranks/certificate, identical carried B, compatible lineage and boundary conventions, transitive conventional coverage `(C union D) union B = R`, both strict multiset comparisons, and no use of a terminal tree layout. Discharges require separate conventional guards. These checks establish only the registered depth-two trace.
