# Artifact sinks

An artifact sink owns destination behavior; an encoder owns bytes. The socket is a distinct `ARTIFACT_SINK` module kind with binary, commit/abort, memory, and file capability flags. Its lifecycle is `begin` → zero or more `write` calls → exactly one `commit`, or `abort`. A zero-length write is valid while active. Writes, commits, and begins in invalid phases are rejected, and byte arithmetic is overflow checked.

The bounded memory sink never allocates. Its buffer remains caller-owned, overflow returns `BUFFER_TOO_SMALL`, and `required_size` records bytes requested through the failing call. Abort discards its logical length. The host-file sink accepts only caller-selected relative paths without `..`, writes `<target>.tmp`, flushes/closes, and renames on commit. Abort removes the temporary. Rename atomicity depends on the host filesystem; no cross-filesystem guarantee is claimed.

Checksums are accumulated as bytes pass through the destination and independently by the runtime writer decorator. Memory and file destinations therefore require no reread and must agree. FNV-1a is used for reproducibility, not security. Future Android content/file-descriptor and stream sinks can implement this contract; neither exists today.
