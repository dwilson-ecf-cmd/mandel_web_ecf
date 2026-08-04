# Паритетность защищённого транспорта

Secure parity сравнивает normalized semantic results между fake transport, local Unix transport и TLS 1.3 loopback transport. TLS session identifiers, encrypted bytes, ephemeral keys, certificate serials and timing исключены из semantic comparison.

Pairing-contract proof и secure transport proof разделены: deterministic prevalidated proof остаётся semantic fixture, а TLS adapter обязан выполнять real TLS 1.3 handshake там, где доступен утверждённый OpenSSL 3.5 LTS test path. Ключи и certificates являются ephemeral, non-production, build/test-only и не отслеживаются Git.

English summary: explains TLS parity scope, excluded TLS metadata, and separation between pairing semantics and transport protection.
