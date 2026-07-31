#ifndef FRACTAL_CPP_ADAPTER_H
#define FRACTAL_CPP_ADAPTER_H
#include "fractal_renderer.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Creates a non-owning stub adapter. Rendering remains unavailable until the
 * inherited full-frame renderer has a verified tile/pixel-layout mapping. */
fractal_result fractal_cpp_adapter_create(fractal_renderer *renderer);
#ifdef __cplusplus
}
#endif
#endif
