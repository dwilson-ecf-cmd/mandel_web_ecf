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

The expected CTest inventory at this milestone contains nine tests in total, including the milestone and
localization checks. To run the focused native executables directly:

```sh
./build/fractal_core_tests.exe
./build/socketable_runtime_tests.exe
./build/thread_pool_scheduler_tests.exe
./build/computation_socket_tests.exe
./build/numeric_socket_tests.exe
./build/formula_socket_tests.exe
./build/header_contract_tests.exe
```

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
explicitly unavailable. Mandelbrot and Julia are selected through the formula socket.

## Frozen identities

- Installed modules: 22; registry identity `616b9a3b92e787e5`.
- Analyzer chain: `0116ac94c4b64d99`.
- Binary64 execution: `2f300bf0f7ea5244`.
- Formula execution identities: Mandelbrot `82001a07c687c7b1`; Julia `b05090f17f32fc19`.
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


## Preservation and authority audit

The repository classification is maintained in
`docs/architecture/repository_inventory.md`; legacy retirement recommendations
are in `docs/architecture/legacy_inventory.md`. Document status is normative in
`docs/architecture/document_authority.md`, and the cleanup document is a proposal
only. Contributor policy is available in Russian at `docs/CONTRIBUTING.ru.md`.
These inventories do not authorize moves, removals, renames, or runtime changes.

## Isolated legacy layout

The collaborator-original standalone renderers are now `RETIRED` and recoverable
only through Git; the original C++ server and the former AArch64 Android/Termux executable
bundle are now `RETIRED` and recoverable only through Git. Use
`docs/architecture/legacy_path_map.md` to resolve every former root path. CMake
must not compile, link, copy, or launch anything under `legacy/`; repository tests verify retirement records and preserved reference media
without opening retired sources as live files. No historical
content was deleted by the isolation milestone, and presence never authorizes
execution or retirement. The former Python server is now `RETIRED`: its historical
paths, checksum, rationale, and Git recovery instructions are recorded in
`docs/architecture/retired_python_server.md`. That retirement applied to no other legacy component. Separate owner
approval subsequently retired the native C++ HTTP server and foreign executable bundle;
see `docs/architecture/retired_native_http_server.md` and
`docs/architecture/retired_foreign_executable_bundle.md`. The standalone renderer sources were subsequently retired after the
unique-knowledge audit found no unresolved responsibility; see
`docs/architecture/standalone_renderer_knowledge_audit.md` and
`docs/architecture/retired_standalone_renderers.md`. The original UI and media
remain preserved references, while `server/renderer/` remains an intentional
compatibility dependency. Retired execution paths are not prototypes or
compatibility requirements for future service architecture.
# Workspace and dual-client milestone

The authoritative product layer is now a bounded, platform-neutral C11
Workspace contract. Desktop and Mobile share typed `ClientIntent` semantics and
self-contained snapshots; the reducer deterministically enforces revisions,
render generations, participant capabilities, command sequences, and the
single-controller lease. No GUI, networking, authentication, cryptography, or
deployment provider was selected. The authoritative design documents are the
new Russian files under `docs/architecture/`, beginning with
`fractal_studio_overview.md`.

## Secure pairing contract baseline

The secure pairing milestone adds a platform-neutral protocol contract and deterministic reducer for turning an unknown client installation into a recognized Workspace participant. It deliberately does not add sockets, TLS, AES, certificates, Android APIs, QR rendering, GUI code, cloud SDKs, or bearer tokens. Pairing is modeled as typed transcript-bound messages, explicit local approval, proof-of-possession hooks, bounded credentials, capability ceilings, and revocation epochs.

## Platform seams and deterministic reacquisition campaign

English summary: the repository now defines contract-only platform service groups, compatibility negotiation, deterministic connection/reacquisition reducers, bounded retry rules, replay-cache idempotence, and fake transport tests. The Russian architecture documents under `docs/architecture/*reacquisition*`, `*compatibility*`, `*lifecycle*`, `*seams*`, and `reconnection_overview.md` are the repository-facing source of truth for this campaign.

## Secure implementation stack campaign

English summary: the Russian security and protocol documents now select OpenSSL 3.5 LTS, Android platform TLS, Android Keystore, ECDSA P-256 with SHA-256, deterministic CBOR, bounded framing, Unix-domain sockets, named pipes, and TLS 1.3 framed streams while retaining existing Workspace, pairing, replay, reacquisition, runtime, and artifact identities. The executable evidence is a bounded codec/framing/security-policy harness plus fake TLS tests; production network listeners and keys remain absent.
