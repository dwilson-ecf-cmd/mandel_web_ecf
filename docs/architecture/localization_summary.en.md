# Russian technical baseline: maintainer summary

Status: `PASSED`

This milestone establishes Russian as the authoritative language for current
human-facing technical documentation and source comments while preserving an
English guide and this English summary for the maintainer.

## Scope completed

- 23 existing Markdown documents were translated or rewritten Russian-first:
  the root README, the completed execution-spine architecture, manifest
  architecture, and the current roadmap.
- 19 source files had existing explanatory comments translated. No executable
  statement or machine string was intentionally changed.
- New Russian policy, glossary, inventory, and ledger documents were added,
  together with `docs/maintainer_guide.en.md` and this English summary.
- The root README now leads in Russian, links clearly to the English guide, and
  preserves the collaborator's original Mandelbrot Studio provenance.

## Terminology

The glossary standardizes terms including “модуль,” “сокет (точка
подключения),” “контракт,” “реестр,” “числовая реализация,” “вычислительный
модуль,” “планировщик,” “запечатанная единица работы,” “цепочка
анализаторов,” “приёмник артефактов,” “детерминированный идентификатор,” and
“транзакционная публикация.” There is no unresolved terminology. No
native-speaker linguistic review is claimed.

## Preserved material

Machine-facing C identifiers, public symbols, filenames, module and schema IDs,
ABI/API identifiers, manifest keys, build targets, command-line flags,
serialized values, hashes, formulas, paths, commands, versions, and test names
remain English or unchanged. CDC research documents, `CDC.pdf`, historical
audits, collaborator-original Russian UI/source text, checked-in AArch64
binaries, BMP frames/renders, and video were not translated or modified.

## Architectural wording

No executable architectural claim was changed. Documents whose zero-or-one
analyzer scope or next-step recommendation described an earlier milestone now
label that scope as historical and link to the completed successor. The current
roadmap names formula-socket extraction and does not claim that it exists.

## Validation

The new deterministic CMake check validates required localization files,
UTF-8, exact machine IDs and schemas, frozen runtime identities, the
`CDC.pdf` checksum, generated-artifact cleanliness, and absence of tracked
`build/` output.

Validation completed in the Windows/MSYS2 MinGW64 environment:

- a clean Ninja configure and 43-step build passed with GCC/G++ 15.2.0;
- all seven CTest targets passed, including `localization_baseline`;
- all five focused native executables passed directly;
- the registry remained 22 modules at `d73d9e545afa7735`;
- Binary64 execution remained `2f300bf0f7ea5244` and the analyzer chain
  remained `0116ac94c4b64d99`;
- Mandelbrot and Julia field, pixel, BMP, and byte identities remained frozen;
- scheduler, computation, assignment, analyzer-record, and module identities
  printed by the native tests matched the starting audit;
- `git diff --check` passed, generated artifacts were unchanged, no build output
  was tracked, and `CDC.pdf` retained its authoritative SHA-256.

Only documentation, comments, and repository validation changed. No formula,
numeric ABI, computation, scheduling, analysis, rasterization, encoding,
manifest runtime data, artifact payload, Android, Ouro, or CDC behavior changed.
