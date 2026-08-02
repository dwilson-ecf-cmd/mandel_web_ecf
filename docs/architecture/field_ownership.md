# Field ownership

`fractal_field_descriptor` separates dimensions, byte stride, format, and flags from storage. `fractal_field_view` is immutable and `fractal_mutable_field_view` makes writes explicit. Validation rejects zero dimensions, unknown format, narrow stride, multiplication overflow, null storage, and insufficient backing size. The scheduler's legacy `fractal_field` remains an allocation-oriented compatibility object.

Runtime scope owns source, optional analysis copy, and pixels and releases each once after encoding or failure. The scheduler passes a caller-owned mutable field view plus a sealed disjoint assignment to the computation module; neither owns or releases storage. Analysis immutably borrows source and may borrow caller-owned output; raster immutably borrows the final analyzed view. Borrowed fields are never released by analyzers. Partial computation progress is represented only in assignment results until the scheduler finalizes `completed_rows`; analysis and raster accept only the successfully finalized source in orchestration.

The escape-summary analyzer only borrows the immutable source view and returns it unchanged. Its fixed record is copied into caller-owned result storage; no field or record heap allocation occurs. Failed or cancelled processing exposes no completed record.
