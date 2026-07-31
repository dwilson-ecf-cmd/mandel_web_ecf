#ifndef FRACTAL_RESULT_H
#define FRACTAL_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum fractal_result {
    FRACTAL_OK = 0,
    FRACTAL_ERROR_INVALID_ARGUMENT,
    FRACTAL_ERROR_INVALID_SPEC,
    FRACTAL_ERROR_BUFFER_TOO_SMALL,
    FRACTAL_ERROR_NOT_IMPLEMENTED,
    FRACTAL_ERROR_OUT_OF_MEMORY,
    FRACTAL_ERROR_IO,
    FRACTAL_ERROR_RENDERER,
    FRACTAL_ERROR_CANCELLED
} fractal_result;

const char *fractal_result_string(fractal_result result);

#ifdef __cplusplus
}
#endif
#endif
