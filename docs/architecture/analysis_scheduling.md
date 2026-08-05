# analysis_scheduling

## Назначение

Документ фиксирует не-визуальный Desktop controller и checkpoint-oriented workflow для локального Workspace service. Контроллер выражает только typed operations и не изменяет каноническое состояние Workspace напрямую.

## Правила

- RenderConfiguration является reusable user preset и не содержит Workspace identity, participants, lease, service identity, replay cache, command sequence, restart epoch, execution state или checkpoint internals.
- Workspace остаётся авторитетным collaborative state, а ReferenceCheckpoint остаётся внутренним crash/restart recovery механизмом.
- Runtime control имеет состояния IDLE, QUEUED, PREPARING, RENDERING, ANALYSIS_PENDING, ANALYZING, PUBLISHING, COMPLETED, CANCELLING, CANCELLED, FAILED и SUPERSEDED.
- Progress и preview являются observational snapshots: они не меняют Workspace revision и не создают generation.
- FRACTAL_ANALYSIS_OFF отключает автоматический анализ; FRACTAL_ANALYSIS_CHECKPOINT запускает анализ после publication; FRACTAL_ANALYSIS_DEFERRED ждёт bounded idle interval и анализирует только newest completed generation.
- Every preview, analysis result, and artifact is generation-bound; superseded result не может быть attached к текущему generation.
- Analysis cost observability собирает requested, skipped, cancelled, completed jobs, elapsed ticks, ratio и selected analyzer count, но не меняет policy автоматически.

## English summary

This Russian document freezes the non-visual Desktop controller workflow, RenderConfiguration separation from Workspace/checkpoint state, runtime progress/publication, generation-bound previews/artifacts/analysis, and OFF/CHECKPOINT/DEFERRED analysis scheduling.
