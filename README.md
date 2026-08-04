# Fractal Studio: нативная вычислительная основа

Fractal Studio сохраняет исходную Mandelbrot Studio участника проекта и
развивает рядом с ней переносимую нативную основу на C11. Русский язык является
авторитетным для актуальной технической документации и комментариев. Правила
заданы в `docs/architecture/language_policy.md`, а утверждённые термины — в
`docs/architecture/terminology_glossary.md`.

**English-speaking maintainer:** see
[docs/maintainer_guide.en.md](docs/maintainer_guide.en.md) for the repository
layout, Windows/MSYS2 build and test commands, architecture summary, frozen
identities, and guidance for reviewing Russian documentation.

Английский документ фиксирует предшествующую русскую техническую основу;
текущее состояние сокета формулы и намеренные изменения идентичности описаны
в русскоязычном `docs/architecture/formula_socket.md` и в итоговом отчёте
сопровождающему.

## Что реализовано

Авторитетная продуктовая модель теперь определяет постоянную Workspace, общие
типизированные намерения Desktop/Mobile и самодостаточные снимки службы. Малый
ограниченный редуктор C11 служит оракулом принятия, ревизий, поколений, ролей и
единственной аренды управления; транспорт, GUI и проверка подлинности намеренно
не выбраны. Обзор находится в
`docs/architecture/fractal_studio_overview.md`.

Авторитетный статический реестр собирает завершённую исполняемую цепочку:

```text
fractal.formula.mandelbrot.v1 or fractal.formula.julia.v1
    -> fractal.numeric.binary64.v1
    -> fractal.compute.scalar.v1
    -> serial or deterministic bounded thread-pool scheduler
    -> bounded analyzer chain
    -> raster.native.iteration-bgr8
    -> encoder.bmp.v3
    -> transactional artifact sink
```

Реестр содержит 22 модуля и имеет идентичность `616b9a3b92e787e5`.
Поддерживаются независимо зарегистрированные формулы Mandelbrot и Julia,
версионированный сокет формулы, единственная числовая реализация
Binary64, последовательный планировщик и детерминированный ограниченный пул
потоков, цепочка не более чем из восьми анализаторов, BGR8, BMP v3, системная
память и транзакционные приёмники памяти и файла. Цепочка анализаторов может
включать сквозной анализатор, сводку выхода и классификации, гистограмму
итераций и пространственную сетку нагрузки.

Вычисление должно успешно завершить всё поле до анализа. Анализ выполняется
последовательно; растеризация и кодирование начинаются только после его успеха.
Артефакт публикуется только после успешного `commit`; при отмене или отказе
активный приёмник получает `abort` и частичный результат не публикуется.

Зафиксированные результаты 32 × 24 с бюджетом 64:

| Формула | Поле | Пиксели | BMP | Байты |
|---|---|---|---|---:|
| Mandelbrot | `99ec88c2a0f8bac3` | `4866aacc38290b5f` | `fb1a83bd5ca28e5f` | 2358 |
| Julia (-0.8, 0.156) | `0fb4458e08bad6e1` | `b272f08b0bbdca2b` | `4d4aa95bd137ec87` | 2358 |

Идентичность цепочки из трёх анализаторов — `0116ac94c4b64d99`, идентичность
выполнения Binary64 — `2f300bf0f7ea5244`. Полные архитектурные сведения
находятся в `docs/architecture/`.

## Сборка и проверка в Windows/MSYS2 MinGW64

Из корня репозитория в оболочке MSYS2 MinGW64:

```sh
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Сборка требует CMake 3.16 или новее, компилятор C11, компилятор C++17 для
границы унаследованного адаптера и нативные потоки. Python не является
производственной зависимостью. Проверка не запускает сохранённые AArch64
Android/Termux файлы.

## Происхождение Mandelbrot Studio

Репозиторий сохраняет приложение Mandelbrot Studio участника проекта:
снятые с эксплуатации автономные рендереры, описанные в `docs/architecture/retired_standalone_renderers.md`, и снятые с эксплуатации Python- и C++-серверы, описанные в `docs/architecture/retired_python_server.md` и `docs/architecture/retired_native_http_server.md`, русскоязычный браузерный интерфейс `index.html` и `style.css`, снятый комплект иностранных программ, описанный в `docs/architecture/retired_foreign_executable_bundle.md`, изображения, 750
кадров и видео. Исходный русский текст и исторические артефакты не переводятся
и не удаляются.

Сохранённые исполняемые файлы — 64-битные little-endian AArch64 Android/Termux
ELF, а не программы Windows или Linux x86-64. При сборке и тестах их нельзя
запускать. Воспроизводимого рецепта их исходной сборки в репозитории нет.
`legacy/servers/server.py` снят с эксплуатации; его история и контрольная сумма сохранены в Git и в `docs/architecture/retired_python_server.md`. Корневые BMP, `render.mp4` и `frames/` являются
историческим сгенерированным выводом участника проекта; новые продукты должны
размещаться в `runtime/artifacts/` или во внешнем хранилище.

## Явные ограничения

В репозитории пока нет дополнительных формул в нативной основе, произвольной
точности, GPU, SIMD, приложения Android, JNI, активной
памяти Ouro или работающего уточнения CDC. Исчерпание бюджета остаётся
классификацией unresolved и не является доказательством ограниченности. Никакая
из этих незавершённых возможностей не заявляется как поддерживаемая.

`CDC.pdf` и исследовательские материалы CDC сохраняются без изменения; его
авторитетная SHA-256:
`5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c`.

Завершённый сокет формулы описан в
`docs/architecture/formula_socket.md`. Текущее состояние дальнейшего плана
зафиксировано в `docs/roadmap/next_attack_plan.md` без объявления новых
поддерживаемых возможностей.

## Secure pairing contract

Fractal Studio now defines a security-contract layer for pairing unknown clients into recognized Workspace participants. The contract separates discovery, identity proof, human approval, capability assignment, credential lifecycle, and revocation without adding sockets, cryptographic libraries, Android APIs, GUI code, cloud SDKs, or host-machine control. Russian source documentation is in `docs/security/`.
