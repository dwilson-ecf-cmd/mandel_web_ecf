#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H
#include <stddef.h>
#include <stdint.h>
#include "fractal/render_spec.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct fractal_renderer fractal_renderer;
typedef struct fractal_renderer_vtable {
 fractal_result (*initialize)(void *state);
 fractal_result (*validate_spec)(void *state, const fractal_render_spec *spec);
 fractal_result (*begin_job)(void *state, const char *job_id, const fractal_render_spec *spec);
 fractal_result (*begin_frame)(void *state, uint32_t frame_index);
 fractal_result (*render_tile)(void *state, uint32_t frame_index, uint32_t x, uint32_t y,
  uint32_t width, uint32_t height, unsigned char *pixels, size_t pixels_capacity);
 fractal_result (*finish_frame)(void *state, uint32_t frame_index);
 fractal_result (*finish_job)(void *state);
 fractal_result (*cancel_job)(void *state);
 void (*shutdown)(void *state);
} fractal_renderer_vtable;
struct fractal_renderer { void *state; const fractal_renderer_vtable *vtable; };
fractal_result fractal_renderer_validate(const fractal_renderer *renderer);
#ifdef __cplusplus
}
#endif
#endif
