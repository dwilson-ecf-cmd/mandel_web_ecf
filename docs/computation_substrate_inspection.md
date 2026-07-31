# Computation substrate preliminary inspection

This inspection was recorded before implementation of the computation substrate.

## Evidence reviewed

The architecture, Android boundary, CDC reference index/glossary/ledger, renderer evaluation, and Experiment 0 documents were read together with the renderer vtable, backend factory, CDC renderer stub, render metrics, manifest implementation, CMake graph, and native tests. `CDC.pdf` remains present with SHA-256 `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

## Findings

1. `fractal_renderer_backend_kind` identifies `CDC_EXPERIMENTAL`, the renderer factory constructs a CDC renderer, and the manifest decides CDC involvement from renderer identity. This incorrectly makes CDC primarily a rasterization concern.
2. CDC model, metric, and trace headers live below `server/renderer/cdc/`; although they contain no implemented mathematics, their location implies renderer ownership.
3. Renderer metrics contain conventional and CDC work counters. Those counters describe point computation and need an independent computation identity even if aggregate render telemetry later copies them.
4. Experiment 0 is already one-point and explicitly unresolved, but it lacks an executable conventional computation oracle and required mapping outcome categories.
5. The renderer vtable is frame/tile/pixel oriented. It is unsuitable as the mathematical point interface and must remain separate rather than being expanded with orbit semantics.
6. The C++ renderer and CDC renderer both intentionally return `FRACTAL_ERROR_NOT_IMPLEMENTED`; legacy source, Python/browser references, binaries, frames, and generated artifacts remain preservation obligations.

## Narrow implementation decision

Add a portable C point-computation contract with legacy-reference, conventional-C, and CDC-experimental identities. Put the executable conventional oracle and honest unresolved CDC backend behind that contract. Retain the CDC renderer as a transitional compatibility identity that owns no CDC mathematics. Extend manifests to record computation, renderer, and memory independently and derive CDC reference metadata only from material CDC computation involvement.
