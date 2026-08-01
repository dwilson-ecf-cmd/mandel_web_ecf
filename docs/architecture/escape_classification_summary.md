# Escape and classification summary Version 1

`fractal.analyzer.escape-classification-summary` consumes an immutable `FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1` view, returns that exact borrowed view, and emits one `fractal.analysis.escape-classification-summary.v1` record. It never allocates, mutates the field, rasterizes, encodes, or writes artifacts.

## Classification and iteration semantics

Every sample must have one declared tag: escaped, bounded, unresolved, cancelled, or failed. Unknown tags reject the field. Conservation is `escaped + bounded + unresolved + cancelled + failed + other_classified == samples_total`; Version 1 always reports `other_classified` as zero. `maximum_iteration_samples` is a derived subset equal to unresolved samples, not an additional conservation category: under this field format a successfully unresolved conventional sample consumed its configured budget.

Escaped steps are the one-based iteration at which escape was detected. Unresolved steps are the consumed budget. Bounded is representable and its completed steps are eligible, although the current producer does not emit it. Escaped, bounded, and unresolved contribute to exact iteration count/min/max/sum. Cancelled and failed partial work do not. Iteration zero is representable and included without reinterpretation. If no eligible samples exist, count/min/max/sum are zero and `iteration_statistics_valid` is false. No floating-point average is stored because consumers can divide exact sum by exact count; Version 1 deliberately has no histogram.

## Determinism, overflow, cancellation, and ownership

The analyzer checks cancellation before processing and once before each row, then scans each row left-to-right. Cancellation or any invalid/overflow condition produces no completed record and prevents rasterization. Width/height storage arithmetic, every counter, and iteration sum are checked; values never saturate. Record bytes use explicit little-endian fixed-width payload encoding, while identity is FNV-1a over canonical JSON. FNV-1a is a reproducibility hash, not a cryptographic digest. Serialization uses stable field order, decimal integers, explicit booleans, required-size reporting, and no heap allocation or struct padding.

The runtime owns source storage. The analyzer borrows it, returns the same address and descriptor, and guarantees source and analyzed checksums match. The result structure owns the bounded record value. Manifests retain a concise subset plus schema and record identity; callers can retrieve and canonically serialize the complete record from `fractal_analysis_result`.

## Limitations

This is an aggregate instrument only. It proves no boundedness theorem and contains no histogram, spatial grouping, orbit, distance, periodicity, or CDC evidence.
