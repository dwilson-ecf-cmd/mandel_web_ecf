# Журнал перевода

## Правила статуса

`COMPLETED` означает завершённый перевод с проверкой терминов;
`PRESERVED` — намеренное сохранение исходного языка или происхождения;
`REVIEW_NEEDED` — необходимое будущее решение. Колонка «Машинный английский»
показывает, что ID, символы, пути, команды, ключи, версии, формулы и
контрольные суммы намеренно оставлены на английском языке или без языкового
изменения.

## Документация

| Путь | Исходный язык | Итоговый язык | Статус | Термины проверены | Смысл сохранён | Неразрешённые вопросы | Машинный английский |
|---|---|---|---|---|---|---|---|
| `README.md` | English with collaborator Russian references | Russian-first with English maintainer link | `COMPLETED` | да | да; актуализировано завершённое состояние | нет | да |
| `docs/architecture.md` | English | Russian | `COMPLETED` | да | да; явно добавлено направление зависимостей | нет | да |
| `docs/architecture/FOUNDATION.md` | English | Russian | `COMPLETED` | да | да; последующие завершённые этапы отмечены отдельно | нет | да |
| `docs/architecture/repository_hygiene_audit.md` | English | Russian | `COMPLETED` | да | да; исторические числа сохранены | нет | да |
| `docs/architecture/runtime_registry.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/socketable_runtime.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/artifact_sink.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/artifact_lifecycle.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/analysis_socket.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/analysis_records.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/analysis_pipeline.md` | English | Russian | `COMPLETED` | да | да; область ноль-или-один явно помечена как историческая | нет | да |
| `docs/architecture/escape_classification_summary.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/iteration_histogram_analyzer.md` | English | Russian | `COMPLETED` | да | да; ID этапа и историческая область сохранены | нет | да |
| `docs/architecture/spatial_workload_grid_analyzer.md` | English | Russian | `COMPLETED` | да | да; историческая область сохранена | нет | да |
| `docs/architecture/bounded_analyzer_chain.md` | English | Russian | `COMPLETED` | да | да; прежний следующий этап отмечен как завершённый исторический | нет | да |
| `docs/architecture/job_specification.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/deterministic_thread_pool_scheduler.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/computation_socket.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/binary64_numeric_socket.md` | English | Russian | `COMPLETED` | да | да; все таблицы и идентичности сохранены | нет | да |
| `docs/architecture/formula_socket.md` | N/A | Russian | `COMPLETED` | да | новый авторитетный контракт формулы | нет | да |
| `docs/architecture/field_ownership.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/architecture/module_compatibility.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/renderer_evaluation.md` | English | Russian | `COMPLETED` | да | да | нет | да |
| `docs/roadmap/next_attack_plan.md` | English, obsolete analyzer milestone | Russian, current formula-socket milestone | `COMPLETED` | да | да; дорожная карта актуализирована по завершённым этапам | нет | да |

## Новые документы политики и сопровождения

| Путь | Исходный язык | Итоговый язык | Статус | Термины проверены | Смысл сохранён | Неразрешённые вопросы | Машинный английский |
|---|---|---|---|---|---|---|---|
| `docs/architecture/language_policy.md` | N/A | Russian | `COMPLETED` | да | новая политика | нет | да |
| `docs/architecture/terminology_glossary.md` | N/A | bilingual English/Russian | `COMPLETED` | да | новый нормативный глоссарий | нет | да |
| `docs/architecture/localization_inventory.md` | N/A | Russian | `COMPLETED` | да | новая инвентаризация | нет | да |
| `docs/architecture/translation_ledger.md` | N/A | Russian | `COMPLETED` | да | новый журнал | нет | да |
| `docs/maintainer_guide.en.md` | N/A | English | `PRESERVED` | да | намеренный английский документ | нет | да |
| `docs/architecture/localization_summary.en.md` | N/A | English | `PRESERVED` | да | намеренное английское резюме | нет | да |
| `docs/architecture/linux_contract_parity.md` | N/A | Russian | `COMPLETED` | да | новый архитектурный отчёт | нет | да |

## Комментарии исходного кода

