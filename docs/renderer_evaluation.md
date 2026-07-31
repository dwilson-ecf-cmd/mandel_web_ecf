# Renderer metrics, manifests, and evaluation

Backend-neutral metrics identify computation, renderer, and memory backends independently; pixels, tiles, frames; maximum per-point steps; escaped/bounded/unresolved/failed points; duration; precision; retries; cancellation latency; active/peak memory; and output/artifact checksums. Conventional iterations and CDC descent steps have distinct counters because they are not semantically equivalent.

CDC counters are limited to defined experimental observations: states evaluated, certified descent steps, conditional branches, terminal/unresolved classifications, fallback count, and evidence depth. Trace reuse, pruning, and region decisions are excluded until defined and validated.

A deterministic manifest carries RenderSpec, backend identities, app/renderer revision, compiler/target, dimensions/pixel format/precision/palette through RenderSpec, checksums, duration/metrics, equivalence expectation, and notes. Manifests with CDC computation capture the authoritative PDF SHA-256. Identity JSON uses a fixed key order.

“Cheap” must be evaluated across CPU time, sustained throughput, memory/capacity/bandwidth, branch behavior, vectorization potential, battery, thermal throttling/device temperature, energy per pixel/classified point, output quality/correctness, and fallback rate. Wall clock alone is insufficient. Android energy/thermal data remains future work pending a real-device harness; no estimate is asserted.
