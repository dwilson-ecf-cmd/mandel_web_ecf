# Карта путей унаследованных материалов

Все перемещения выполнены через `git mv`; имена файлов, содержимое и режимы Git сохранены. Авторитетная замена означает архитектурное направление, а не заявление о полном функциональном паритете.

| Исходный путь | Новый путь | Исходное назначение | Авторитетная замена | Безопасность выполнения | Условие снятия | SHA-256 |
|---|---|---|---|---|---|---|
| `render.cpp` → `legacy/applications/render.cpp` | `retired_standalone_renderers.md` (`RETIRED`) | автономный CLI-рендерер | формула/число/compute/scheduler/raster/encoder/sink | удалён; восстановим из Git | аудит знаний завершён | `6e40c8c23a52f8deffde730b6c34816152ee29cfa7b8e9830c8b3fafd94a1b60` |
| `render_engine.cpp` → `legacy/applications/render_engine.cpp` | `retired_standalone_renderers.md` (`RETIRED`) | image/HD/video и ffmpeg | формула/число/compute/scheduler/raster/encoder/sink | удалён; восстановим из Git | аудит знаний завершён | `e9f683a9396ccf722db55d8f3276c16f7431738d43bd03d74e5e8ffb2ab668c5` |
| `server.cpp` → `legacy/servers/server.cpp` | `retired_native_http_server.md` (`RETIRED`) | исходный C++ HTTP-сервер | `shared/` + `runtime/`; будущая служба отдельно | удалён по разрешению владельца; восстановим из Git | решение выполнено | `ff2c3d6464962298dc040ece72ae3a631da55b4ba6a3195b6b4f3a7074e5bbe1` |
| `server.py` → `legacy/servers/server.py` | `retired_python_server.md` (`RETIRED`) | исходный Python-сервер и средство запуска | `shared/` + `runtime/`; сеть не реализуется этим этапом | удалён по разрешению владельца; восстановим из Git | решение выполнено; иные компоненты не затронуты | `81fa70a1a47321a4b122b0048f72ce2c3c9422f55a6d30a69d51394f7733f214` |
| `app_server` → `legacy/binaries/app_server` | `retired_foreign_executable_bundle.md` (`RETIRED`) | собранный сервер | `shared/` + `runtime/` | удалён; восстановим из Git | решение выполнено | `3258c1c944adaee9cf60de606b9629f29b04f15b259e3df3556ec6862bdcf749` |
| `render` → `legacy/binaries/render` | `retired_foreign_executable_bundle.md` (`RETIRED`) | собранный CLI-рендерер | `retired_standalone_renderers.md` | удалён; восстановим из Git | решение выполнено | `69a50f78f8787ded0f56d1b8570d44366d1f824ce814204477eed75a1c33be08` |
| `render_engine` → `legacy/binaries/render_engine` | `retired_foreign_executable_bundle.md` (`RETIRED`) | собранный генератор изображений и видео | `retired_standalone_renderers.md` | удалён; восстановим из Git | решение выполнено | `6d5868a29e4f9be0b2d65bd55040bf9249af27324087266832ea75d3f47da19f` |

Старые пути намеренно не дублируются. Исторические документы могут упоминать их как координаты прежнего состояния; для фактического доступа следует применять эту карту.
