# Паритетность локального транспорта

Local parity запускает те же scenarios через fake transport и доступный Unix-domain socket adapter. Linux validation использует local endpoint, restrictive permissions, bounded framing, overflow-safe parsing, cancellation и clean shutdown; TCP listener и public interface запрещены. Хотя test harness применяет socketpair для process-boundary proof, raw file descriptors остаются в Linux-specific test source.

Windows named pipe получает compile-safe contract only в текущей Linux environment. Execution status — COMPILE_ONLY до запуска на Windows; socket/TCP fallback запрещён.

English summary: records Unix-socket parity, process-boundary validation, and honest Windows named-pipe status.
