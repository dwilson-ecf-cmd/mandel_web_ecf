#include "fractal_cdc_renderer.h"
static fractal_result unavailable0(void *state) { (void)state; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static fractal_result unavailable_spec(void *state, const fractal_render_spec *spec) { (void)state; (void)spec; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static fractal_result unavailable_job(void *state, const char *job_id, const fractal_render_spec *spec) { (void)state; (void)job_id; (void)spec; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static fractal_result unavailable_frame(void *state, uint32_t frame) { (void)state; (void)frame; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static fractal_result unavailable_tile(void *state, uint32_t frame, uint32_t x, uint32_t y, uint32_t width, uint32_t height, unsigned char *pixels, size_t capacity) { (void)state; (void)frame; (void)x; (void)y; (void)width; (void)height; (void)pixels; (void)capacity; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static void shutdown(void *state) { (void)state; }
static const fractal_renderer_vtable table={unavailable0,unavailable_spec,unavailable_job,unavailable_frame,unavailable_tile,unavailable_frame,unavailable0,unavailable0,shutdown};
fractal_result fractal_cdc_renderer_create(fractal_renderer *renderer) {
 if (!renderer) return FRACTAL_ERROR_INVALID_ARGUMENT;
 renderer->state=NULL; renderer->vtable=&table; renderer->backend_kind=FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL;
 return FRACTAL_OK;
}
