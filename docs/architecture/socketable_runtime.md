# Socketable native runtime v1

The first installed assembly is `formula.mandelbrot.quadratic`,
`numeric.binary64`, `compute.conventional.scalar-c`, `refinement.none`,
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

Only binary64, scalar compute, no refinement, serial traversal, one fixed palette,
BGR8, BMP, system memory, no-op telemetry, and a bounded in-memory write sink are
implemented. Budget exhaustion remains unresolved and never claims boundedness.
Module loading is static. A natural next milestone is a host platform file sink and
streaming manifest/artifact API, followed by a deterministic thread-pool scheduler;
CDC and Ouro should remain inactive until their own contracts are implemented and
validated.
