# Обзор транспортной паритетности

Одна и та же коллекция Workspace scenarios запускается через три слоя: fake transport, local platform transport и secure TLS transport. Если результат отличается, failure изолируется как semantic, platform или security problem.

Потерянный acknowledgment не дублирует edits: повторная отправка identical intent возвращает original replay result. TLS защищает traffic, но не определяет Workspace meaning. Windows execution и Android rows остаются deferred до появления соответствующих implementations. Success означает `semantic drift = 0`.

English summary: concise collaborator overview of the three-layer campaign and zero-drift success rule.
