# Краткий обзор безопасного стека

Fractal Studio выбирает OpenSSL 3.5 LTS для backend TLS и cryptographic operations, Android platform TLS для Android, ECDSA P-256 with SHA-256 для service/device proof, Android Keystore для неэкспортируемого device signing key, deterministic CBOR profile для wire format, Unix-domain socket и named pipe для local loopback, а TLS 1.3 over bounded framed byte stream для remote transport. Все реализации остаются за platform seams. Semantic identity Workspace, pairing, replay и reacquire не зависит от bytes codec или patch version library.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
