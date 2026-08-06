# Модель транспортных отказов

Fault driver имеет пять уровней: semantic-message, frame, transport lifecycle, service lifecycle и security/session. Duplicate message относится к semantic-message layer; truncated payload относится к frame layer; disconnect относится к transport lifecycle; service restart относится к service lifecycle; wrong certificate относится к security/session layer.

Категории не смешиваются: каждый injected fault имеет stable scenario step, deterministic fault identity, kind и layer. Это позволяет изолировать regression как semantic, platform или security problem.

English summary: defines bounded fault layers and stable fault identities for diagnosis.
