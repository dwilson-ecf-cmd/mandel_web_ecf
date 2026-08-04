# Формат ограниченного сообщения

Frame v1 содержит: magic `FSF1`, frame version, message class, flags, payload length, session sequence и payload.

## Требования

- Byte order фиксирован как big-endian.
- Maximum payload size: 1024 bytes.
- Length parsing overflow-safe; allocation не выполняется на основании untrusted length.
- Frame consumption exact: trailing или concatenated ambiguity rejected.
- Session sequence доступна для diagnostics и replay handling, но не является Workspace command sequence.
- Payload содержит ровно один deterministic CBOR object.
- Framing identity отделена от semantic message identity.
- Compression отсутствует в v1.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
