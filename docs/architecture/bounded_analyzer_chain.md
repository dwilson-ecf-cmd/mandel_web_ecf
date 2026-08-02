# Deterministic bounded analyzer chain

## Baseline audit

The implementation started at commit `e06fc0a62f00a9dc3093d66ba5aa8ef19b70e95d` on branch `work`, with a clean tree. The analyzer lifecycle was `validate` → `begin` → `process` → `finish`, with `abort` after processing failure. Requests and results used fixed caller-visible structures, while each analyzer's state temporarily owned one embedded result. Registry assembly accepted one optional analyzer ID. Runtime validation, job preflight, execution, manifests, serializers, and record serializers all contained explicit zero-or-one assumptions (`count > 1`, index zero, and one-record limits).

The source field is scope-owned by the runtime until rasterization; analyzer state is stack-owned; result records are copied into the caller's `fractal_runtime_output`. The old capacity was one analyzer and one record. Ordering was trivial. Cancellation was checked by computation and within an analyzer, but there was no between-analyzer boundary. A failed analyzer was aborted and no later analyzer ran. The frozen Mandelbrot and Julia field, pixel, artifact, summary, histogram, and spatial fixtures were recorded by the existing native tests before this change and remain the compatibility oracle.

## Ownership and capacity

`FRACTAL_ANALYZER_CHAIN_MAX` is eight. The chain, requests, analyzer state, aggregate result, and up to eight typed records use bounded caller or stack storage; chain execution performs no allocation and uses no mutable static result. Initialization checks the sum of requested record capacities, rejects `size_t` overflow, and rejects totals larger than the supplied/result bound. Each analyzer receives a deterministic contiguous portion of the aggregate record sequence. Aggregate output is assigned to the caller only after every analyzer succeeds, so failure cannot publish a shorter successful chain.

## Ordering and identity

Selections are resolved by module kind and exact module ID through the installed registry. Declaration order is execution and record order. Duplicate IDs, missing IDs, over-capacity chains, unavailable modules, and incompatible field descriptors are rejected. Identity is FNV-1a over canonical semantic text containing chain ABI, count, ordered module IDs and versions, ordered schema IDs and versions, output modes, and record requirements. It never hashes pointers, padding, or raw structures. Canonical serialization uses stable JSON field order, decimal integers, lowercase booleans, explicit identity, and required-size reporting.

## Lifecycle, cancellation, and failure

The complete chain is preflighted before computation via the actual job field descriptor and again before analysis. An empty chain succeeds as an identity-bearing field-preserving observation. One and maximum-size chains follow the same path. Before every analyzer, cancellation is checked. An active analyzer is aborted when processing fails or cancels. Finished analyzers need no cleanup; no analyzers are eagerly prepared, so there are no prepared-but-unfinished later analyzers to abort. Begin, process, finish, preservation, checksum, or capacity failure terminates the chain; later analyzers never run and the caller's previous result remains unchanged. No synthetic record is created.

## Immutable field fan-out

Every analyzer receives the identical `const fractal_field_view`. Copy/transformation output is not supported by a multi-analyzer observational chain. Each returned view must alias the source with the same extent, and every successful analyzer must report the source checksum as its output checksum. Rasterization always receives the original source view. Thus `source_field_checksum == analyzed_field_checksum` is mandatory for every successful supported chain.

## Result and manifest representation

Each aggregate record carries module ID/version, schema ID/version, chain ordinal, payload size, semantic record identity, and explicit status. The aggregate carries chain identity/version, analyzer count, required/produced record counts, accumulated samples examined, source/output checksums, preservation and cancellation flags, and overall status. Runtime and artifact manifests retain concise identity, count, status, checksums, preservation, and typed-record summary fields rather than copying large payloads. Typed payloads remain authoritative in the caller-owned analysis result.

## Limitations and next milestone

Execution is sequential and observational. There is no analyzer parallelism, dynamic loading/allocation, field transformation, scheduling feedback, refinement influence, or formula/raster/palette change. Cancellation uses the shared atomic request token, but a deterministic test hook between analyzers is not exposed publicly.

The scheduler contract and immutable work-unit model now exist. The recommended
next milestone is to define and extract the computation socket and immutable
compute-kernel contract, using the serial and thread-pool schedulers as independent
consumers of the same computation interface.
