#ifndef FRACTAL_RENDER_SPEC_H
#define FRACTAL_RENDER_SPEC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_IDENTIFIER_CAPACITY 64u
#define FRACTAL_DECIMAL_CAPACITY 128u

typedef enum fractal_numerical_precision {
    FRACTAL_PRECISION_FLOAT32 = 0,
    FRACTAL_PRECISION_FLOAT64,
    FRACTAL_PRECISION_LONG_DOUBLE,
    FRACTAL_PRECISION_ARBITRARY_DECIMAL
} fractal_numerical_precision;

typedef struct fractal_animation_spec {
    bool enabled;
    uint32_t frame_count;
    char frames_per_second[FRACTAL_DECIMAL_CAPACITY];
    char zoom_multiplier_per_frame[FRACTAL_DECIMAL_CAPACITY];
} fractal_animation_spec;

typedef struct fractal_render_spec {
    char fractal_type[FRACTAL_IDENTIFIER_CAPACITY];
    char center_real[FRACTAL_DECIMAL_CAPACITY];
    char center_imaginary[FRACTAL_DECIMAL_CAPACITY];
    char scale[FRACTAL_DECIMAL_CAPACITY];
    uint32_t image_width;
    uint32_t image_height;
    uint32_t maximum_iterations;
    char bailout_radius[FRACTAL_DECIMAL_CAPACITY];
    fractal_numerical_precision numerical_precision;
    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t supersampling;
    char palette_identifier[FRACTAL_IDENTIFIER_CAPACITY];
    fractal_animation_spec animation;
} fractal_render_spec;

fractal_result fractal_render_spec_init_default(fractal_render_spec *spec);
fractal_result fractal_render_spec_validate(const fractal_render_spec *spec);
fractal_result fractal_render_spec_serialize_canonical_json(
    const fractal_render_spec *spec, char *output, size_t output_capacity,
    size_t *output_length);
bool fractal_render_spec_equal(const fractal_render_spec *left,
                               const fractal_render_spec *right);
const char *fractal_numerical_precision_string(fractal_numerical_precision precision);

#ifdef __cplusplus
}
#endif
#endif
