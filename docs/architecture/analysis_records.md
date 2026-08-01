# Analysis records

Records are bounded, typed values with numeric type ID, schema version, explicit payload size, canonical identity, and at most 112 payload bytes. Result structures own returned record values. Pipeline order defines deterministic record order, and unknown types or versions are rejected rather than interpreted as an untyped property bag.

The empty sequence serializes as `[]`. Escape-summary Version 1 emits exactly one `fractal.analysis.escape-classification-summary.v1` record with twelve fixed-width counters and an iteration-validity boolean. Its 97 meaningful payload bytes are explicitly encoded, zero-initialized, and decoded field-by-field, so padding and host struct layout cannot affect identity. Canonical JSON always emits every counter in stable order using decimal integers and deterministic booleans. Buffer-too-small calls report the required content length and allocate no memory.
