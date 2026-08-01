# Refinement-contract extension

Changing allowance 2 to 4 is forbidden mutation. It creates a **superseding new proof-search contract** with a new deterministic identity; it is neither the same proof nor a child obligation. The old contract and every old certificate remain byte-for-byte valid and exhausted under their original identity.

A superseding proof problem may reference prior evidence by old contract identity, certificate identity, obligation identity, and evidence digest. Reuse requires revalidation that mathematical subject, goal, numeric policy, split rules, and evidence premises are compatible. It imports evidence; it does not rewrite ranks, statuses, or lineage history. New obligations are issued under the new identity and receive newly derived ranks.

Native `extend` implements only new-contract creation and rejects non-increasing allowance, unsupported kind, and values beyond the registered v1 bound. Consequently the illustrative 2-to-4 request is rejected by this implementation (4 is unregistered), while 1-to-2 creates a distinct contract. There is no in-place API.
