# Fractal computation substrate

## Data flow and ownership

```text
computation problem
  -> computation backend
  -> backend-neutral point result and evidence
  -> renderer/rasterizer
  -> pixels or platform presentation
```

Computation, rendering, and memory are independent axes. The portable computation API contains no Android, JNI, Kotlin, allocator selection, pixels, palette, image, tile, or platform-graphics concept. It validates a formula-level problem and computes one point. A future rasterizer may consume point or region results; it must not own CDC mathematics.

## Computation identities

- `legacy-reference`: identity for inherited behavior; point computation is unavailable until an honest adapter exists.
- `conventional-c`: narrow binary64 quadratic Mandelbrot oracle used by Experiment 0.
- `cdc-experimental`: participates by returning an explained unresolved result and requesting fallback. It does not claim a valid Mandelbrot method.

Every backend supplies a stable identifier and revision. Unknown identities fail construction and validation. Cancellation is input to point computation. Results distinguish escaped, proven bounded, unresolved, numerically uncertain, cancelled, and failed. Iteration-budget exhaustion is unresolved, never proven bounded. Conventional iterations and CDC descent steps are separate counters.

## Identity matrix

A manifest records computation, renderer, and memory independently. Supported descriptions include conventional-C or CDC-experimental computation with a renderer identity and either system or prospective Ouro memory. Selecting CDC controls CDC reference metadata; selecting the transitional CDC renderer does not. Selecting Ouro describes requested manifest identity only—it does not initialize or imply availability of the currently unimplemented Ouro runtime.

## Transitional renderer

`FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL` remains for compatibility. It is deprecated conceptually: all lifecycle calls remain unavailable and its header states that it owns no CDC mathematics. A later compatibility milestone may replace it with a native rasterizer identity after backend-neutral point/region consumption exists, migrate stored manifests, and retire the name through an explicit versioned decision.

## Finite precision and corpus gate

The conventional oracle defines operation order and binary64 observations, but does not prove boundedness. CDC progression requires a reviewed configuration, nonnegative potential, guarded update, certified relation, invariant, halt/terminal claim, and a sound finite-precision evidence rule. The PDF supplies no rounding-error or interval rule for this mapping.

Progress from one point to a corpus requires: an accepted Experiment 0 outcome; canonical problem vectors including escape, budget exhaustion, cancellation, and numerical uncertainty; deterministic trace/identity conventions; an independent conventional reference for every expected result; review of precision behavior; and predefined mismatch/fallback reporting. Corpus work must still not infer region or rendering semantics.
