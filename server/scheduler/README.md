# Scheduler

The runtime installs the unchanged serial compatibility scheduler, its exact
`fractal.scheduler.serial.v1` registry alias, and the bounded deterministic
`fractal.scheduler.thread-pool.v1` implementation. The thread-pool scheduler
uses sealed contiguous-row assignments and fixed scheduler-owned storage. See
`docs/architecture/deterministic_thread_pool_scheduler.md` for its ownership,
decomposition, cancellation, failure, and lifecycle contract.
