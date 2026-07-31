#ifndef FRACTAL_CDC_RENDERER_H
#define FRACTAL_CDC_RENDERER_H
#include "fractal_renderer.h"
/* Transitional compatibility identity only. CDC mathematics belongs to the
 * computation substrate; this renderer performs no computation or rasterization
 * and every lifecycle operation remains unavailable. */
fractal_result fractal_cdc_renderer_create(fractal_renderer *renderer);
#endif
