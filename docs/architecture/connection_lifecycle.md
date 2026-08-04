# Жизненный цикл соединения

Документ фиксирует формальный контракт кампании платформенных швов. Машинные идентификаторы остаются английскими, а смысловые правила описаны русским техническим языком.

## Инварианты

- Платформенные сборки разделяют семантические контракты, а не платформенные зависимости.
- Workspace остается авторитетным при разрыве соединения, перезапуске сервиса и потере процесса клиента.
- Повторное соединение восстанавливает полномочия из сервиса; кэш клиента является только рекомендательным.
- Одинаковый принятый intent возвращает тот же семантический результат и не применяется повторно.
- Изменение реализации backend не требует обновления клиента, пока опубликованный согласованный контракт совместим.

## Контракт v1

- `platform.core`, `platform.secure-storage`, `platform.random`, `platform.transport`, `platform.lifecycle` и `platform.discovery` являются ограниченными группами интерфейсов; общий глобальный vtable запрещен.
- `CompatibilityHello` выбирает максимальную общую версию по каждому диапазону и отделяет обязательные возможности от необязательных.
- Состояния соединения: `UNINITIALIZED`, `DISCONNECTED`, `DISCOVERING`, `CONNECTING`, `NEGOTIATING`, `AUTHENTICATING`, `REACQUIRING`, `SYNCHRONIZED`, `DEGRADED`, `SUSPENDED`, `FAILED`, `REVOKED`, `INCOMPATIBLE`.
- `ReacquireRequest` передает последние известные ревизии, поколения, идентичности предпросмотра, анализов и артефактов, но не объявляет кэш авторитетным.
- Ответ reacquire возвращает полный snapshot, ограниченную delta или точную причину отказа: отсутствие Workspace, отзыв доступа, отзыв credential, смену service identity, repair, upgrade.
- Повтор intent определяется тройкой: participant ID, command sequence, intent identity. Иной payload на той же последовательности дает `COMMAND_SEQUENCE_CONFLICT`; слишком старое окно дает `REPLAY_HISTORY_EXPIRED`.
- Потерянное подтверждение восстанавливается повторной отправкой того же envelope; revision и generation не увеличиваются второй раз.
- Предпросмотр, analysis и artifact возобновляются отдельно от Workspace через generation, revision, identity и checksum.
- `SERVICE_RESTART`, `SERVICE_IDENTITY_ROTATION` и `WORKSPACE_STATE_LOSS` являются разными событиями.
- Offline mode только для чтения; мутации являются черновиками и не считаются принятыми.

## English summary

This document defines the v1 platform seam, compatibility, reconnect, retry, reacquisition, replay, preview/artifact resume, service restart, and Android lifecycle rules. Android, desktop, Linux backend, and Windows backend share contracts while keeping platform APIs behind bounded seams.
