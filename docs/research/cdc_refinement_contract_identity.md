# Refinement-contract identity and immutability

## Canonical identity

V1 canonical fields, in order, are: schema/version; contract kind; target endpoint IEEE-754 bit patterns; maximum depth; minimum-real-width bit pattern; fixed token allowance; exhaustion policy. Unused alternatives serialize as zero. Classification goal, numeric policy, and split rule are fixed by the narrow registered version and therefore version-bound; a later version that makes them selectable must include them explicitly.

Serialization is compact JSON with fixed field order, decimal enums/integers, lowercase fixed-width hexadecimal bit patterns and identity. Identity is deterministic 64-bit FNV-1a over a typed canonical byte sequence. It is a stable mismatch detector, **not** a cryptographic security digest. Repository provenance continues to use SHA-256 for `CDC.pdf`.

Contract validation recomputes identity; modifying any canonical field without issuing a new identity rejects. Mutable data—queue state, obligation status, accumulated evidence, runtime counters, scheduler order, memory use, and timestamps—lives outside the contract.

## Inheritance and lineage

Children inherit the exact same contract identity. Their regions and lineage depth are new immutable obligation creation fields; rank is derived and stored at creation, then replay validation recomputes it. V1 has no derived subcontract operation. A future subcontract must record parent contract identity, derivation rule, canonical parameters, and its own identity. Silent mutation, identity reuse after changed bytes, and retrospective rank rewrite are invalid.
