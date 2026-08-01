# Artifact lifecycle

The runtime validates and computes field data, rasterizes pixels, begins the caller-selected sink, and streams BMP header, bottom-up rows, and deterministic zero padding. Successful encoding is not an artifact until sink commit succeeds. Any encoding failure invokes abort. A failed commit sets no valid committed result and the file sink removes its temporary file.

`fractal_artifact_result` separates encoder identity, sink identity, logical name, media identity, byte count, content checksum, pixel checksum, committed state, destination class, optional file path, and result code. Paths never enter field models and do not contribute to content identity.
