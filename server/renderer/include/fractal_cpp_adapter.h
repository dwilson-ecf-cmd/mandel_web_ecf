#ifndef FRACTAL_CPP_ADAPTER_H
#define FRACTAL_CPP_ADAPTER_H
#include "fractal_renderer.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Создаёт невладеющий адаптер-заглушку. Визуализация остаётся недоступной, пока
 * унаследованный полноэкранный визуализатор не получит проверенное отображение плиток и пикселей. */
fractal_result fractal_cpp_adapter_create(fractal_renderer *renderer);
#ifdef __cplusplus
}
#endif
#endif
