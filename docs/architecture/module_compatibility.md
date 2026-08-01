# Module compatibility

Assembly first validates every descriptor's ABI, version, kind, stable ASCII ID, and
lifetime-bearing pointers. It then resolves the small capability chain: formula and
compute require scalar arithmetic; conventional compute consumes point-scalar
formulas; serial scheduling produces an iteration field; native raster consumes that
field and produces BGR8; BMP consumes BGR8; system memory supplies scopes.

Capability masks describe data contracts, not quality or mathematical truth. Formula
parameters and selected IDs receive a second validation at job assembly. CDC
refinement and Ouro memory have descriptors so selection fails explicitly with
`NOT_IMPLEMENTED`; neither is silently replaced. The initial statically linked model
does not perform dynamic discovery or negotiate versions.

BGR8 means three consecutive bytes in blue, green, red order and has no machine-word
endianness dependency. The raster buffer is top-down and tightly packed. The BMP
encoder emits a bottom-up BMP v3 stream and adds deterministic zero row padding.

## Registry and artifact output

Registry initialization rejects malformed IDs, null descriptors or implementations, unsupported ABI, invalid kinds, and duplicate IDs within a kind. Registry-driven assembly resolves every required typed selection and rejects unavailable entries before runtime validation. Stream BMP advertises `ENCODER_STREAM_OUTPUT`; sinks advertise binary bytes, commit/abort, and exactly the destination classes they implement. Artifact-sink compatibility is checked at the socket boundary rather than overloading encoder or platform identity.

## Analyzer compatibility

`ANALYZER` is resolved independently by kind plus stable ID. The installed pass-through module advertises only compatible-field production, exact value preservation, and cancellation. Assembly rejects missing, wrong-kind, unavailable, ABI-invalid, or descriptor/implementation-mismatched analyzers. The concrete iteration/classification descriptor is validated before analysis and raster consumes only the final compatible view.

The escape-summary module additionally requires whole-field processing and one record slot, and advertises record production without claiming streaming rows. Its request rejects copy mode, missing preservation/record requirements, or zero capacity. Unknown field formats/versions and unknown classification tags are invalid.
