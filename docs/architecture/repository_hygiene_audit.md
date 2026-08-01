# Repository hygiene audit

Audit date: 2026-08-01 (UTC)

This document records the checkout as it existed before the hygiene change. It
is an inventory and safety policy, not a request to remove collaborator-authored
material. Paths and counts below are derived from Git rather than from a prior
task summary.

## Mandatory baseline

- Starting commit: `97f73056102f98c4fa98e5a2430080d85d83050f`
  (`Merge pull request #8 from
  dwilson-ecf-cmd/codex/establish-socketable-fractal-runtime`).
- Branch/state: `work`, with `git status --short --branch` reporting only
  `## work`; the index and working tree were clean.
- Branches: the checkout exposed only the local `work` branch.
- Remotes: none were configured; `git remote -v` produced no entries.
- The 20-commit baseline history ran from `97f7305` through `a12b0ca` and was
  inspected before any edit.

## Runtime determination

`SOCKETABLE_RUNTIME_PRESENT`

This is based on implementation and tests in the checkout, not merely on the
merge title or architecture notes. The repository contains module descriptors
and assembly validation, a binary64 numeric backend, Mandelbrot and Julia
formula modules, a conventional point-computation backend, a serial scheduler,
an iteration field, a BGR8 rasterizer, a BMP encoder, scoped system memory, no-op
telemetry, and a host platform adapter. The implementations are assembled in
`runtime/src/socketable_runtime.c`, their public contracts are in
`shared/include/fractal/socketable_runtime.h`, and end-to-end coverage is in
`tests/native/test_socketable_runtime.c`.

The descriptors named `refinement.cdc.unavailable` and
`memory.ouro.unavailable` are explicit unavailable sentinels, not working
backends. They do not make the conventional socketable runtime partial: runtime
validation rejects either choice and the supported assembly uses no refinement
and system memory. No unavailable module was filled in during this milestone.

## Deterministic inventory method

Run these commands from the repository root. `LC_ALL=C` makes ordering stable;
the NUL-delimited variants preserve unusual path names.

```sh
LC_ALL=C git ls-files
LC_ALL=C git ls-files -s
git status --ignored --short
find . -path ./.git -prune -o -type f -print | LC_ALL=C sort
find . -path ./.git -prune -o -type l -print | LC_ALL=C sort
find . -path ./.git -prune -o -type f -perm -111 -print | LC_ALL=C sort
du -ah --exclude=.git . | LC_ALL=C sort -h
git ls-files -z | xargs -0 stat --printf='%s\t%n\n' | LC_ALL=C sort -nr
```

At the baseline, every worktree file outside `.git` was tracked: there were no
untracked or ignored files, and no symbolic links. Git recorded 901 paths and
935,110,328 bytes. Top-level distribution was 750 paths under `frames/`, 61
under `docs/`, 31 each under `server/` and `shared/`, five under `web/`, four
under `runtime/`, three under `tests/`, and 16 repository-root paths.

## Classified tracked content

