#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fractal/memory_backend.h"
#include "fractal/render_artifact.h"
#include "fractal/render_failure.h"
#include "fractal/render_job.h"
#include "fractal/worker_status.h"
#include "fractal_cpp_adapter.h"

static int failures;
#define CHECK(expression) do { if (!(expression)) { fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #expression); failures++; } } while (0)

static void test_spec(void) {
 fractal_render_spec spec, copy; char first[2048], second[2048], small[8]; size_t n1=0,n2=0,needed=0;
 CHECK(fractal_render_spec_init_default(&spec)==FRACTAL_OK);
 CHECK(fractal_render_spec_validate(&spec)==FRACTAL_OK);
 copy=spec; CHECK(fractal_render_spec_equal(&spec,&copy));
 CHECK(fractal_render_spec_serialize_canonical_json(&spec,first,sizeof(first),&n1)==FRACTAL_OK);
 CHECK(fractal_render_spec_serialize_canonical_json(&copy,second,sizeof(second),&n2)==FRACTAL_OK);
 CHECK(n1==n2 && memcmp(first,second,n1)==0);
 CHECK(fractal_render_spec_serialize_canonical_json(&spec,NULL,0u,&needed)==FRACTAL_ERROR_BUFFER_TOO_SMALL);
 CHECK(needed==n1);
 CHECK(fractal_render_spec_serialize_canonical_json(&spec,small,sizeof(small),&needed)==FRACTAL_ERROR_BUFFER_TOO_SMALL);
 CHECK(needed==n1);
 copy=spec; copy.image_width=0; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; copy.image_height=0; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; copy.tile_width=copy.image_width+1u; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; copy.tile_height=0; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; copy.maximum_iterations=0; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; copy.numerical_precision=(fractal_numerical_precision)99; CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; strcpy(copy.bailout_radius,"NaN"); CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
 copy=spec; strcpy(copy.scale,"4.0"); CHECK(fractal_render_spec_validate(&copy)==FRACTAL_ERROR_INVALID_SPEC);
}

static void test_memory(void) {
 fractal_memory_backend backend, ouro; fractal_memory_scope scope; fractal_memory_statistics stats;
 void *a,*b;
 CHECK(fractal_memory_backend_init_default(&backend)==FRACTAL_OK);
 CHECK(backend.kind==FRACTAL_MEMORY_BACKEND_SYSTEM);
 CHECK(fractal_memory_scope_begin(&backend,FRACTAL_MEMORY_SCOPE_SHORT,&scope)==FRACTAL_OK);
 a=fractal_memory_scope_alloc(&scope,13u,64u); b=fractal_memory_scope_alloc(&scope,7u,8u);
 CHECK(a!=NULL && ((uintptr_t)a % 64u)==0u); CHECK(b!=NULL && ((uintptr_t)b % 8u)==0u);
 CHECK(fractal_memory_scope_alloc(&scope,4u,3u)==NULL);
 stats=fractal_memory_backend_get_statistics(&backend);
 CHECK(stats.requested_bytes==24u && stats.successful_bytes==20u && stats.failed_bytes==4u);
 CHECK(stats.active_bytes==20u && stats.active_allocations==2u && stats.failed_allocations==1u);
 CHECK(fractal_memory_scope_reset(&scope)==FRACTAL_OK);
 stats=fractal_memory_backend_get_statistics(&backend);
 CHECK(stats.active_bytes==0u && stats.active_allocations==0u && stats.released_bytes==20u);
 /* Reset invalidates all returned pointers; tests intentionally never dereference them. */
 a=fractal_memory_scope_alloc(&scope,5u,16u);
 CHECK(a!=NULL && ((uintptr_t)a % 16u)==0u);
 fractal_memory_scope_end(&scope); fractal_memory_backend_shutdown(&backend);
 CHECK(fractal_memory_backend_init_ouro(&ouro)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 CHECK(!ouro.initialized && ouro.kind==FRACTAL_MEMORY_BACKEND_OURO);
}

static void test_adapter(void) {
 fractal_renderer renderer; fractal_render_spec spec;
 CHECK(fractal_cpp_adapter_create(&renderer)==FRACTAL_OK);
 CHECK(fractal_renderer_validate(&renderer)==FRACTAL_OK);
 CHECK(fractal_render_spec_init_default(&spec)==FRACTAL_OK);
 CHECK(renderer.vtable->validate_spec(renderer.state,&spec)==FRACTAL_OK);
 CHECK(renderer.vtable->initialize(renderer.state)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 renderer.vtable->shutdown(renderer.state);
}

static void test_foreign_binaries_are_data_only(void) {
 const char *names[]={"app_server","render","render_engine"}; size_t i;
 for(i=0;i<sizeof(names)/sizeof(names[0]);++i) {
  unsigned char magic[4]={0}; FILE *file=fopen(names[i],"rb");
  CHECK(file!=NULL); if(file) { CHECK(fread(magic,1,sizeof(magic),file)==sizeof(magic)); fclose(file); }
  CHECK(memcmp(magic,"\177ELF",4)==0);
 }
}
int main(void) {
 test_spec(); test_memory(); test_adapter(); test_foreign_binaries_are_data_only();
 if(failures) fprintf(stderr,"%d native checks failed\n",failures);
 return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
