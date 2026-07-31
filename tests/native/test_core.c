#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fractal/memory_backend.h"
#include "fractal_computation.h"
#include "fractal_cdc_certificate.h"
#include "fractal_cdc_region_study.h"
#include "fractal/render_artifact.h"
#include "fractal/render_failure.h"
#include "fractal/render_job.h"
#include "fractal/render_manifest.h"
#include "fractal_cdc_renderer.h"
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


static void test_computation_backends(void) {
 fractal_computation_backend legacy,conventional,cdc,invalid={0};
 fractal_computation_problem problem; fractal_point_input point={2.0,0.0},inside={0.0,0.0};
 fractal_computation_cancellation cancellation={false}; fractal_point_result result;
 CHECK(fractal_computation_backend_create(FRACTAL_COMPUTATION_BACKEND_LEGACY_REFERENCE,&legacy)==FRACTAL_OK);
 CHECK(fractal_computation_backend_validate(&legacy)==FRACTAL_OK);
 CHECK(legacy.metadata.kind==FRACTAL_COMPUTATION_BACKEND_LEGACY_REFERENCE);
 CHECK(fractal_computation_backend_create(FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,&conventional)==FRACTAL_OK);
 CHECK(strcmp(conventional.metadata.stable_identifier,"conventional-c")==0);
 CHECK(fractal_computation_backend_create(FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL,&cdc)==FRACTAL_OK);
 CHECK(fractal_computation_backend_create((fractal_computation_backend_kind)99,&invalid)==FRACTAL_ERROR_INVALID_ARGUMENT);
 CHECK(fractal_computation_backend_string((fractal_computation_backend_kind)99)==NULL);
 CHECK(fractal_computation_problem_init_experiment_0(&problem)==FRACTAL_OK);
 CHECK(conventional.vtable->compute_point(&problem,&point,&cancellation,&result)==FRACTAL_OK);
 CHECK(result.classification==FRACTAL_POINT_ESCAPED);
 CHECK(result.conventional_iteration_count==2u && result.cdc_descent_step_count==0u);
 CHECK(result.trace_count==3u && result.trace[0].z_real==0.0 && result.trace[1].z_real==2.0 && result.trace[2].z_real==6.0);
 CHECK(result.final_z_real==6.0 && result.final_z_imaginary==0.0);
 CHECK(conventional.vtable->compute_point(&problem,&inside,&cancellation,&result)==FRACTAL_OK);
 CHECK(result.classification==FRACTAL_POINT_UNRESOLVED);
 CHECK(result.classification!=FRACTAL_POINT_PROVEN_BOUNDED);
 CHECK(cdc.vtable->compute_point(&problem,&point,&cancellation,&result)==FRACTAL_OK);
 CHECK(result.classification==FRACTAL_POINT_UNRESOLVED && result.fallback_required);
 CHECK(result.cdc_descent_step_count==0u && strcmp(result.evidence_identity,"CDC-EXPERIMENT-0-NEGATIVE-RESULT")==0);
 cancellation.requested=true;
 CHECK(conventional.vtable->compute_point(&problem,&point,&cancellation,&result)==FRACTAL_OK);
 CHECK(result.classification==FRACTAL_POINT_CANCELLED);
}

