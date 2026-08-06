# English maintainer guide

Russian is the authoritative language for current human-facing technical
documentation and source comments. This guide keeps the English-speaking
maintainer fully able to build, validate, and review the repository.

## Repository layout

- `shared/` contains portable C11 contracts, data models, registry code, and
  analyzer records.
- `runtime/` contains installed modules, the socketable execution pipeline,
  Binary64 arithmetic, scalar computation, schedulers, memory, and artifact sinks.
- `server/computation/` contains the separate computation/research substrate;
  `server/renderer/` contains renderer boundaries and the inherited C++ adapter.
- `tests/native/` contains host-native behavior and frozen-identity tests;
  `tests/cmake/` contains deterministic repository-boundary checks.
- `docs/architecture/` is the authoritative Russian technical architecture.
  `docs/research/` and the CDC documents preserve research provenance.
- Root renderer sources, UI, checked-in AArch64 binaries, BMP files, and the MP4
  are preserved collaborator material, not the production native build.

## Windows/MSYS2 MinGW64 build

Open the MSYS2 MinGW64 shell, change to the repository root, and run:

```sh
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

The expected CTest inventory at this milestone contains eight native and
repository-boundary targets. To run the focused native executables directly:

```sh
./build/fractal_core_tests.exe
./build/socketable_runtime_tests.exe
./build/thread_pool_scheduler_tests.exe
./build/computation_socket_tests.exe
./build/numeric_socket_tests.exe
```

## Qt 6.11.1 workstation and Android SDK

The validated Windows workstation uses the installed Qt 6.11.1 MinGW package
and keeps the portable native build independent from Qt. The optional
`mandel_qt_smoke` target proves Qt Core, GUI, QML/Quick, Android packaging, and
linkage to the frozen portable registry without defining the production UI.

The pinned paths are:

- Qt host: `C:\Qt\6.11.1\mingw_64`;
- Qt Android ABIs: `C:\Qt\6.11.1\android_*`;
- Qt tools: `C:\Qt\Tools`;
- JDK 21: `D:\Dev\Java\jdk-21`;
- Android SDK: `D:\Android\Sdk`;
- Android NDK: `D:\Android\Sdk\ndk\27.2.12479018`;
- AVD and Gradle data: `D:\Android\Avd` and `D:\Android\Gradle`;
- Qt build trees: `D:\Build\mandel_web_ecf`.

In PowerShell, load the deterministic environment and validate it with:

```powershell
. .\scripts\qt-env.ps1
.\scripts\validate-qt6.ps1
```

The current-user PowerShell execution policy is `RemoteSigned`, which permits
these local scripts while retaining the downloaded-script signature check.
`qt-env.ps1` deliberately puts Qt's MinGW runtime first for the current process.
Run the portable MSYS2 build in a fresh shell, or restore its runtime first with
`$env:PATH = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:PATH"`; never mix
objects or runtime DLLs from the two MinGW distributions.

Desktop and Android builds use `CMakePresets.json`. Useful presets include
`qt-desktop-debug`, `qt-desktop-release`, each single Android ABI debug preset,
and `qt-android-multiabi-release`. The corresponding `apk` and `aab` build
presets create installable packages. Qt Creator contains one desktop kit and
four Android kits using API 36, Build Tools 36.0.0, JDK 21, and NDK r27c.

The API 36 x86_64 AVD is named `mandel_api36_x86_64`. It requires AMD SVM in
firmware and Windows Hypervisor Platform before accelerated execution. Enable
SVM in UEFI/BIOS, enable **Windows Hypervisor Platform** in **Turn Windows
features on or off**, reboot, and verify with `emulator -accel-check`. A physical
arm64 device with USB debugging is a valid alternative. Production keystores
and signing secrets must never be committed.

Then run `git diff --check`, `git status --short --branch`, and
`sha256sum CDC.pdf`. The checked-in AArch64 binaries must not be executed on the
Windows host.

## Language and terminology

The governing rules are in `docs/architecture/language_policy.md`. Approved
English-to-Russian terms are in
`docs/architecture/terminology_glossary.md`. Review Russian changes against the
glossary and translation ledger, then use the English milestone summary to check
scope and unresolved questions. Machine-facing material stays English: C
identifiers, public symbols, filenames, module and schema IDs, ABI/API names,
manifest keys, build targets, command-line flags, commands, and serialized values.

The localization validator checks file presence, UTF-8 decoding, frozen IDs and
checksums, generated-artifact cleanliness, and build-tree policy. It cannot judge
translation quality and does not claim native-speaker review.

## Current architecture

The static registry assembles a formula, Binary64 numeric implementation,
scalar computation module, refinement selection, serial or deterministic bounded
thread-pool scheduler, bounded analyzer chain, rasterizer, BMP encoder, scoped
memory implementation, telemetry/platform modules, and transactional artifact
sink. Computation produces a complete field before sequential analysis;
rasterization and encoding occur only after success. Publication commits only
after the complete encoded stream succeeds. CDC refinement and Ouro memory remain
explicitly unavailable. There is no formula socket yet.

## Frozen identities

- Installed modules: 22; registry identity `d73d9e545afa7735`.
- Analyzer chain: `0116ac94c4b64d99`.
- Binary64 execution: `2f300bf0f7ea5244`.
- Mandelbrot field/pixels/BMP: `99ec88c2a0f8bac3`, `4866aacc38290b5f`,
  `fb1a83bd5ca28e5f`; 2358 bytes.
- Julia field/pixels/BMP: `0fb4458e08bad6e1`, `b272f08b0bbdca2b`,
  `4d4aa95bd137ec87`; 2358 bytes.
- Serial/thread-pool scheduler module identities: `063fcb1076049dd8` and
  `d55b1ea157b8ae6a`.
- `CDC.pdf` SHA-256:
  `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

## Reviewing and prompting future Codex work

Ask Codex to follow `docs/architecture/language_policy.md`, use
`docs/architecture/terminology_glossary.md`, keep implementation discussion and
the final report in English, preserve machine identifiers, and update both
`docs/architecture/translation_ledger.md` and an English milestone summary.
State frozen identities and explicit non-goals when behavior must remain stable.

To understand a Russian change, ask: “Explain in English the technical changes
to `<path>`, mapping terminology through
`docs/architecture/terminology_glossary.md`, and identify any change to an
architectural claim.” Codex should explain in English without replacing the
authoritative Russian document.
