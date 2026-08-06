# Матрица применимости платформенных реализаций

| Surface | Linux backend | Windows backend | Desktop | Android |
|---|---|---|---|---|
| TLS | SUPPORTED: OpenSSL 3.5 | SUPPORTED_WITH_FALLBACK: OpenSSL 3.5 or platform adapter | SUPPORTED_WITH_FALLBACK: selected adapter | SUPPORTED: Android platform TLS |
| Signing key | SUPPORTED_WITH_FALLBACK: TPM/HSM/PKCS#11 or hardened fallback | SUPPORTED_WITH_FALLBACK: CNG/provider | DEFERRED: participant as needed | SUPPORTED_WITH_FALLBACK: Android Keystore StrongBox/TEE |
| Secure storage | SUPPORTED_WITH_FALLBACK: TPM/PKCS#11 or hardened fallback | SUPPORTED_WITH_FALLBACK: CNG/protected storage | DEFERRED: local credential storage | SUPPORTED_WITH_FALLBACK: Android Keystore |
| CBOR | SUPPORTED_WITH_FALLBACK: bounded reference codec; production library gated | SUPPORTED_WITH_FALLBACK: same C profile | SUPPORTED_WITH_FALLBACK: shared/client codec | DEFERRED: validated Android codec required |
| Local transport | SUPPORTED_WITH_FALLBACK: Unix socket | SUPPORTED_WITH_FALLBACK: named pipe | SUPPORTED_WITH_FALLBACK: platform adapter | NOT_APPLICABLE |
| Remote transport | SUPPORTED: TLS stream | SUPPORTED: TLS stream | SUPPORTED: TLS client | SUPPORTED: TLS client |

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
