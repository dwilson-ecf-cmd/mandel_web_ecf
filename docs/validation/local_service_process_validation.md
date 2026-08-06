# Проверка локального процесса Workspace service

Проверка охватывает independent process path: запуск сервиса, локальный клиент, compatibility negotiation, operator session, Workspace open, snapshot, lease, render-affecting intent, non-render intent, reacquire, lost-ack replay, duplicate detection, checkpoint restore, malformed frame и clean shutdown.

Нормализованное сравнение исключает PID, endpoint path, OS peer identity, file descriptor, timing, границы chunk и restart-local session id. Ожидаемый semantic drift равен 0 для in-process harness, fake transport и Unix-domain local service seam. Windows named-pipe остаётся COMPILE_ONLY до запуска на Windows.

## English summary

The validation document defines the process scenarios and semantic-drift normalization for the local Workspace service. Linux process validation is available; Windows named-pipe runtime validation remains pending.
