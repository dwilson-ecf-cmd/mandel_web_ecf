# Снятие с эксплуатации автономных исходных рендереров

Статус: `RETIRED`.

## Состав и происхождение

| Исторические пути | Исходное назначение | Связанный снятый файл | Размер | SHA-256 |
|---|---|---|---:|---|
| `render.cpp` → `legacy/applications/render.cpp` | именованный CLI, несколько формул и палитр, BMP | `render` | 6 964 | `6e40c8c23a52f8deffde730b6c34816152ee29cfa7b8e9830c8b3fafd94a1b60` |
| `render_engine.cpp` → `legacy/applications/render_engine.cpp` | image/HD/video, кадры и ffmpeg | `render_engine` | 6 571 | `e9f683a9396ccf722db55d8f3276c16f7431738d43bd03d74e5e8ffb2ab668c5` |

Полный аудит обязанностей находится в `standalone_renderer_knowledge_audit.md`; элементов `UNRESOLVED` нет. Математика Mandelbrot/Julia, Binary64, вычисление, планирование, анализ, растеризация, BMP, публикация и воспроизводимость имеют авторитетные модульные замены и замороженные тесты.

## Намеренно не перенесённое поведение

CLI и позиционный ABI, экспериментальные формулы, прежние палитры, фиксированные имена, OpenMP, shell/ffmpeg-видеоконвейер и слабая обработка ошибок имеют исторический, а не совместимый статус. Их описание и происхождение сохранены аудитом, Git и неизменёнными BMP/кадрами/MP4; они не становятся требованиями современной среды выполнения.

Владелец явно разрешил снятие. Git является архивом: использовать `git log --all -- legacy/applications/render.cpp` либо `git log --all -- legacy/applications/render_engine.cpp`, затем `git show <commit>:<путь>`. Удаление выполняется коммитом `Retire original standalone renderer sources`.

Исходный UI (`index.html`, `style.css`) и сгенерированные носители (`frames/`, `render.bmp`, `render_hd.bmp`, `render.mp4`) остаются `PRESERVED_REFERENCE`. `server/renderer/` остаётся `COMPATIBILITY`; его удаление этим решением не разрешено.
