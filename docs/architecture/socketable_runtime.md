# Socketable native runtime v1

The serial reference assembly is `formula.mandelbrot.quadratic`,
`fractal.numeric.binary64.v1`, `fractal.compute.scalar.v1`, `refinement.none`,
`scheduler.serial.row-major`, `raster.native.iteration-bgr8`, `encoder.bmp.v3`,
`memory.system.scoped`, `telemetry.noop`, and `platform.host.c11`. Julia substitutes
only `formula.julia.quadratic`.

The 32×24, 64-step fixtures use center (-0.5, 0), vertical scale 3, pixel-center
mapping, positive imaginary upward, palette `palette.socket-v1`, and BMP v3:

| formula | field FNV-1a-64 | pixels FNV-1a-64 | artifact FNV-1a-64 | bytes |
|---|---|---|---|---:|
| Mandelbrot | `99ec88c2a0f8bac3` | `4866aacc38290b5f` | `fb1a83bd5ca28e5f` | 2358 |
| Julia (-0.8, 0.156) | `0fb4458e08bad6e1` | `b272f08b0bbdca2b` | `4d4aa95bd137ec87` | 2358 |

## Limitations and next milestone

Only binary64, scalar compute, no refinement, serial or bounded contiguous-row
thread-pool traversal, one fixed palette, BGR8, BMP, system memory, no-op telemetry,
and bounded memory/host-file sinks are implemented. Budget exhaustion remains
unresolved and never claims boundedness. Module loading is static. CDC and Ouro
remain inactive. Numeric ownership and replay identity are specified in
`binary64_numeric_socket.md`.
