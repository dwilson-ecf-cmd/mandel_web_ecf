# Кроссплатформенная матрица

| Client | Transport | Service | Status |
| --- | --- | --- | --- |
| Reference client | Fake | Reference service | PASSED |
| Desktop test client | Unix socket | Linux service | PASSED |
| Desktop test client | Named pipe | Windows service | COMPILE_ONLY |
| Desktop test client | TLS 1.3 | Linux service | BLOCKED |
| Desktop test client | TLS 1.3 | Windows service | DEFERRED |
| Android | TLS 1.3 | Linux service | DEFERRED |
| Android | TLS 1.3 | Windows service | DEFERRED |

Android parity не заявляется до появления Android implementation. Windows execution не заявляется в Linux environment.

Real TLS 1.3 Linux execution is BLOCKED in environments that provide OpenSSL 3.0 instead of the selected OpenSSL 3.5 LTS path; semantic/security-contract tests still run separately and must not be reported as real TLS execution.

English summary: gives honest platform status without claiming unavailable Windows, Android, or OpenSSL 3.5 execution.
