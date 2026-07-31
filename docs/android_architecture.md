# Dedicated Android architecture

Termux is historical only. Checked-in AArch64 Android/Termux executables are preserved reference data, not supported targets and never part of the production build.

```text
Dedicated Android application
  -> Kotlin/Android application layer
  -> narrow JNI adapter
  -> portable native C core
       +-> computation backend (conventional-C / CDC experimental)
       +-> renderer / rasterizer (CDC renderer identity is transitional)
       +-> system memory (reference/default)
       +-> optional embedded Ouro (currently unavailable)
```

## Responsibility boundaries

The **Kotlin/Android layer** owns activities, composables/views, lifecycle, input, presentation, touch/zoom/pan, settings, permissions, foreground/background-service coordination, artifact browsing, save/export/share, progress/diagnostics, localization, and accessibility.

The **JNI adapter** may create/destroy a native runtime; submit immutable render specifications; select renderer and memory backends independently; cancel; forward progress snapshots/callbacks; retrieve artifacts; convert stable errors; and forward lifecycle events only where necessary. It must not own fractal or CDC mathematics, allocator/scheduling policy, artifact-format policy, or business logic. A stub is deferred until an NDK build can test it; avoiding a speculative broad JNI API better proves the narrow boundary.

The **portable C core** owns spec validation, job lifecycle, renderer selection, memory scopes, telemetry, manifests, diagnostics, backend-neutral scheduling contracts, and cancellation state. It has no Android UI dependency and must build/test on a host.

Likely additive structure (not a migration plan): `android/app/` for Kotlin and resources, `android/native/` for the JNI-only target, with existing `shared/`, `runtime/`, and `server/` remaining portable.

## Independent axes

Computation (`legacy-reference`, `conventional-c`, `cdc-experimental`), renderer, and memory (`system`, prospective `ouro`) identities are selected independently. The CDC renderer identity is transitional and unavailable; CDC mathematics belongs exclusively to the computation substrate.

System memory stays default. Ouro remains `FRACTAL_ERROR_NOT_IMPLEMENTED` and may later provide scoped job/frame/tile lifetimes, allocation telemetry, diagnostics, lifecycle accounting, failure evidence, and Android-native embedding. Allocator evidence cannot establish renderer correctness, and CDC policy cannot enter the allocator.

## Legacy rendering gap

The inherited C++ renderer writes whole BMP frames/files using its own lifecycle. The C contract describes job, frame, and tile calls with caller-owned pixel buffers. There is no validated mapping among inherited pixel layout/palette/precision, tile calls, cancellation, and whole-frame output; nor from buffers to Android Bitmap/texture presentation. The adapter therefore validates a spec but returns `NOT_IMPLEMENTED` for rendering. This milestone neither rewrites formulas nor falsely forces the whole-frame renderer into tiles.
