# Стратегия транспортной валидации

Документ задаёт кампанию из трёх слоёв: детерминированный fake transport, локальный platform transport и test-only secure transport. Единый инвариант: transport не меняет meaning Workspace. Все слои используют общий scenario contract, общий client/service harness и authoritative Workspace reducers; отдельные Workspace, pairing, replay, compatibility или reacquisition semantics запрещены.

Semantic drift определяется как любое различие в authoritative semantic output. Успешный fixture обязан завершаться значением `semantic drift = 0`.

English summary: defines the three-layer validation campaign and the zero semantic drift invariant.
