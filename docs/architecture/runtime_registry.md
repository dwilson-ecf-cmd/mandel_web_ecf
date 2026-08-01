# Runtime registry

`fractal_module_registry` is caller-owned, bounded state initialized from module-owned descriptors and implementation contracts. Initialization validates every descriptor, ABI, kind, identifier, implementation pointer, and kind-local ID uniqueness, then copies registrations and sorts by `(kind, module_id)`. Thus input pointer order and addresses do not affect lookup, iteration, serialization, or identity. The same ID in different kinds is legal and unambiguous.

`fractal_installed_modules_registry` is the one authoritative static catalog. It registers Mandelbrot and Julia formulas, binary64 numeric, conventional scalar compute, none and unavailable CDC refinement, serial scheduling, native BGR8 rasterization, BMP v3, system and unavailable Ouro memory, no-op telemetry, host platform, and bounded-memory and host-file sinks. It claims no dynamic discovery or plugin loading.

Registry identity is FNV-1a over canonical kind, ID, ABI, implementation version, availability, and capability metadata. It is a deterministic reproducibility identifier, **not** a cryptographic digest. Inspection APIs expose totals, kind counts, indexed descriptors, typed lookup, availability/capabilities through descriptors, implementation resolution, identity, and deterministic JSON without exposing the internal array.

The authoritative assembly API accepts typed IDs and resolves implementation contracts through a supplied registry. Missing and unavailable selections fail before execution. Direct-pointer validation remains for low-level callers.