static void test_experiment_0_certificate(void) {
 fractal_cdc_experiment_0_certificate c,bad; char a[768],b[768]; size_t na=0,nb=0; uint64_t raw;
 CHECK(fractal_cdc_experiment_0_certificate_create(&c)==FRACTAL_OK);
 CHECK(fractal_cdc_experiment_0_certificate_validate(&c)==FRACTAL_OK);
 memcpy(&raw,&c.trace_real[0],sizeof(raw)); CHECK(raw==UINT64_C(0));
 memcpy(&raw,&c.trace_real[1],sizeof(raw)); CHECK(raw==UINT64_C(0x4000000000000000));
 memcpy(&raw,&c.trace_real[2],sizeof(raw)); CHECK(raw==UINT64_C(0x4018000000000000));
 CHECK(c.trace_real[2]*c.trace_real[2] > c.escape_radius*c.escape_radius);
 CHECK(c.cdc_descent_step_count==0u && c.mapping_decision==FRACTAL_CDC_MAPPING_NEGATIVE_RESULT);
 CHECK(fractal_cdc_experiment_0_certificate_serialize(&c,a,sizeof(a),&na)==FRACTAL_OK);
 CHECK(fractal_cdc_experiment_0_certificate_serialize(&c,b,sizeof(b),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(a,b,na)==0 && strstr(a,"NEGATIVE_RESULT")!=NULL);
 bad=c; bad.present_fields=0u; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.c_real=3.0; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.formula=(fractal_formula_kind)99; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.escape_radius=3.0; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.trace_real[2]=7.0; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.cdc_descent_step_count=1u; CHECK(fractal_cdc_experiment_0_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_cdc_experiment_0_certificate_serialize(&bad,a,sizeof(a),&na)==FRACTAL_ERROR_INVALID_SPEC);
}

static void test_region_study(void) {
 fractal_cdc_region_certificate c,bad; fractal_cdc_region invalid={1.0,1.0,0.0,1.0};
 fractal_cdc_region unsupported={1.0,1.5,0.0,0.25}; char a[1024],b[1024]; size_t na=0,nb=0;
 CHECK(fractal_cdc_region_study_0_create(&c)==FRACTAL_OK);
 CHECK(fractal_cdc_region_validate(&c.region)==FRACTAL_OK);
 CHECK(fractal_cdc_region_validate(&invalid)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_region_conventional_oracle(&c.region)==FRACTAL_REGION_CERTIFIED_ESCAPED);
 CHECK(fractal_region_conventional_oracle(&unsupported)==FRACTAL_REGION_UNRESOLVED);
 CHECK(fractal_cdc_region_certificate_validate(&c)==FRACTAL_OK);
 CHECK(c.potential_before==1u && c.potential_after==0u && c.cdc_descent_step_count==1u);
 CHECK(fractal_cdc_region_certificate_serialize(&c,a,sizeof(a),&na)==FRACTAL_OK);
 CHECK(fractal_cdc_region_certificate_serialize(&c,b,sizeof(b),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(a,b,na)==0 && strstr(a,"NEGATIVE_RESULT")!=NULL);
 bad=c; bad.z2_real_lower=2.01; CHECK(fractal_cdc_region_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.guard_sound=false; CHECK(fractal_cdc_region_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.potential_after=1u; CHECK(fractal_cdc_region_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.cdc_descent_step_count=0u; CHECK(fractal_cdc_region_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
}

static void test_renderer_backends_and_manifest(void) {
 fractal_renderer legacy,cdc_renderer,invalid={0}; fractal_render_spec spec;
 fractal_render_manifest conventional,transitional_renderer,cdc,ouro; char ja[768],jb[768]; size_t na=0,nb=0;
 CHECK(fractal_renderer_create(FRACTAL_RENDERER_BACKEND_LEGACY_CPP,&legacy)==FRACTAL_OK);
 CHECK(legacy.backend_kind==FRACTAL_RENDERER_BACKEND_LEGACY_CPP);
 CHECK(fractal_renderer_create(FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL,&cdc_renderer)==FRACTAL_OK);
 CHECK(fractal_renderer_validate(&cdc_renderer)==FRACTAL_OK);
 CHECK(cdc_renderer.vtable->render_tile(cdc_renderer.state,0,0,0,1,1,NULL,0)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 CHECK(fractal_renderer_create((fractal_renderer_backend_kind)99,&invalid)==FRACTAL_ERROR_INVALID_ARGUMENT);
 CHECK(fractal_render_spec_init_default(&spec)==FRACTAL_OK);
 CHECK(fractal_render_manifest_init(&conventional,&spec,FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(conventional.cdc_reference_sha256[0]=='\0');
 CHECK(fractal_render_manifest_init(&transitional_renderer,&spec,FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(transitional_renderer.cdc_reference_sha256[0]=='\0');
 CHECK(conventional.metrics.computation_backend==FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C);
 CHECK(fractal_render_manifest_init(&cdc,&spec,FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(strcmp(cdc.cdc_reference_sha256,FRACTAL_CDC_PDF_SHA256)==0);
 CHECK(cdc.metrics.renderer_backend==conventional.metrics.renderer_backend && cdc.metrics.memory_backend==conventional.metrics.memory_backend && cdc.metrics.computation_backend!=conventional.metrics.computation_backend);
 CHECK(fractal_render_manifest_init(&ouro,&spec,FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_OURO)==FRACTAL_OK);
 CHECK(ouro.metrics.computation_backend==cdc.metrics.computation_backend && ouro.metrics.renderer_backend==cdc.metrics.renderer_backend && ouro.metrics.memory_backend!=cdc.metrics.memory_backend);
 CHECK(fractal_render_manifest_serialize_identity_json(&cdc,ja,sizeof(ja),&na)==FRACTAL_OK);
 CHECK(fractal_render_manifest_serialize_identity_json(&cdc,jb,sizeof(jb),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(ja,jb,na)==0 && strstr(ja,"\"computation_backend\":\"cdc-experimental\"")!=NULL);
}

static void test_milestone_files_preserved(void) {
 const char *paths[]={"CDC.pdf","server.py","server.cpp","render.cpp","render_engine.cpp","index.html",
  "app_server","render","render_engine","frames/frame_0000.bmp","render.mp4",
  "docs/cdc_reference_index.md","docs/cdc_interpretation_ledger.md"}; size_t i;
 for(i=0;i<sizeof(paths)/sizeof(paths[0]);++i) { FILE *f=fopen(paths[i],"rb"); long size=-1; CHECK(f!=NULL); if(f){ CHECK(fseek(f,0,SEEK_END)==0); size=ftell(f); fclose(f); CHECK(size>0); } }
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
 test_spec(); test_memory(); test_adapter(); test_computation_backends(); test_experiment_0_certificate(); test_region_study(); test_renderer_backends_and_manifest(); test_milestone_files_preserved(); test_foreign_binaries_are_data_only();
 if(failures) fprintf(stderr,"%d native checks failed\n",failures);
 return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
