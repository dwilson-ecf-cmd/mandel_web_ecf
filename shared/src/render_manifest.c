#include "fractal/render_manifest.h"
#include <stdio.h>
#include <string.h>
fractal_result fractal_render_manifest_set_computation(fractal_render_manifest *m,
 const char *id,uint32_t version,uint32_t assignments,const char *status,uint64_t identity){
 if(!m||!id||!*id||strlen(id)>=sizeof(m->computation_module)||!version||!status||!*status||
    strlen(status)>=sizeof(m->computation_execution_status))return FRACTAL_ERROR_INVALID_ARGUMENT;
 strcpy(m->computation_module,id);strcpy(m->computation_execution_status,status);
 m->computation_module_version=version;m->computation_assignment_count=assignments;
 m->computation_execution_identity=identity;return FRACTAL_OK;
}
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
 count=snprintf(output,capacity,"{\"cdc_reference_sha256\":\"%s\",\"computation_assignment_count\":%u,\"computation_backend\":\"%s\",\"computation_execution_identity\":\"%016llx\",\"computation_execution_status\":\"%s\",\"computation_module\":\"%s\",\"computation_module_version\":%u,\"memory_backend\":\"%s\",\"renderer_backend\":\"%s\"}",m->cdc_reference_sha256,m->computation_assignment_count,computation,(unsigned long long)m->computation_execution_identity,m->computation_execution_status,m->computation_module,m->computation_module_version,memory,renderer);
 if (count<0) return FRACTAL_ERROR_IO;
 *length=(size_t)count;
 return !output || capacity<=(size_t)count ? FRACTAL_ERROR_BUFFER_TOO_SMALL : FRACTAL_OK;
}