В каждом файле ниже переведены только существующие поясняющие комментарии.
Исполняемый код, строковые литералы, идентификаторы, директивы, тестовые
фикстуры и математические обозначения не изменены.

| Путь | Исходный язык | Итоговый язык | Статус | Термины проверены | Смысл сохранён | Неразрешённые вопросы | Машинный английский |
|---|---|---|---|---|---|---|---|
| `runtime/include/fractal/installed_modules.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `runtime/src/memory_backend.c` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/computation/cdc/include/fractal_cdc_substrate.h` | English | Russian comments | `COMPLETED` | да | да; утверждение CDC не расширено | нет | да |
| `server/computation/cdc/src/fractal_cdc_region_study.c` | English | Russian comments | `COMPLETED` | да | да; область оракула неизменна | нет | да |
| `server/computation/cdc/src/fractal_cdc_two_child.c` | English | Russian comments | `COMPLETED` | да | да; обозначения сохранены | нет | да |
| `server/renderer/cdc/include/fractal_cdc_metrics.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/include/fractal_cdc_model.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/include/fractal_cdc_renderer.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/include/fractal_cdc_trace.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/src/fractal_cdc_metrics.c` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/src/fractal_cdc_model.c` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/cdc/src/fractal_cdc_trace.c` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `server/renderer/include/fractal_cpp_adapter.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `shared/include/fractal/analysis.h` | English | Russian comments | `COMPLETED` | да | да; формулы интервалов неизменны | нет | да |
| `shared/include/fractal/formula.h` | N/A | Russian comments | `COMPLETED` | да | новое состояние и границы совместимости | нет | да |
| `shared/include/fractal/scheduler.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `shared/include/fractal/socketable_runtime.h` | English | Russian comments | `COMPLETED` | да | да | нет | да |
| `shared/src/analysis.c` | English | Russian comments | `COMPLETED` | да | да; арифметическое утверждение неизменно | нет | да |
| `shared/src/render_spec.c` | English | Russian comments | `COMPLETED` | да | да; грамматика неизменна | нет | да |
| `tests/native/test_core.c` | English | Russian comments | `COMPLETED` | да | да; обязательства доказательства не изменены | нет | да |

## Намеренно сохранённые материалы

`CDC.pdf`, `docs/research/*.md`, `docs/cdc_*.md`, унаследованные аудиты,
исходный русский интерфейс в `index.html`, `style.css`, `render.cpp`,
`render_engine.cpp`, `server.cpp` и `server.py`, двоичные файлы, BMP и видео не
переводились. Их точная классификация находится в
`docs/architecture/localization_inventory.md`. Неразрешённой терминологии на
этом этапе нет; лингвистическая проверка носителем языка не заявляется.

## Изоляция унаследованных поверхностей

Добавлены русские `legacy/README.md`, `legacy_isolation_plan.md`, `legacy_path_map.md` и `directory_ownership.md`, а также русские сообщения детерминированной проверки. Исходное содержимое перемещённых файлов участника проекта не переводилось и не изменялось. Английские машинные пути, статусы и SHA-256 сохранены намеренно.

## Снятие Python-сервера

Добавлена русская авторитетная запись `retired_python_server.md`; действующие документы различают `ISOLATED`, `RETIRED`, `PRESERVED` и `COMPATIBILITY`. Исходный текст удалённого файла не переводился и остаётся восстановимым через Git.

## Снятие нативного сервера и иностранных программ

Добавлены русские записи `retired_native_http_server.md` и `retired_foreign_executable_bundle.md`. Удалённые исходный файл и двоичные данные не переводились и сохраняются в Git. Действующие документы отличают `RETIRED` от `PRESERVED`, `ISOLATED`, `COMPATIBILITY` и `AUTHORITATIVE`.

## Снятие автономных рендереров

Добавлены русские `standalone_renderer_knowledge_audit.md` и `retired_standalone_renderers.md`. Удалённые исходники не переводились; знания классифицированы, суммы и восстановление сохранены. Статусы репозитория теперь различают `AUTHORITATIVE`, `COMPATIBILITY`, `PRESERVED_REFERENCE` и `RETIRED`.
