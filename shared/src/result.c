#include "fractal/result.h"

const char *fractal_result_string(fractal_result result) {
    switch (result) {
    case FRACTAL_OK: return "ok";
    case FRACTAL_ERROR_INVALID_ARGUMENT: return "invalid argument";
    case FRACTAL_ERROR_INVALID_SPEC: return "invalid render specification";
    case FRACTAL_ERROR_BUFFER_TOO_SMALL: return "buffer too small";
    case FRACTAL_ERROR_NOT_IMPLEMENTED: return "not implemented";
    case FRACTAL_ERROR_OUT_OF_MEMORY: return "out of memory";
    case FRACTAL_ERROR_IO: return "I/O error";
    case FRACTAL_ERROR_RENDERER: return "renderer error";
    case FRACTAL_ERROR_CANCELLED: return "cancelled";
    default: return "unknown result";
    }
}
