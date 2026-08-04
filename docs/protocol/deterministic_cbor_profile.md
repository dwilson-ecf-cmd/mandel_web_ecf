# Детерминированный профиль CBOR

Профиль основан на RFC 8949 и является authority для wire encoding.

## Правила v1

- Только definite-length items; indefinite-length encoding запрещен.
- Maximum message 1024 bytes, string 256 bytes, byte-string 1024 bytes, array 32 entries, map 16 pairs, nesting depth 4.
- Map keys являются integer field labels; free-form string labels запрещены.
- Keys sorted по детерминированному порядку encoded key bytes; для v1 integer labels это ascending numeric order with shortest encoding.
- Shortest integer encoding обязателен.
- Duplicate map keys rejected.
- Unknown optional fields ignored только если contract version разрешает это поле как optional.
- Unknown required fields rejected; в scaffolding labels >=1000 считаются unknown required.
- Invalid UTF-8 rejected for text fields.
- NaN и infinity запрещены, если конкретное поле явно не разрешило их.
- Integer и floating-point не конвертируются неявно.
- Binary64 представляется только как explicit binary64 field, canonical big-endian IEEE 754 payload, и не входит в integer slots.
- Trailing bytes rejected; decoder fails closed.
- Serialized CBOR bytes не являются semantic identity Workspace objects; identity продолжает вычисляться из normalized semantic fields.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
