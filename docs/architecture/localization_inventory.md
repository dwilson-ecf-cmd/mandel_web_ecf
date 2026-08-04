# Инвентаризация локализации

## Метод и правила охвата

Инвентаризация выполнена от стартового коммита
`4d6933e5d03be6d3d84f7766eb59939af442dbd9`. На нём Git отслеживал 935 путей:
92 файла Markdown, 35 файлов C, 4 файла C++, 41 заголовок, 2 файла CMake и
750 кадров `frames/frame_*.bmp`. Каждый отслеживаемый путь покрыт конкретной
строкой или шаблоном ниже. Шаблон означает все отслеживаемые пути, совпадающие
с ним; исключения имеют приоритет над общими строками.

Статусы стабильны и машиночитаемы:

- `TRANSLATE_NOW` — авторитетный технический текст этого этапа;
- `PRESERVE_ORIGINAL` — исходный авторский или происхожденческий текст;
- `MACHINE_FACING_NO_TRANSLATION` — машинно-значимое содержимое;
- `LEGACY_REFERENCE` — унаследованная справочная реализация или документация;
- `GENERATED_DO_NOT_EDIT` — сгенерированный или двоичный артефакт;
- `RESEARCH_REFERENCE` — исследовательский материал, не изменяемый локализацией;
- `REQUIRES_LATER_REVIEW` — человекочитаемый материал вне текущего авторитетного набора.

## Авторитетная документация

| Путь | Статус | Обоснование |
|---|---|---|
| `README.md` | `TRANSLATE_NOW` | Русская точка входа и английская ссылка для сопровождающего. |
| `docs/architecture.md` | `TRANSLATE_NOW` | Направление зависимостей и границы ответственности. |
| `docs/architecture/FOUNDATION.md` | `TRANSLATE_NOW` | Состояние основы репозитория. |
| `docs/architecture/repository_hygiene_audit.md` | `TRANSLATE_NOW` | Авторитетный аудит гигиены и артефактов. |
| `docs/architecture/runtime_registry.md` | `TRANSLATE_NOW` | Реестр модулей. |
| `docs/architecture/socketable_runtime.md` | `TRANSLATE_NOW` | Исполняемая среда и зафиксированные результаты. |
| `docs/architecture/artifact_sink.md` | `TRANSLATE_NOW` | Контракт приёмника артефактов. |
| `docs/architecture/artifact_lifecycle.md` | `TRANSLATE_NOW` | Транзакционный жизненный цикл артефакта. |
| `docs/architecture/analysis_socket.md` | `TRANSLATE_NOW` | Сокет анализа. |
| `docs/architecture/analysis_records.md` | `TRANSLATE_NOW` | Типизированные записи анализаторов. |
| `docs/architecture/analysis_pipeline.md` | `TRANSLATE_NOW` | Исторический контракт конвейера с нулём или одним анализатором. |
| `docs/architecture/escape_classification_summary.md` | `TRANSLATE_NOW` | Сводный анализатор. |
| `docs/architecture/iteration_histogram_analyzer.md` | `TRANSLATE_NOW` | Гистограмма итераций. |
| `docs/architecture/spatial_workload_grid_analyzer.md` | `TRANSLATE_NOW` | Пространственная нагрузка. |
| `docs/architecture/bounded_analyzer_chain.md` | `TRANSLATE_NOW` | Текущая ограниченная цепочка анализаторов. |
| `docs/architecture/job_specification.md` | `TRANSLATE_NOW` | Неизменяемая спецификация задания. |
| `docs/architecture/deterministic_thread_pool_scheduler.md` | `TRANSLATE_NOW` | Детерминированный планировщик пула потоков. |
| `docs/architecture/computation_socket.md` | `TRANSLATE_NOW` | Сокет скалярных вычислений. |
| `docs/architecture/binary64_numeric_socket.md` | `TRANSLATE_NOW` | Числовой контракт Binary64. |
| `docs/architecture/formula_socket.md` | `TRANSLATE_NOW` | Контракт формулы, состояние и идентичности. |
| `docs/architecture/field_ownership.md` | `TRANSLATE_NOW` | Владение полями. |
| `docs/architecture/module_compatibility.md` | `TRANSLATE_NOW` | Совместимость модулей. |
| `docs/roadmap/next_attack_plan.md` | `TRANSLATE_NOW` | Текущий следующий этап. |

Новые файлы `docs/architecture/language_policy.md`,
`docs/architecture/terminology_glossary.md`,
`docs/architecture/localization_inventory.md` и
`docs/architecture/translation_ledger.md` имеют статус `TRANSLATE_NOW`.
`docs/maintainer_guide.en.md` и
`docs/architecture/localization_summary.en.md` намеренно имеют английский язык
и статус `PRESERVE_ORIGINAL` в будущих этапах.

## Исходный код и комментарии

