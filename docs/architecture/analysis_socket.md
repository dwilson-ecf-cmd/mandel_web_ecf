# Analysis socket

Analysis is an independent registered module axis downstream of completed computation and upstream of rasterization. The runtime owns source and optional copied output storage; analyzers borrow immutable input views and never free fields. The reference `fractal.analyzer.passthrough` either borrows input or copies it, preserves descriptor and bytes exactly, emits zero records, supports boundary cancellation, and reports exact sample and checksum metadata.

The lifecycle is `validate -> begin -> process -> finish`; a failure invokes `abort` and prevents rasterization. A state may be used once. Descriptors and vtables have static lifetime. Empty dimensions are rejected. Input/output aliasing is allowed only for exact alias; partial overlap is rejected. Processing is deterministic row-major byte preservation. Future analyzers may accumulate whole-field statistics or emit records, but must state whether they preserve field semantics.
