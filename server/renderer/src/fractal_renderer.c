#include "fractal_renderer.h"
fractal_result fractal_renderer_validate(const fractal_renderer *r) {
 if (!r || !r->vtable || !r->vtable->initialize || !r->vtable->validate_spec ||
  !r->vtable->begin_job || !r->vtable->begin_frame || !r->vtable->render_tile ||
  !r->vtable->finish_frame || !r->vtable->finish_job || !r->vtable->cancel_job || !r->vtable->shutdown)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 return FRACTAL_OK;
}