| Путь или шаблон | Статус | Обоснование |
|---|---|---|
| `shared/include/fractal/*.h` | `MACHINE_FACING_NO_TRANSLATION` | Идентификаторы и контракты машинные; существующие авторитетные поясняющие комментарии переводятся точечно. |
| `shared/src/*.c` | `MACHINE_FACING_NO_TRANSLATION` | Код и строки сохраняются; переводятся только существующие поясняющие комментарии. |
| `runtime/include/fractal/*.h` | `MACHINE_FACING_NO_TRANSLATION` | Публичные идентификаторы сохраняются; комментарии о совместимости переводятся. |
| `runtime/src/*.c` | `MACHINE_FACING_NO_TRANSLATION` | Исполняемое поведение и строки не меняются; комментарии о владении переводятся. |
| `server/computation/**/*.c`, `server/computation/**/*.h` | `RESEARCH_REFERENCE` | CDC-математика не меняется; существующие краткие комментарии переводятся только без изменения обозначений. |
| `server/renderer/**/*.c`, `server/renderer/**/*.h`, `server/renderer/**/*.cpp` | `MACHINE_FACING_NO_TRANSLATION` | Активные границы и переходные комментарии переводятся, код и идентификаторы сохраняются. |
| `tests/native/*.c` | `MACHINE_FACING_NO_TRANSLATION` | Фикстуры, строки и проверки сохраняются; поясняющие комментарии переводятся. |
| `tests/cmake/*.cmake`, `CMakeLists.txt` | `MACHINE_FACING_NO_TRANSLATION` | Сборка и диагностические токены сохраняются; добавляется автономная проверка локализации. |
| `index.html`, `style.css` | `PRESERVE_ORIGINAL` | Исходный русский интерфейс участника проекта сохраняется как `PRESERVED_REFERENCE`. |
| `render.cpp`, `render_engine.cpp` | `RETIRED` | Сняты после аудита знаний; исходный текст доступен через Git. |
| `server.py` → `legacy/servers/server.py` | `RETIRED` | Файл снят с эксплуатации по разрешению владельца; перевод не выполнялся, история сохранена в Git. |

Пользовательские диагностические строки в активном C/C++ коде проверены. Они
являются именами модулей, сериализованными статусами, тестовыми диагностическими
сообщениями или унаследованным интерфейсом и поэтому не переводятся на этом
этапе: изменение таких строк могло бы изменить манифест, идентичность или
происхождение.

## Справочные, исследовательские и унаследованные документы

| Путь или шаблон | Статус | Обоснование |
|---|---|---|
| `docs/research/*.md` | `RESEARCH_REFERENCE` | 42 файла исследовательской трассируемости CDC; формулировки являются частью доказательного происхождения. |
| `docs/cdc_*.md` | `RESEARCH_REFERENCE` | Индексы, глоссарии, журналы и эксперимент CDC. |
| `docs/computation_architecture.md`, `docs/computation_substrate_inspection.md` | `RESEARCH_REFERENCE` | Исследовательская граница вычислительной основы. |
| `docs/campaign1_python_audit.md`, `docs/repository_discovery.md` | `LEGACY_REFERENCE` | Исторические аудиты исходного состояния. |
| `docs/collaborator_requirements.md`, `docs/legacy_retirement_criteria.md` | `PRESERVE_ORIGINAL` | Требования и критерии, связанные с происхождением. |
| `docs/renderer_evaluation.md` | `TRANSLATE_NOW` | Архитектура метрик и манифестов завершённой основы. |
| `docs/android_architecture.md`, `docs/ouro_integration.md`, `docs/fractal_studio_model.md` | `REQUIRES_LATER_REVIEW` | Полезный человекочитаемый текст вне завершённой исполняемой основы. |
| `docs/architecture/*_audit.md` | `LEGACY_REFERENCE` | Исторические аудиты отдельных завершённых этапов; исключение: `repository_hygiene_audit.md` переводится сейчас. |
| `docs/architecture/runtime_registry_artifact_audit.md` | `LEGACY_REFERENCE` | Историческая фиксация этапа. |
| `runtime/artifacts/README.md` | `REQUIRES_LATER_REVIEW` | Инструкции каталога артефактов вне текущего ядра документации. |
| `server/*/README.md`, `web/**/*.md`, `web/README.md` | `REQUIRES_LATER_REVIEW` | Заготовки будущих компонентов, не являющиеся текущей исполняемой основой. |

## Двоичные, сгенерированные и машинные файлы

| Путь или шаблон | Статус | Обоснование |
|---|---|---|
| `CDC.pdf` | `RESEARCH_REFERENCE` | Авторитетная исследовательская ссылка; SHA-256 фиксирован. |
| `server.cpp`, `app_server`, `render`, `render_engine` | `RETIRED` | Сняты по разрешению владельца; исходный текст и байты доступны через Git, суммы записаны авторитетно. |
| `frames/frame_[0-9][0-9][0-9][0-9].bmp` | `GENERATED_DO_NOT_EDIT` | 750 исторических сгенерированных кадров. |
| `render.bmp`, `render_hd.bmp`, `render.mp4` | `GENERATED_DO_NOT_EDIT` | Исторические продукты визуализации участника проекта. |
| `.gitignore` | `MACHINE_FACING_NO_TRANSLATION` | Узкие шаблоны политики артефактов. |

Содержимое `build/`, `runtime/jobs/`, новые файлы в `runtime/artifacts/` и кэши
интерпретатора не отслеживаются Git и имеют статус `GENERATED_DO_NOT_EDIT`.
Хеши, идентификаторы схем и модулей, JSON-ключи, команды, имена целей, публичные
символы и прочие машинные значения во всех категориях имеют статус
`MACHINE_FACING_NO_TRANSLATION` независимо от статуса окружающего файла.
