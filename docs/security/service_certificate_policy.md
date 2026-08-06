# Политика сертификатов сервиса

## Development

- Локально генерируемая service identity допускается только как development identity.
- Пользователь явно подтверждает fingerprint; trust claim production отсутствует.
- Validity короткая, reset простой, UI/лог явно маркирует development mode.

## Production

- Stable service identity связывается с certificate или public key.
- Rotation использует overlapping trust window, key-version tracking и revocation handling.
- Silent identity replacement запрещен; expiry alerts и recovery procedure обязательны.
- Первичный профиль допускает оба назначения: public-CA certificate для stable DNS gateway и private service certificate pinned through pairing для лабораторной identity. Paired service-identity proof остается authoritative даже при public-CA TLS.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
