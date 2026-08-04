# План изоляции унаследованных поверхностей

## Основание решения

План применяет только решения из `repository_inventory.md`, `legacy_inventory.md` и `repository_cleanup_proposal.md`. Авторитетные каталоги `shared/`, `runtime/` и `tests/`, а также используемая совместимая поверхность `server/renderer/` остаются на месте. До физического перемещения утверждён следующий точный набор.

| Текущий путь или группа | Классификация | Зависимость сборки / тестов | Совместимость и историческая ценность | Предлагаемое место | Статус |
|---|---|---|---|---|---|
| `render.cpp`, `render_engine.cpp` | LEGACY | отсутствует / только статическая проверка границ | исходные автономные рендереры участника проекта | `legacy/applications/` | `MOVE_APPROVED` |
| `server.cpp`, `server.py` | LEGACY | отсутствует / только статическая проверка границ | исходные HTTP-сервер и средство запуска; сохранённый рабочий процесс | `legacy/servers/` | `MOVE_APPROVED` |
| `app_server`, `render`, `render_engine` | REFERENCE | отсутствует / проверка существования и суммы | двоичные файлы AArch64 Android/Termux, происхождение прежних приложений | `legacy/binaries/` | `MOVE_APPROVED` |
| `server/renderer/` | COMPATIBILITY | `fractal_core` и `fractal_renderer_adapter` / `fractal_core_tests` | действующая совместимая граница C/C++ | без изменения | `BLOCKED_BY_COMPATIBILITY` |
| `runtime/src/scheduler.c` | LEGACY | не входит в CMake / отсутствует | ранняя реализация планировщика | без изменения | `REFERENCE_ONLY` |
| `index.html`, `style.css` | COMPATIBILITY | не входят в нативную сборку / отсутствует | единственный завершённый интерфейс участника проекта; возможное основание будущего Android UI | без изменения | `KEEP_IN_PLACE` |
| `web/` | EXPERIMENTAL | отсутствует / отсутствует | зарезервированные границы будущего интерфейса | без изменения | `KEEP_IN_PLACE` |
| `frames/*.bmp` | GENERATED | отсутствует / контроль сохранности | 750 кадров; массовое перемещение создаёт большой churn и меняет исторические пути | без изменения | `DEFER` |
| `render.bmp`, `render_hd.bmp`, `render.mp4` | GENERATED | отсутствует / контроль сохранности | исторические результаты, связанные с корневым UI | без изменения | `DEFER` |
| `CDC.pdf` и `docs/research/` | REFERENCE | исследовательское происхождение / контроль суммы PDF | неизменяемый источник CDC | без изменения | `REFERENCE_ONLY` |

## Политика носителей

Для `frames/*.bmp`, корневых BMP и `render.mp4` выбрана политика `DEFER_TO_LARGE_ARTIFACT_MIGRATION`: файлы не регенерируются, не перекодируются и не перемещаются. Для веб-ресурсов выбрана `KEEP_IN_PLACE_AND_CLASSIFY`. Политика `MOVE_NOW` применяется только к четырём исходным приложениям/серверам и трём двоичным справочным файлам.

## Последующее решение

После изоляции владелец отдельно разрешил снять `legacy/servers/server.py` с эксплуатации. Строка плана сохраняет историческое решение `MOVE_APPROVED`; текущий статус `RETIRED` определяет `retired_python_server.md`.

## Последующее снятие нативной эпохи

Владелец отдельно разрешил снять `legacy/servers/server.cpp` и три файла из `legacy/binaries/`. Их текущий статус `RETIRED` определяют `retired_native_http_server.md` и `retired_foreign_executable_bundle.md`. Исторические строки `MOVE_APPROVED` сохраняют решение этапа изоляции, а не текущую доступность файлов.

## Последующее снятие автономных рендереров

Полный аудит `standalone_renderer_knowledge_audit.md` не выявил `UNRESOLVED`. Владелец разрешил снять два файла; текущий статус `RETIRED` задаёт `retired_standalone_renderers.md`. Историческая строка `MOVE_APPROVED` больше не означает наличие живого пути.
