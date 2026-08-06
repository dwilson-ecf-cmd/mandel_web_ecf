# Отчёт о semantic drift

Normalized semantic result включает scenario identity, Workspace identity, final revision, final generation, snapshot identity, controller lease summary, participant capability summary, accepted and rejected intent identities, replay-result identities, preview identity, analysis identity, artifact identities, service restart epoch и reacquisition result.

Исключены file descriptors, pipe names, socket paths, TLS handles, cipher-record bytes, OS error numbers, timing jitter, thread IDs и process IDs. Drift report содержит expected transport, observed transport, scenario identity, mismatched fields, expected identities, observed identities и semantic drift count. Success требует `semantic drift count = 0`.

English summary: documents the normalized result model and bounded drift report.
