# Политика зависимостей и уязвимостей

- Major/minor dependency policy pinned: OpenSSL остается в линии 3.5 LTS до отдельного решения.
- Patch versions отслеживаются и обновляются; уязвимый patch не фиксируется бессрочно.
- Security advisories проверяются по cadence и emergency procedure.
- Unsupported cryptographic releases запрещены.
- Dependency inventory должен быть воспроизводимым, с source и license verification.
- Vulnerability scanning добавляется в CI позже; negative tests сохраняются при upgrades.
- OpenSSL patch upgrade не меняет protocol identity.
- Protocol compatibility не привязана к library patch version.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
