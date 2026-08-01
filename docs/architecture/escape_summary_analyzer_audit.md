# Escape and classification summary analyzer audit

Starting commit: `7895df7` (`Establish analysis pipeline and pass-through
analyzer`). The `work` checkout was clean, had no configured remotes, and
contained the `ANALYZER` kind, registered pass-through analyzer, descriptor and
view model, request/result/record types, zero-or-one pipeline, registry-driven
selection, split source/analyzed checksums, and preserved Mandelbrot and Julia
goldens.

## Source facts

`fractal_point_result_compact` is eight bytes: a 32-bit unsigned `steps`, an
8-bit `classification`, and three reserved bytes. Fields are visited in
deterministic row-major order and may have row padding described by `stride`.
The declared classifications are escaped, bounded, unresolved, cancelled, and
failed. No separate maximum-iteration classification exists.

Conventional computation initializes a sample to unresolved with zero steps,
then stores the one-based iteration after each completed step. Escaped therefore
stores the exact iteration at which escape was detected. A successfully
completed unresolved sample has consumed the entire configured budget and its
steps equal that budget; within fields produced by this runtime it is also a
maximum-iteration sample. Bounded is representable but not produced by the
current conventional formula path. Cancelled and failed can contain partial
work and therefore are not eligible for completed iteration statistics.

The field alone contains no configured budget identity, orbit, coordinates,
intermediate values, escape magnitude, distance, periodicity evidence, region
connectivity, or CDC evidence. It can exactly provide classification totals;
the unresolved/max-budget relationship relies on the documented semantics of
the existing iteration/classification field format, not a new inference by the
analyzer.

## Existing analysis seam

The lifecycle is validate, begin, process, finish or abort. Input is immutable;
pass-through supports exact borrowing or copying. Cancellation is checked
before pass-through processing. Analyzer state and output metadata are owned by
the caller, while runtime scope owns field storage. The current fixed record
payload is 64 bytes and serialization accepts only zero records, so it must be
extended for eleven fixed-width counters without serializing struct padding.
The pipeline supports one analyzer and manifests currently carry analyzer,
pipeline, checksum, preservation, sample, record-count, and result metadata.

## Derivable Version 1 statistics

Without computation changes the analyzer can report total, escaped, bounded,
unresolved, maximum-iteration (the unresolved subset), cancelled, failed, and
other-known classification counts; completed iteration sample count; exact
minimum, maximum, and sum; and an explicit validity flag. Unknown numeric
classification tags violate the field contract and will be rejected rather
than counted as legitimate data. Classification conservation excludes the
derived maximum-iteration subset to avoid double counting. With no eligible
samples, min/max/sum are zero and the validity flag is false. No average or
histogram is derivable as a stored Version 1 result by policy, and no
substantive spatial or mathematical conclusion is justified.

## Implemented identities and frozen fixtures

Adding the second analyzer changes the authoritative registry identity to
`6844c8478833c246`. The canonical Version 1 record identities are
`2820bd77cffceeef` for the 32x24 Mandelbrot fixture and `5eabac4db10ecc82`
for Julia. FNV-1a provides reproducibility identity only, not cryptographic
integrity.
