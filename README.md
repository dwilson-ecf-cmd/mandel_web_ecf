# Mandelbrot Studio / Fractal Studio spine

This repository preserves a collaborator's Mandelbrot Studio: C++ fractal renderers,
a small legacy Python HTTP launcher, a browser UI, prebuilt Android/Termux executables,
and example/generated render output. The Fractal Studio production spine is native
C11, with inherited rendering isolated behind a C ABI implemented in C++ where
necessary. It does **not** replace the renderer or change its pixels.

## Verified repository contents

The original commit contains no build file or dependency manifest. Its executable
source is `render.cpp` (a CLI BMP renderer), `render_engine.cpp` (image, HD, and video
modes), and `server.cpp` (a minimal HTTP server). `server.py` is a separate static-file
server whose `/api/render` handler invokes `./render_engine`. `index.html` and
`style.css` provide the Russian-language Mandelbrot Studio UI.

The tracked `app_server`, `render`, and `render_engine` files are compiled 64-bit,
little-endian AArch64 Android position-independent ELF executables. They are
dynamically linked and request Android/Termux libraries, including
`libc++_shared.so`, `libdl.so`, `libm.so`, and `libc.so`; they are not portable Linux
x86-64 executables. Static symbols and strings associate `app_server` with
`server.cpp`, but no reproducible build recipe is present, so source/binary identity
is not asserted. `app_server` has SHA-256
`3258c1c944adaee9cf60de606b9629f29b04f15b259e3df3556ec6862bdcf749`.

`server.cpp` binds all interfaces on port 8080, returns a newly generated 600x400 BMP
for request text containing `GET /render`, and otherwise serves `index.html`.
It has no command-line options. By contrast, the browser calls `/api/render`; that
route is implemented by `server.py`, which binds `127.0.0.1:8080`, shells out to
`render_engine`, and relies on Python 3 plus the platform-compatible renderer. This
original launcher is preserved as legacy/reference functionality; it is not part of
the native production spine.
Video mode also requires `ffmpeg`. The Python handler interpolates request values
into a shell command without quoting, so it must not be exposed to untrusted clients.

The root `render.bmp` (800x600), `render_hd.bmp` (1920x1080), and `render.mp4` are
render products. All 750 tracked `frames/frame_0000.bmp` through
`frames/frame_0749.bmp` are 854x480, uncompressed 24-bit BMPs. Their exact name and
dimensions match `render_engine.cpp` video generation, which deletes/recreates
`frames/` and feeds the sequence to `ffmpeg`. They are therefore confirmed generated
outputs retained as collaborator artifacts, not source inputs. New job output belongs
under `runtime/artifacts/`; precise ignore rules prevent accidental additions while
leaving tracked files intact.

Git discovery found one local branch and one root commit, no configured remote, no
remote branches or tags, no submodule declaration, no ignored source, and no
unreachable Git objects. Consequently this checkout provides no evidence about an
upstream repository, releases, other branches, or older source.

## Safe development and validation

Static inspection (`readelf`, `strings`, checksums, and BMP header parsing) is safe.
Do not run the checked-in binaries unless using a suitable, isolated AArch64 Android/
Termux environment with the required libraries and after reviewing their source.
Native tests open the three checked-in executables only to verify their ELF magic;
they never create a process from `app_server` or either renderer binary.

The native spine requires a C11 compiler, a C++17 compiler for the inherited-renderer
adapter boundary, and CMake 3.16 or newer. It does not require Python. Configure,
build, and run its tests from the repository root:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

These commands build only new native source. They neither compile nor execute the
checked-in Android binaries. No supported build command for those inherited binaries
can be documented until the original author supplies compiler/linker flags and target
environment.

## Existing versus planned functionality

**Existing:** synchronous local BMP rendering, an `ffmpeg` video path, a static UI,
and two simple HTTP implementations. There is no demonstrated job queue, progressive
tile transport, manifest, durable storage, worker pool, telemetry system, or Ouro
integration.

**New production spine:** C headers and implementations under `shared/` define the
neutral model; `server/renderer/` defines a C renderer contract and C++ adapter stub;
and `runtime/` supplies explicit native memory scopes. The system allocator is the
functional reference/default backend. Ouro returns `FRACTAL_ERROR_NOT_IMPLEMENTED`
and remains a future optional adapter. The inherited renderer is not yet mapped into
the tile contract, so the C++ adapter deliberately returns the same result rather than
inventing a behavior-changing translation.

**Planned:** native API/server and scheduler implementations, a verified C++ renderer
mapping, artifact manifests, telemetry, and UI expansion. Any future Ouro-backed
render must preserve pixel-checksum parity with the system backend.

Current limitations and open provenance questions are recorded in
`docs/repository_discovery.md`; model semantics are in
`docs/fractal_studio_model.md`.

## Socketable native runtime

The native vertical slice now assembles formula, numeric, compute, refinement,
scheduler, raster, encoder, memory, telemetry, and platform sockets before work
begins. Mandelbrot and Julia share the same binary64 scalar compute, row-major
scheduler, formula-independent rasterizer, and deterministic BMP encoder. The
legacy APIs remain compatibility boundaries. Architecture, module identities, and
golden checksums are recorded in `docs/architecture/socketable_runtime.md`.

Scalar arithmetic is now owned by the versioned
`fractal.numeric.binary64.v1` contract. Computation retains recurrence order,
field writes, assignments, and cancellation but routes every real and complex
operation through that numeric socket. The Binary64 contract, replay metadata,
and frozen identities are recorded in
`docs/architecture/binary64_numeric_socket.md`.

## Current product direction

The future target is a dedicated Android application, not Termux. Existing Android/Termux
executables are preserved as historical, inert reference artifacts. The portable core has
explicit, independent renderer and memory backend identities: the inherited C++ behavior
remains the renderer reference, CDC is registered but unavailable, system memory remains
the default, and Ouro remains optional and unavailable. See `docs/android_architecture.md`
and the CDC traceability documents. Native configure/build/test does not invoke Python or
checked-in foreign executables.


## Computation substrate

Fractal mathematics now has a portable point-computation boundary independent of
rasterization and memory selection. `conventional-c` is the narrow binary64 oracle for
Experiment 0; `cdc-experimental` returns a traceable unresolved result and requests
fallback without asserting a CDC Mandelbrot method. Manifests identify computation,
renderer, and memory separately. The former CDC renderer identity remains only as an
unavailable transitional compatibility name. See `docs/computation_architecture.md`.
