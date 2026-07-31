# Fractal Studio architecture

## Intended flow

```text
Fractal Studio UI
        |
        v
Render API
        |
        v
Job Scheduler
        |
        v
Renderer Backend
        |
        +--> Worker Pool
        +--> Artifact Storage
        +--> Application Telemetry
        +--> Optional Ouro Memory Backend
```

The production side of every layer through the renderer interface is C11. C++ is
limited to the inherited-renderer adapter, whose public header is a C ABI. Python is
not a production dependency; the collaborator's `server.py` remains a legacy
launcher. This is otherwise a target boundary diagram, not a claim that the native
API or scheduler exists. The API accepts and returns model records; the scheduler owns job
state and cancellation; workers receive immutable specifications and bounded tile
assignments; the renderer alone owns fractal calculation; storage publishes an
artifact only after checksum and manifest creation. Partial frames may later be
published as explicitly provisional events.

`fractal_renderer_vtable` is a C lifecycle contract: initialize, validate a specification,
begin a job, render a tile, finish a frame, finish or cancel a job, and shut down.
The C++ adapter currently validates neutral specifications but returns
`FRACTAL_ERROR_NOT_IMPLEMENTED` for rendering. The inherited implementation renders
whole BMP files while the new seam consumes tiles and caller-owned pixel buffers; its
formula/palette variants and pixel layout also need an author-approved mapping. A
future adapter must preserve formulas, coordinate mapping, byte layout, and golden
pixel checksums rather than copying equations into nominal C.

The native memory lifecycle is similarly narrow: initialize, begin a caller-owned
scope, allocate, reset/end that scope, report statistics, and shut down. The system
backend maintains a linked allocation ledger and frees every allocation at reset/end;
it does not claim arena behavior. It is the reference implementation and default. An Ouro adapter may be introduced only against
a stable embedded API; it must be optional, selected explicitly, and parity-tested
against system-backed renders. Application correctness must never be inferred from
allocator counters, and Ouro code is not vendored here.

## Telemetry boundaries

* **Application telemetry** describes API requests, job state transitions, queue
  depth, artifact publication, and end-to-end latency. It is authoritative for job
  state.
* **Renderer telemetry** describes frame/tile calculation rates, precision mode, and
  renderer failures. It does not decide scheduler state.
* **Worker telemetry** describes heartbeats, assignments, retries, health, and work
  completed. A missing heartbeat is evidence for the scheduler, not an allocator
  failure.
* **Allocator/runtime telemetry** describes allocation counts, bytes, scopes, and
  runtime errors. It is diagnostic only and remains absent with backends that cannot
  provide a metric.

Metrics should carry stable identifiers but avoid duplicating entire render specs or
high-cardinality diagnostics. Estimated completion remains unknown until enough
observations exist; the model therefore makes it optional.

## Reproducibility and ownership

A job references one immutable, canonically serialized `RenderSpec`. An artifact
manifest should eventually record that serialization, renderer identity, backend
selection, media metadata, and checksums. Scheduler state, renderer progress, and
allocator state have separate owners. Generated data goes under `runtime/artifacts/`
or an external destination and is never treated as source.
