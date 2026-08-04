# Выбор реализаций безопасного стека

## Запись решений

| Поверхность | Статус | Выбор | Отклонено | Причина и жизненный цикл | Граница владения |
|---|---|---|---|---|---|
| Backend TLS и crypto | SELECTED | OpenSSL 3.5 LTS, patched release в линии 3.5 | OpenSSL 4.x для v1, TLS 1.2, low-level crypto API | 3.5 является LTS до 2030-04-08; patch обновления разрешены без смены protocol identity. | Только platform adapter; shared headers не содержат OpenSSL types. |
| Android TLS | SELECTED | Android platform TLS | Bundled backend OpenSSL, trust-all manager, hostname bypass, plaintext fallback | Network Security Configuration поддерживает cleartext opt-out, custom anchors, debug overrides и pinning. | Android transport adapter. |
| Signing identity | SELECTED | ECDSA P-256 with SHA-256 | Один долгосрочный AES key как identity | Подходит для device proof, service proof, pairing transcript и certificate public keys. | Pairing/security adapter; semantic identity остается независимой. |
| Transport encryption | SELECTED | TLS 1.3 AEAD, preferred TLS_AES_256_GCM_SHA384 | 0-RTT mutating intents, application data before handshake | TLS выводит ephemeral session keys; приложение их не обменивает вручную. | platform.transport + TLS adapter. |
| C CBOR codec | SELECTED_WITH_GATES | Малый bounded Fractal reference codec для профиля и vectors | Large third-party import без license review | Достаточен для conformance scaffolding; production C library выбирается после license/dependency audit. | shared protocol test harness. |
| Android CBOR codec | DEFERRED | Wire specification + vectors являются authority | Kotlin default CBOR как protocol authority | Default Android-side library must prove byte-for-byte deterministic vectors first. | Android adapter later. |
| Local transport | SELECTED_WITH_GATES | Linux Unix-domain socket, Windows named pipe, Test in-memory | TCP port for integrated local use | OS-local transport остается за seam, endpoint derived from service identity. | platform.transport local adapter. |
| Remote transport | SELECTED | TLS 1.3 over bounded reliable byte stream | HTTP/WebSocket/gRPC/QUIC dependency for core v1 | Core needs framed bytes, not application protocol semantics. | remote transport adapter. |

## Неразрешенные риски

- Нужно выбрать production C CBOR library или утвердить малый audited codec.
- Требуется реальный certificate rotation runbook.
- Требуется CI-инвентаризация зависимостей и advisory monitoring.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
