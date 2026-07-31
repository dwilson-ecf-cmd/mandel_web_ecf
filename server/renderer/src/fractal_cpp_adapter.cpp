#include "fractal_cpp_adapter.h"
namespace {
fractal_result unavailable0(void *) { return FRACTAL_ERROR_NOT_IMPLEMENTED; }
fractal_result validate(void *, const fractal_render_spec *spec) { return fractal_render_spec_validate(spec); }
fractal_result begin_job(void *, const char *, const fractal_render_spec *) { return FRACTAL_ERROR_NOT_IMPLEMENTED; }
fractal_result frame(void *, uint32_t) { return FRACTAL_ERROR_NOT_IMPLEMENTED; }
fractal_result tile(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, unsigned char *, size_t) { return FRACTAL_ERROR_NOT_IMPLEMENTED; }
void shutdown(void *) {}
const fractal_renderer_vtable table = {unavailable0, validate, begin_job, frame, tile, frame, unavailable0, unavailable0, shutdown};
}
extern "C" fractal_result fractal_cpp_adapter_create(fractal_renderer *renderer) {
 if (renderer == nullptr) return FRACTAL_ERROR_INVALID_ARGUMENT;
 renderer->state=nullptr; renderer->vtable=&table; renderer->backend_kind=FRACTAL_RENDERER_BACKEND_LEGACY_CPP; return FRACTAL_OK;
}
