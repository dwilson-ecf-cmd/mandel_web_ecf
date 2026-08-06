# Локальный reference service

Local reference service является bounded validation component, а не production daemon. Он запускается детерминированно для test campaign, обслуживает только local endpoint, использует authoritative Workspace reducers и завершает работу после сценария. Public listener, discovery, rendezvous, database и persistent service запрещены.

Process boundary validation доказывает, что client и service могут обмениваться framed bytes через OS local transport без изменения Workspace meaning.

English summary: describes the non-daemon local reference service harness and process-boundary rule.
