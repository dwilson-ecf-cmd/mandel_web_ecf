# Repository discovery report (Campaign 1)

Inspection was performed without executing any checked-in executable.

## Findings

* The sole commit tracks 762 files totaling 934,236,910 bytes: three C++ sources, one
  Python script, one HTML file, one CSS file, three ELF executables, 752 BMPs (750 in
  `frames/` plus two root renders), and one MP4. There are no build/configuration
  manifests or other scripts.
* `app_server` is a 56,968-byte executable, not source or a script. ELF metadata says
  64-bit little-endian AArch64, System V ABI, Android dynamic interpreter
  `/system/bin/linker64`, PIE, and Android/Termux runpath. Dynamic dependencies are
  `libc++_shared.so`, `libdl.so`, `libm.so`, and `libc.so`; therefore it is dynamically,
  not statically, linked. Its SHA-256 is
  `3258c1c944adaee9cf60de606b9629f29b04f15b259e3df3556ec6862bdcf749`.
* Its unstripped symbol table names `server.cpp` and `generate_bmp`; strings expose
  `GET /render`, port URL `http://localhost:8080`, HTML/BMP response headers, and no
  usage/options text. These match the tracked `server.cpp` behavior closely, but a
  reproducible build or binary attestation is absent.
* `render` and `render_engine` have the same AArch64 Android/Termux dynamic target.
  `render.cpp` parses named CLI options. `render_engine.cpp` parses positional image,
  HD, and video parameters and writes fixed output paths. `server.py` supplies the
  `/api/render` route expected by `index.html`; `app_server` does not.
* Every numbered frame is an uncompressed 24-bit 854x480 BMP of 1,230,774 bytes. The
  contiguous 0000-0749 naming, dimensions, and `render_engine.cpp` implementation
  conclusively identify these as generated video frames. Root render dimensions also
  match fixed renderer outputs. Their original invocation/spec is not recorded.
* Git has branch `work`, root commit `a12b0ca`, no remotes/tags/other branches, no
  submodules, no ignored files, and no unreachable objects. No release/upstream
  inquiry is possible from this checkout because it has no remote URL.
* The host lacked `file` and `ffprobe`; equivalent executable facts were obtained
  from `readelf`, and BMP facts from Python header parsing. Host `ldd` reported “not a
  dynamic executable” because it cannot load the foreign Android target; `readelf -d`
  is authoritative about its dynamic dependencies.

## Preserved behavior and limitations

No original file was edited, moved, executed, or deleted. Existing binaries and all
render products remain tracked. There is no established build, test, packaging,
manifest, job orchestration, tile API, progressive update, telemetry, or Ouro API.
The existing servers do not implement the same routes, and the Python server's shell
interpolation is unsafe for untrusted input.

The new root CMake build is exclusively for the native Fractal Studio spine; it is not
a recovered build recipe for the inherited binaries. The collaborator's `server.py`
is preserved unchanged and explicitly treated as legacy/reference functionality.

## Questions for the original author

1. Which server (`server.py` or `server.cpp`) and renderer (`render` or
   `render_engine`) are canonical, and what exact Android NDK/Termux build commands
   and flags produced each binary?
2. What exact parameters produced the checked-in BMP/MP4 set, and should it become a
   named golden fixture, release asset, or external archive?
3. Is there an upstream URL, missing branch/release, or manifest containing history,
   licenses, build metadata, and artifact provenance?
4. Which formula/palette set and coordinate/precision semantics are compatibility
   requirements, especially where the two renderer sources differ?
5. Is the public HTTP interface intended to be `/render` or `/api/render`, and is the
   Python service intentionally local-only?

## Recommended Campaign 2

Obtain provenance/build answers first. Then add a reproducible, host-supported C++
build without altering math; capture a very small, explicitly specified golden BMP
fixture and checksum; implement an in-process or carefully bounded subprocess adapter
behind `fractal_renderer_vtable`; map validated `fractal_render_spec` values explicitly
(never shell concatenation); and produce a versioned artifact manifest using system
memory only.
Defer queues, Ouro, progressive transport, and dashboard redesign until parity and
the single-job lifecycle are proven.
