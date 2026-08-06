# local_operator_trust

## Назначение

Документ фиксирует локальную архитектуру Fractal Studio для первого сервиса Workspace. Workspace имеет собственный жизненный цикл, отдельный от процесса сервиса и клиентских сеансов.

## Зафиксированные правила

- Локальный транспорт остаётся только локальным: Unix-domain socket на Linux и named pipe со статусом COMPILE_ONLY на Windows.
- Клиент не изменяет каноническое состояние Workspace напрямую; все CLIENT_INTENT проходят через авторитетный reducer.
- Локальный оператор получает явный профиль capability, а lease, revision, replay и reacquire сохраняются обязательными.
- Перезапуск сервиса переводит Workspace через RESTORING и не означает CLOSE.
- Формат checkpoint помечен как REFERENCE_CHECKPOINT_V1 и не является долгосрочным дисковым ABI.

## English summary

This Russian document freezes the local-only Workspace service boundary, separating Workspace lifetime from process lifetime and requiring reducer authority, replay protection, reacquisition, bounded framing, local peer admission, and reference checkpoint recovery.
