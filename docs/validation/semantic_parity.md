# Детерминированная семантическая паритетность

Semantic parity layer выполняется только на platform-neutral fake infrastructure. Fault injection доступен на stable scenario step и fault identity для NONE, DROP_MESSAGE, DUPLICATE_MESSAGE, DELAY_MESSAGE, REORDER_MESSAGES, CORRUPT_FRAME, DROP_ACK, DISCONNECT, RECONNECT, NETWORK_CHANGE, CLIENT_SUSPEND, CLIENT_RESUME, SERVICE_RESTART, SERVICE_IDENTITY_ROTATION, CREDENTIAL_REVOCATION и CAPABILITY_CHANGE.

Lost acknowledgment моделируется как accepted intent с потерянным acknowledgment и повторной отправкой того же intent после reconnect. Replay cache возвращает original result, revision и generation не увеличиваются повторно, preview и artifact references не дублируются.

English summary: describes deterministic fake-transport scenarios, fault coverage, and the lost-acknowledgment invariant.
