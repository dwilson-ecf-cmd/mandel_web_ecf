# Следующий план атаки

Текущая кампания выбрала безопасный implementation stack: OpenSSL 3.5 LTS для backend TLS/crypto, Android platform TLS, Android Keystore, ECDSA P-256 with SHA-256, deterministic CBOR profile, bounded message framing, Unix-domain socket, Windows named pipe и TLS 1.3 framed remote stream. Реальные public listener, production keys, Android/JNI/Gradle, HTTP/WebSocket/gRPC/QUIC, compression, GUI, cloud SDK и database не добавлены.

Рекомендуемый следующий milestone: реализовать bounded local loopback reference service и desktop client adapter с выбранным framing и serialization profile, оставив remote TLS и Android implementation выключенными.

## Следующий milestone

Implement the bounded local Workspace reference service and non-visual desktop client core as independently executable processes, using the validated local transport path while keeping remote access disabled.


## Next milestone

Build the first non-visual desktop Workspace controller workflow over the validated local service, including saved local configurations and live runtime progress, before selecting the graphical toolkit.


## Next milestone

Select and implement the first graphical Desktop shell over the proven Workspace controller, without moving service, runtime, analysis, or persistence logic into the GUI.
