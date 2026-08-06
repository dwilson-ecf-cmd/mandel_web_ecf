# Выбор remote secure transport

Remote profile выбирает TLS 1.3 over bounded reliable byte stream. Backend terminates TLS; Android validates approved service identity; pairing transcript выполняет mutual proof и capability authorization. Workspace messages начинаются только после compatibility negotiation и authentication. Android never connects directly to workers. Public gateway и backend могут быть разделены позже. Core contract не требует HTTP, WebSocket, gRPC или QUIC, и plaintext discovery не дает trust.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
