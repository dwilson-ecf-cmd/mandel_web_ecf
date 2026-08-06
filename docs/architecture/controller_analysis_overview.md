# Desktop controller и checkpoint analysis

Desktop controller управляет локальным Workspace service через typed operations: open, load-config, save-config, set-viewport, set-formula, start, status, cancel, wait, show-analysis и show-artifact. Он не является GUI и не получает прямого доступа к каноническому Workspace state.

Быстрые изменения viewport supersede промежуточные generations. Интерактивный render имеет приоритет над deferred analysis: pending analysis для старого generation отменяется или пропускается, а newest generation получает чистый execution lifecycle.

Analysis запускается только на stable immutable generation. В режиме CHECKPOINT она выполняется после publication, а в режиме DEFERRED ждёт bounded idle interval. Results, preview и artifact остаются attached к тому generation, который их произвёл, и не переносятся на новый generation.

Saved RenderConfiguration — это reusable preset пользователя. Она не является Workspace checkpoint и не содержит service/session/replay/restart state. Будущий GUI должен использовать этот controller без переноса service, runtime, analysis или persistence logic в graphical shell.

## English summary

The Desktop controller drives the local Workspace service without GUI logic. Rapid edits supersede intermediate generations, rendering has priority over deferred analysis, analysis attaches only to stable immutable generations, saved configurations are not checkpoints, and the future GUI should reuse this controller unchanged.
