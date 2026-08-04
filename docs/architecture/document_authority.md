# Полномочия архитектурной документации

## Правило

Действующие контракты описываются только документами со статусом **Авторитетный**. **Исторический**, **Совместимость**, **Справочный** и **Заменённый** материалы сохраняют контекст, но не могут переопределять действующий контракт. При расхождении действует наиболее узкий авторитетный документ из таблицы.

## Архитектурный комплект

| Документ | Статус | Область полномочий или замена |
|---|---|---|
| `FOUNDATION.md` | Авторитетный | Основные неизменяемые принципы |
| `language_policy.md`, `terminology_glossary.md` | Авторитетный | Язык и утверждённая терминология |
| `runtime_registry.md`, `module_compatibility.md` | Авторитетный | Реестр, модули и совместимость |
| `formula_socket.md`, `binary64_numeric_socket.md`, `computation_socket.md` | Авторитетный | Формула, число и вычисление |
| `scheduler_contract.md`, `deterministic_thread_pool_scheduler.md` | Авторитетный | Планирование |
| `analysis_socket.md`, `analysis_pipeline.md`, `analysis_records.md`, `bounded_analyzer_chain.md`, `escape_classification_summary.md`, `iteration_histogram_analyzer.md`, `spatial_workload_grid_analyzer.md` | Авторитетный | Анализаторы, цепочка и записи |
| `artifact_sink.md`, `artifact_lifecycle.md` | Авторитетный | Транзакционная публикация |
| `job_specification.md`, `field_ownership.md`, `socketable_runtime.md` | Авторитетный | Задание, владение и композиция |
| `linux_contract_parity.md` | Авторитетный | Межплатформенный контракт текущего этапа |
| `localization_completion_audit.md`, `remaining_english_inventory.md`, `repository_inventory.md`, `legacy_inventory.md`, `document_authority.md`, `repository_cleanup_proposal.md` | Авторитетный | Сохранность и классификация репозитория |
| `localization_inventory.md`, `translation_ledger.md` | Исторический | Журнал перехода; итог задаёт аудит завершения |
| `localization_summary.en.md` | Справочный | Английский отчёт, не архитектурная норма |
| `runtime_registry_artifact_audit.md`, `analysis_socket_audit.md`, `escape_summary_analyzer_audit.md`, `socketable_runtime_audit.md` | Исторический | Свидетельства соответствующих этапов |
| `repository_hygiene_audit.md` | Заменённый | Заменён текущими инвентаризациями и предложением очистки |

## Документы вне каталога

| Поверхность | Статус | Примечание |
|---|---|---|
| `docs/architecture.md`, `docs/computation_architecture.md`, `docs/fractal_studio_model.md` | Исторический | Ранние общие модели; действуют узкие документы выше |
| `docs/computation_substrate_inspection.md`, `docs/renderer_evaluation.md`, `docs/repository_discovery.md` | Исторический | Снимки обследования |
| `docs/android_architecture.md`, `docs/ouro_integration.md` | Справочный | Будущие интеграционные исследования, не текущий контракт |
| `docs/collaborator_requirements.md`, `docs/legacy_retirement_criteria.md` | Авторитетный | Требования сотрудничества и условия вывода |
| `docs/cdc_*.md`, `docs/research/*.md` | Справочный | Исследовательское происхождение; решения CDC применимы только внутри эксперимента |
| `docs/campaign1_python_audit.md` | Исторический | Аудит прежней кампании |
| `docs/roadmap/next_attack_plan.md` | Авторитетный | Состояние и очередность, но не контракт выполнения |

Конфликтующих авторитетных документов не выявлено: общие правила не заменяют более узкие контракты, а прежние обследования явно не обладают нормативной силой.

## Авторитетные поверхности выполнения

| Роль | Реализация | Дубликаты и готовность вывода |
|---|---|---|
| Registry | `shared/src/module_registry.c`, состав в `runtime/src/installed_modules.c` | Старые ручные композиции совместимы лишь как наследие; вывод после поиска потребителей |
| Formula | `runtime/src/formula_mandelbrot.c`, `formula_julia.c`, общее ядро `formula_quadratic.c` | Прежняя математика в корневом рендерере сохраняется для происхождения |
| Numeric | `runtime/src/numeric_binary64.c` | Иных подключённых числовых реализаций нет |
| Compute | `runtime/src/scalar_computation.c` | `server/computation/` — отдельная экспериментальная подложка, не замена штатного сокета |
| Scheduler | `runtime/src/socketable_runtime.c` (serial), `thread_pool_scheduler.c` | `runtime/src/scheduler.c` не собирается и готов только к справочному архивированию |
| Analyzer | `shared/src/analysis.c`, регистрация в `installed_modules.c` | Анализаторы образуют одну ограниченную цепочку; исследовательские метрики CDC отдельны |
| Rasterizer | `runtime/src/socketable_runtime.c` | `server/renderer/` нужен адаптеру совместимости; не готов к удалению |
| Encoder | `runtime/src/socketable_runtime.c` | Корневые писатели BMP наследованы; сначала проверить потребителей |
| Artifact Sink | `runtime/src/artifact_sink.c` | Прямые файловые пути прежних программ обходят контракт и остаются наследием |

## Документы изоляции

`legacy_isolation_plan.md`, `legacy_path_map.md` и `directory_ownership.md` являются авторитетными для физического расположения сохранённых поверхностей. `legacy/README.md` является справочным входом и не переопределяет условия снятия с эксплуатации.

## Запись о снятии с эксплуатации

`retired_python_server.md` является авторитетной записью статуса `RETIRED`, контрольной суммы и способа восстановления Python-сервера. Исторические обследования сохраняют значение снимков, но не возвращают компоненту действующий статус.

## Записи нативной исполняемой эпохи

`retired_native_http_server.md` и `retired_foreign_executable_bundle.md` являются авторитетными для статуса `RETIRED`, исторических сумм и восстановления соответствующих файлов. Они не задают требования будущей сетевой архитектуре и не меняют полномочия `shared/`, `runtime/` или совместимого рендерера.

## Записи автономных рендереров

`standalone_renderer_knowledge_audit.md` авторитетно классифицирует исторические обязанности; `retired_standalone_renderers.md` задаёт статус `RETIRED`, суммы и восстановление. Они не заменяют контракты `shared/`/`runtime/` и не меняют статус `COMPATIBILITY` каталога `server/renderer/`.
