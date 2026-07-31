#include "fractal/render_manifest.h"
#include <stdio.h>
#include <string.h>
fractal_result fractal_render_manifest_init(fractal_render_manifest *m, const fractal_render_spec *spec,
 fractal_computation_backend_kind computation, fractal_renderer_backend_kind renderer,
 fractal_memory_backend_kind memory) {
 if (!m || fractal_render_spec_validate(spec)!=FRACTAL_OK || !fractal_computation_backend_string(computation) ||
     !fractal_renderer_backend_string(renderer) ||
     (memory!=FRACTAL_MEMORY_BACKEND_SYSTEM && memory!=FRACTAL_MEMORY_BACKEND_OURO)) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m)); m->spec=*spec; m->metrics.computation_backend=computation;
 m->metrics.renderer_backend=renderer; m->metrics.memory_backend=memory;
 if (computation==FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL) strcpy(m->cdc_reference_sha256,FRACTAL_CDC_PDF_SHA256);
 return FRACTAL_OK;
}
fractal_result fractal_render_manifest_serialize_identity_json(const fractal_render_manifest *m,
 char *output, size_t capacity, size_t *length) {
 const char *computation,*renderer,*memory; int count;
 if (!m || !length || (output==NULL && capacity!=0u)) return FRACTAL_ERROR_INVALID_ARGUMENT;
 computation=fractal_computation_backend_string(m->metrics.computation_backend);
 renderer=fractal_renderer_backend_string(m->metrics.renderer_backend);
 memory=m->metrics.memory_backend==FRACTAL_MEMORY_BACKEND_SYSTEM?"system":m->metrics.memory_backend==FRACTAL_MEMORY_BACKEND_OURO?"ouro":NULL;
 if (!computation || !renderer || !memory) return FRACTAL_ERROR_INVALID_ARGUMENT;
 count=snprintf(output,capacity,"{\"cdc_reference_sha256\":\"%s\",\"computation_backend\":\"%s\",\"memory_backend\":\"%s\",\"renderer_backend\":\"%s\"}",m->cdc_reference_sha256,computation,memory,renderer);
 if (count<0) return FRACTAL_ERROR_IO;
 *length=(size_t)count;
 return !output || capacity<=(size_t)count ? FRACTAL_ERROR_BUFFER_TOO_SMALL : FRACTAL_OK;
}
