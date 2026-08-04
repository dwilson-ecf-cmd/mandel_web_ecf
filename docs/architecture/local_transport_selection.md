# Выбор local loopback transport

Linux выбирает Unix-domain socket, Windows выбирает named pipe, test profile использует deterministic in-memory adapter. Endpoint name derived from installation/service identity, а не из user input. Доступ по умолчанию restricted to service account/user. Frames bounded и совпадают с remote semantic messages. OS-local peer credentials используются где доступны. Raw handles не выходят за platform adapters.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
