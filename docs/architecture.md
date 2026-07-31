# Fractal Studio architecture

## Intended flow

```text
Android application / future API
        -> immutable problem and render specifications
        -> computation backend
        -> backend-neutral point or region results
        -> renderer / rasterizer
        -> artifact storage or platform presentation
```

The portable native side is C11. C++ is limited to the inherited-renderer adapter behind a C ABI. Python remains a preserved legacy launcher, not a production dependency. Computation owns formula transitions, classifications, work counters, and mathematical evidence. Rendering owns rasterization and presentation output, not fractal or CDC mathematics. Scheduling, artifact publication, application telemetry, renderer telemetry, computation evidence, and allocator diagnostics remain separate responsibilities.

The current renderer vtable is a job/frame/tile/pixel lifecycle. The inherited adapter remains unavailable because whole BMP behavior has no validated mapping to caller-owned buffers. The transitional CDC renderer is also unavailable and owns no mathematics. See `computation_architecture.md` for the point substrate.

System memory remains the reference/default. Ouro is optional and unimplemented. Computation identity never selects memory, allocator behavior never proves computation correctness, and render output never proves allocator correctness.

## Reproducibility

A manifest independently records computation, renderer, and memory identities, along with immutable RenderSpec and checksums. CDC reference checksum metadata appears only when CDC computation is selected. Generated data belongs under `runtime/artifacts/` or external storage and is never source.

## Platform direction

The future product is a dedicated Android application with Kotlin presentation, a narrow JNI adapter, and the host-testable portable native core. Termux is historical only; checked-in executables remain inert reference artifacts. No Android application or JNI implementation is part of this milestone.
