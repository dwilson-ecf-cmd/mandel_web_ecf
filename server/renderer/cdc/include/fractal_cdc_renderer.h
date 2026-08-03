#ifndef FRACTAL_CDC_RENDERER_H
#define FRACTAL_CDC_RENDERER_H
#include "fractal_renderer.h"
/* Только переходная идентичность совместимости. Математика CDC принадлежит
 * вычислительной основе; этот визуализатор не вычисляет и не растеризует,
 * а все операции жизненного цикла остаются недоступными. */
fractal_result fractal_cdc_renderer_create(fractal_renderer *renderer);
#endif