| Class | Deterministic identification | Baseline result and disposition |
| --- | --- | --- |
| Source | `git ls-files '*.c' '*.cpp' '*.py' '*.html' '*.css'` | 32 implementation files: 25 C, four C++, one Python, one HTML, and one CSS. Active native sources live in `shared/`, `runtime/`, and `server/`; root application sources are preserved. |
| Public headers | `git ls-files 'shared/include/*.h' 'shared/include/**/*.h'` | 23 headers under `shared/include/fractal/`; these are the public native contracts. |
| Component/private headers | `git ls-files 'runtime/include/**/*.h' 'server/**/include/*.h' 'server/**/include/**/*.h'` | 14 headers: one runtime memory header and 13 server computation/renderer headers. Their narrower locations define their current component scope. |
| Tests | `git ls-files 'tests/**'` | Three files: `tests/native/test_core.c`, `tests/native/test_socketable_runtime.c`, and `tests/cmake/validate_milestone.cmake`. |
| CMake | `git ls-files 'CMakeLists.txt' '*.cmake'` | Root `CMakeLists.txt` plus the milestone validation script (two files). |
| Documentation | `git ls-files '*.md'` | 72 Markdown files: 61 under `docs/`, five under `web/`, and six elsewhere. |
| PDF references | `git ls-files '*.pdf'` | One tracked reference, `CDC.pdf` (589,882 bytes). It is source/reference material, not disposable build output. |
| Legacy/root application material | Explicit root paths | `server.cpp`, `server.py`, `render.cpp`, `render_engine.cpp`, `index.html`, and `style.css` are preserved. Their location/history may be legacy, but this audit does not make an authorship or retirement decision. |
| Compiled binaries | Git executable mode plus ELF magic inspection | `app_server` (56,008 bytes), `render` (911,856), and `render_engine` (888,264) are tracked mode `100755`, 64-bit little-endian AArch64 ELF files. They were inspected as data and **not executed**. |
| Generated images | `git ls-files '*.bmp'` | 752 tracked BMPs: `render.bmp`, `render_hd.bmp`, and exactly `frames/frame_0000.bmp` through `frames/frame_0749.bmp`. These collaborator-authored tracked artifacts remain tracked. |
| Frame sequences | `git ls-files 'frames/frame_[0-9][0-9][0-9][0-9].bmp'` | One complete tracked 750-frame sequence, approximately 882 MiB on disk; each frame is 1,230,774 bytes. |
| Videos | `git ls-files '*.mp4' '*.webm'` | One tracked video, `render.mp4` (1,610,086 bytes). |
| Runtime output | Explicit runtime paths | `runtime/artifacts/README.md` is tracked; no generated runtime artifact or job directory existed. Root renders and frames are historical tracked outputs, not current dirty-state symptoms. |
| Build output | Known build paths and CMake products | No untracked build tree existed. The three tracked ELF files are historical artifacts and are not reclassified as safe host tools. |
| Editor metadata | `.vscode`, `.idea`, swap/backup-name inspection | None present. |
| Platform metadata | Android/Gradle/Termux manifest and project-name inspection | None present. The AArch64 executables are the only platform-specific binary artifacts identified. |
| Temporary files | names ending `~`, `.tmp`, `.temp`, `.swp`, `.swo`, `.bak` | None present. |
| Large files | tracked byte-size sort | 753 paths exceed 1 MiB: 750 frames, the two root BMPs, and the MP4. `render_hd.bmp` is largest at 6,220,854 bytes; total tracked content is about 892 MiB. |
| Executable files | `git ls-files -s | awk '$1 == 100755'` | Only the three AArch64 ELF files above. No script was executable. |
| Symbolic links | filesystem scan and Git mode `120000` | None. |
| Suspicious names/permissions | portable-character scan and mode scan | No tracked path contains whitespace/control characters, and files use only `0644` or the three intentional Git `0755`/`100755` modes. The extensionless executable names are conspicuous and are documented above. |

Extension totals provide a compact completeness cross-check: 752 `.bmp`, 72
`.md`, 37 `.h`, 25 `.c`, four `.cpp`, three extensionless files, and one each
of `.cmake`, `.css`, `.gitignore`, `.html`, `.mp4`, `.pdf`, `.py`, and `.txt`.

## Artifact and ignore policy

Tracked artifacts remain tracked regardless of ignore matching. In particular,
the existing frame sequence and root renders are not removed, and adding an
ignore rule cannot conceal modifications to them. The current `.gitignore`
uses intentionally narrow rules:

- `/runtime/artifacts/*.bmp`, `/runtime/artifacts/*.mp4`, and
  `/runtime/jobs/` cover runtime products in their designated location;
- `/frames/frame_[0-9][0-9][0-9][0-9].bmp` prevents recreation of this exact
  numbered output family without hiding other images or fixtures;
- `__pycache__/` and `*.py[cod]` cover Python interpreter caches; and
- `/build/` covers only the repository-root out-of-source native build tree.

There is deliberately no repository-wide `*.bmp`, `*.mp4`, executable, PDF,
Android, editor, `tmp`, or generic binary ignore. Such patterns could conceal
legitimate fixtures, references, or collaborator files. Before adding any new
rule, first use `git status --ignored --short` to confirm the dirty-state cause,
then document the producer and choose the narrowest stable path/name pattern.

## Safety and reproducibility conclusions

The checked-in AArch64 executables must never be invoked during inspection,
build, or test. Native validation must instead compile sources into an
out-of-source `build/` directory and run only those newly produced host test
programs through CTest. A clean-checkout validation sequence is:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
git status --short
```

The build has no download step or generated source dependency. Historical
tracked renders are unnecessary to compile the native libraries and tests but
remain part of the checkout. This audit changes no public API, directory
architecture, mathematical behavior, runtime feature, or tracked artifact.
