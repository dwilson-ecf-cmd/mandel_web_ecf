# Протокольные test vectors

Vectors являются wire authority для будущих C, Windows и Android codecs.

| Vector | Semantic fields | Encoded bytes | Expected | Semantic identity namespace | Framing metadata |
|---|---|---|---|---|---|
| compatibility hello | `{1:1,2:300,3:5}` | `a3 01 01 02 19 01 2c 03 05` | decode same map | `protocol.vector.compatibility.v1` | class compatibility, seq 42 |
| pairing invitation | `{1:1,4:7}` | `a2 01 01 04 07` | decode same map | `protocol.vector.pairing-invitation.v1` | class pairing |
| client hello | `{1:1,5:13}` | `a2 01 01 05 0d` | decode same map | `protocol.vector.client-hello.v1` | class compatibility |
| service hello | `{1:1,6:13}` | `a2 01 01 06 0d` | decode same map | `protocol.vector.service-hello.v1` | class compatibility |
| proof transcript | `{1:1,7:256}` | `a2 01 01 07 19 01 00` | decode same map | `protocol.vector.proof.v1` | class pairing |
| approval | `{1:1,8:1}` | `a2 01 01 08 01` | decode same map | `protocol.vector.approval.v1` | class pairing |
| credential metadata | `{1:1,9:3}` | `a2 01 01 09 03` | decode same map | `protocol.vector.credential.v1` | class pairing |
| Workspace intent | `{1:1,10:44}` | `a2 01 01 0a 18 2c` | decode same map | `protocol.vector.workspace-intent.v1` | class workspace |
| Workspace snapshot summary | `{1:1,11:45}` | `a2 01 01 0b 18 2d` | decode same map | `protocol.vector.workspace-snapshot.v1` | class workspace |
| reacquisition request | `{1:1,12:46}` | `a2 01 01 0c 18 2e` | decode same map | `protocol.vector.reacquire-request.v1` | class reacquire |
| reacquisition response | `{1:1,13:47}` | `a2 01 01 0d 18 2f` | decode same map | `protocol.vector.reacquire-response.v1` | class reacquire |
| replay-result record | `{1:1,14:48}` | `a2 01 01 0e 18 30` | decode same map | `protocol.vector.replay-record.v1` | class replay |

## Negative vectors

- Malformed frame: wrong magic.
- Duplicate keys: `a2 01 01 01 02`.
- Oversized length: payload length > 1024.
- Unknown optional field: accepted only when required gate is false.
- Unknown required field: label 1000 rejected.
- Invalid UTF-8: rejected by text-field decoders.
- Truncated payload: rejected by exact length.
- Trailing bytes: rejected by exact consumption.

## English summary

This document records the selected secure implementation stack for Fractal Studio while keeping concrete libraries behind the established platform seams. It does not introduce public listeners, production keys, Android code, JNI, Gradle, GUI code, cloud SDKs, databases, HTTP/WebSocket/gRPC/QUIC, compression, or complete TLS networking.
