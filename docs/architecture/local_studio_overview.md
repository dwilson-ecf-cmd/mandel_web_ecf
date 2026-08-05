# Локальный Fractal Studio

Fractal Studio теперь описан и проверяется как две локальные части: не-визуальное ядро desktop client и авторитетный Workspace service. Клиент подключается к локальному транспорту, проходит compatibility negotiation, открывает локальный operator session и отправляет типизированные CLIENT_INTENT.

Workspace переживает отключение клиента. Перезапуск сервиса является отдельным событием: Workspace не закрывается, а восстанавливается из REFERENCE_CHECKPOINT_V1 через состояние RESTORING. Reconnect, reacquire и replay-result cache предотвращают повторное применение уже принятого intent после потерянного ответа.

Удалённого listener, Android-реализации и GUI в этом этапе нет. Будущий GUI должен использовать существующее ядро desktop client, не обходя сервис и reducer.

## English summary

Fractal Studio is now framed as a local desktop client core plus an authoritative local Workspace service. Disconnect, restart, replay, and reacquisition are separate semantics; no remote listener, Android client, or GUI is implemented yet.
