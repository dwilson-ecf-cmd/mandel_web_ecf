# Native job specification

The socketable path separates immutable mathematical intent (`problem`), coordinate
mapping (`view`), field-to-pixel policy (`raster`), and artifact intent (`artifact`).
Module selection belongs to the independently validated `fractal_runtime_modules`.
Formula parameters carry a stable formula type ID, exact size, and bounded typed
record. This permits deterministic validation and serialization without interpreting
an untyped blob.

`fractal_render_spec` remains the compatibility model for legacy jobs. It is not an
alias for the native job because animation, tiling, and presentation-era fields have
no role in the first native vertical slice.

The view maps pixel centers in row-major order. Positive imaginary coordinates are
at the top. `scale` is vertical extent and horizontal extent is scale multiplied by
the image aspect ratio.
