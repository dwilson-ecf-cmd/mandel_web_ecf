# Scheduler and immutable work-unit contract

The scheduler owns deterministic ordering, execution sequencing, cancellation boundaries, and the lifetime of one execution. It does not own formula mathematics, numeric or compute algorithms, analysis, rasterization, encoding, or artifact storage. Those operations remain sockets selected before a work unit is sealed.

`fractal_work_unit` schema version 1 snapshots every selected module identity and version, the analyzer-chain identity, view and output description, formula parameter bytes, cancellation token, and a deterministic identity. Construction is the sole mutation point; callers and schedulers receive it as `const` thereafter. Empty or incomplete requests are rejected rather than scheduled.

The canonical `fractal.scheduler.serial.v1` implementation executes exactly one work unit in this order: prepare, scheduling-compatibility validation, compute, analyzer chain (with a cancellation check between analyzers), renderer, transactional artifact sink, publication. A cancellation is honored before scheduling, before/during compute, after compute/before analysis, between analyzers, before rendering, and before publication. An aborted execution never commits a partial successful artifact.

Compatibility validation compares declared socket identities, versions, chain identity, capabilities, and immutable request shape. The scheduler does not inspect formula mathematics or numeric implementation details.

Outcomes are explicit: malformed or incompatible selections are `rejected`; cancellation is `cancelled`; phase errors are attributed to compute, analysis, renderer, or sink; only a committed, checksum-verified artifact is `published`. Sink transactions are aborted on upstream failure. Sequence identifiers are logical and deterministic, not wall-clock timestamps.

The serial scheduler is authoritative because it has no timing-dependent ordering and is therefore the checksum and lifecycle oracle against which future schedulers must be tested.

## Next milestone

Introduce deterministic parallel scheduler implementing the same scheduler contract.
