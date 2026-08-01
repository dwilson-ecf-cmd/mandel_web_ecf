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
