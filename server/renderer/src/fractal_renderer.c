#include "fractal_renderer.h"
#include "fractal_cdc_renderer.h"
#include "fractal_cpp_adapter.h"
const char *fractal_renderer_backend_string(fractal_renderer_backend_kind kind) {
 switch(kind) {
 case FRACTAL_RENDERER_BACKEND_LEGACY_CPP: return "legacy-cpp";
 case FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL: return "cdc-experimental";
 default: return NULL;
 }
}
fractal_result fractal_renderer_validate(const fractal_renderer *r) {
 if (!r || !fractal_renderer_backend_string(r->backend_kind) || !r->vtable || !r->vtable->initialize || !r->vtable->validate_spec ||
  !r->vtable->begin_job || !r->vtable->begin_frame || !r->vtable->render_tile ||
  !r->vtable->finish_frame || !r->vtable->finish_job || !r->vtable->cancel_job || !r->vtable->shutdown)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 return FRACTAL_OK;
}
fractal_result fractal_renderer_create(fractal_renderer_backend_kind kind, fractal_renderer *renderer) {
 if (!renderer) return FRACTAL_ERROR_INVALID_ARGUMENT;
 switch(kind) {
 case FRACTAL_RENDERER_BACKEND_LEGACY_CPP: return fractal_cpp_adapter_create(renderer);
 case FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL: return fractal_cdc_renderer_create(renderer);
 default: return FRACTAL_ERROR_INVALID_ARGUMENT;
 }
}
