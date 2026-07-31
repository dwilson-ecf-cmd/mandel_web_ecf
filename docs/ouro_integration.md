# Future Ouro integration

Ouro is an optional embedded Android/native memory backend, independent of renderer choice.
A future approved milestone may provide scoped allocation for render-job/frame/tile lifetimes,
allocation telemetry, runtime diagnostics, lifecycle accounting, failure evidence, and
Android-native embedding. This milestone imports no Ouro source. System allocation remains
the reference/default, while Ouro initialization explicitly returns
`FRACTAL_ERROR_NOT_IMPLEMENTED`. Allocator behavior cannot prove CDC or conventional renderer
correctness; renderer results cannot prove allocator correctness.
