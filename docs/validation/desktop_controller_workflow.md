# Проверка Desktop controller workflow

Проверка сравнивает normalized results для in-process controller workflow, fake transport workflow и Unix local process workflow. Поля PID, endpoint path, timing, transport chunk boundary и restart-local session id исключаются из semantic comparison.

Сценарии доказывают: progress не меняет Workspace revision; preview не создаёт generation; analysis attachment ссылается на правильный generation; DEFERRED ждёт rapid edits; анализируется только newest stable generation; historical analysis сохраняется; superseded analysis не публикуется в current generation; save/load RenderConfiguration детерминирован; configuration не содержит Workspace/session/checkpoint state; lost acknowledgment применяется один раз; reacquire сохраняет runtime observation.

## English summary

The validation plan defines deterministic controller scenarios and semantic-drift normalization for progress, preview, deferred analysis, supersession, configuration round-trip, replay, reconnect, and frozen runtime identity preservation.
