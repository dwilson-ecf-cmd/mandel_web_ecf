#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fractal/memory_backend.h"
#include "fractal_computation.h"
#include "fractal_cdc_certificate.h"
#include "fractal_cdc_region_study.h"
#include "fractal_cdc_two_child.h"
#include "fractal_cdc_depth_two.h"
#include "fractal_cdc_refinement_contract.h"
#include "fractal/render_artifact.h"
#include "fractal/render_failure.h"
#include "fractal/render_job.h"
#include "fractal/render_manifest.h"
#include "fractal/numeric.h"
#include "fractal/formula.h"
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
 /* Сброс делает все возвращённые указатели недействительными; тесты намеренно их не разыменовывают. */
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

static void test_two_child_study(void) {
 fractal_cdc_two_child_split split,bad_split;
 fractal_cdc_two_child_certificate c,bad; char a[2048],b[2048]; size_t na=0,nb=0;
 uint32_t obligations=0u; uint64_t potential=0u;
 CHECK(fractal_cdc_two_child_split_create(&split)==FRACTAL_OK);
 CHECK(fractal_cdc_two_child_split_validate(&split)==FRACTAL_OK);
 CHECK(split.parent.real_min==2.0 && split.parent.real_max==2.25 && split.split_value==2.125);
 CHECK(split.left.real_max==split.right.real_min);
 CHECK(split.left.real_max-split.left.real_min < split.parent.real_max-split.parent.real_min);
 CHECK(split.right.real_max-split.right.real_min < split.parent.real_max-split.parent.real_min);
 CHECK(fractal_region_conventional_oracle(&split.left)==FRACTAL_REGION_CERTIFIED_ESCAPED);
 CHECK(fractal_region_conventional_oracle(&split.right)==FRACTAL_REGION_CERTIFIED_ESCAPED);
 CHECK(fractal_cdc_two_child_multiset_descends(2u,1u,1u));
 CHECK(!fractal_cdc_two_child_multiset_descends(2u,2u,1u));
 CHECK(!fractal_cdc_two_child_multiset_descends(2u,1u,2u));
 CHECK(fractal_cdc_two_child_discharge(3u,1u,&obligations,&potential)==FRACTAL_OK);
 CHECK(obligations==2u && potential==1u); /* одно дочернее обязательство снято */
 CHECK(fractal_cdc_two_child_discharge(3u,0u,&obligations,&potential)==FRACTAL_OK);
 CHECK(obligations==3u && potential==2u); /* неразрешённые условия не дают прогресса */
 CHECK(fractal_cdc_two_child_discharge(3u,3u,&obligations,&potential)==FRACTAL_OK);
 CHECK(obligations==0u && potential==0u); /* снятие коммутирующего набора */
 CHECK(fractal_cdc_two_child_discharge(4u,0u,&obligations,&potential)==FRACTAL_ERROR_INVALID_SPEC);
 bad_split=split; bad_split.split_value=2.0;
 CHECK(fractal_cdc_two_child_split_validate(&bad_split)==FRACTAL_ERROR_INVALID_SPEC);
 bad_split=split; bad_split.left.real_max=bad_split.left.real_min;
 CHECK(fractal_cdc_two_child_split_validate(&bad_split)==FRACTAL_ERROR_INVALID_SPEC);
 bad_split=split; bad_split.right.real_min=2.0;
 CHECK(fractal_cdc_two_child_split_validate(&bad_split)==FRACTAL_ERROR_INVALID_SPEC);
 bad_split=split; bad_split.right_area_units=2u;
 CHECK(fractal_cdc_two_child_split_validate(&bad_split)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_cdc_two_child_certificate_create(&c)==FRACTAL_OK);
 CHECK(fractal_cdc_two_child_certificate_validate(&c)==FRACTAL_OK);
 CHECK(c.sequential_potential[0]==3u && c.sequential_potential[1]==2u &&
  c.sequential_potential[2]==1u && c.sequential_potential[3]==0u);
 CHECK(c.batch_potential[0]==3u && c.batch_potential[1]==2u && c.batch_potential[2]==0u);
 CHECK(fractal_cdc_two_child_certificate_serialize(&c,a,sizeof(a),&na)==FRACTAL_OK);
 CHECK(fractal_cdc_two_child_certificate_serialize(&c,b,sizeof(b),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(a,b,na)==0 && strstr(a,"SUPPORTED_ONE_LEVEL_BRANCHING_DESCENT")!=NULL);
 CHECK(strstr(a,"NEGATIVE_RESULT")!=NULL && strstr(a,"COMPOSITE_OBJECT_REQUIRED")!=NULL);
 bad=c; bad.child_escape_guard[1]=false;
 CHECK(fractal_cdc_two_child_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.child_discharged[1]=false;
 CHECK(fractal_cdc_two_child_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.sequential_potential[1]=3u;
 CHECK(fractal_cdc_two_child_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.cdc_descent_step_count=0u;
 CHECK(fractal_cdc_two_child_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
}

static void test_depth_two_study(void) {
 fractal_cdc_depth_two_tree tree,bad_tree;
 fractal_cdc_depth_two_certificate c,bad;
 uint8_t s0[1]={2u},s1[2]={1u,1u},s2[3]={0u,0u,1u},duplicate[2]={0u,0u},huge[1]={41u};
 uint64_t potential=0u; char a[2048],b[2048]; size_t na=0u,nb=0u;
 CHECK(fractal_cdc_depth_two_tree_create(&tree)==FRACTAL_OK);
 CHECK(fractal_cdc_depth_two_tree_validate(&tree)==FRACTAL_OK);
 CHECK(tree.split_1==2.125 && tree.split_2==2.0625);
 CHECK(tree.region[FRACTAL_CDC_NODE_C].real_max==tree.region[FRACTAL_CDC_NODE_D].real_min);
 CHECK(tree.region[FRACTAL_CDC_NODE_C].real_min==tree.region[FRACTAL_CDC_NODE_A].real_min);
 CHECK(tree.region[FRACTAL_CDC_NODE_D].real_max==tree.region[FRACTAL_CDC_NODE_A].real_max);
 CHECK(tree.rank[0]==2u && tree.rank[1]==1u && tree.rank[2]==1u && tree.rank[3]==0u && tree.rank[4]==0u);
 CHECK(fractal_cdc_depth_two_potential(s0,1u,&potential)==FRACTAL_OK && potential==9u);
 CHECK(fractal_cdc_depth_two_potential(s1,2u,&potential)==FRACTAL_OK && potential==6u);
 CHECK(fractal_cdc_depth_two_potential(s2,3u,&potential)==FRACTAL_OK && potential==5u);
 CHECK(fractal_cdc_depth_two_potential(duplicate,2u,&potential)==FRACTAL_OK && potential==2u);
 CHECK(fractal_cdc_depth_two_potential(NULL,0u,&potential)==FRACTAL_OK && potential==0u);
 CHECK(fractal_cdc_depth_two_potential(huge,1u,&potential)==FRACTAL_ERROR_INVALID_SPEC);
 bad_tree=tree; bad_tree.split_2=2.0;
 CHECK(fractal_cdc_depth_two_tree_validate(&bad_tree)==FRACTAL_ERROR_INVALID_SPEC);
 bad_tree=tree; bad_tree.rank[FRACTAL_CDC_NODE_C]=1u;
 CHECK(fractal_cdc_depth_two_tree_validate(&bad_tree)==FRACTAL_ERROR_INVALID_SPEC);
 bad_tree=tree; bad_tree.rank[FRACTAL_CDC_NODE_A]=0u; /* ретроспективная перезапись запрещена */
 CHECK(fractal_cdc_depth_two_tree_validate(&bad_tree)==FRACTAL_ERROR_INVALID_SPEC);
 bad_tree=tree; bad_tree.region[FRACTAL_CDC_NODE_B].real_max=2.0; /* соседний узел должен оставаться неизменным */
 CHECK(fractal_cdc_depth_two_tree_validate(&bad_tree)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_cdc_depth_two_certificate_create(&c)==FRACTAL_OK);
 CHECK(fractal_cdc_depth_two_certificate_validate(&c)==FRACTAL_OK);
 CHECK(c.split_potential[0]>c.split_potential[1] && c.split_potential[1]>c.split_potential[2]);
 CHECK(c.sequential_potential[2]==5u && c.sequential_potential[3]==4u &&
  c.sequential_potential[4]==3u && c.sequential_potential[5]==0u);
 CHECK(c.batch_potential[2]==5u && c.batch_potential[3]==0u);
 CHECK(c.ranks_immutable && c.future_independent);
 CHECK(fractal_cdc_depth_two_certificate_serialize(&c,a,sizeof(a),&na)==FRACTAL_OK);
 CHECK(fractal_cdc_depth_two_certificate_serialize(&c,b,sizeof(b),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(a,b,na)==0 && strstr(a,"SUPPORTED_DEPTH_TWO_COMPOSITION")!=NULL);
 CHECK(strstr(a,"SUPPORTED_ONE_LEVEL_BRANCHING_DESCENT")!=NULL && strstr(a,"NEGATIVE_RESULT")!=NULL);
 bad=c; bad.future_independent=false;
 CHECK(fractal_cdc_depth_two_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.conventional_guards[0]=false; /* остановка означает unresolved, а не ложный переход */
 CHECK(fractal_cdc_depth_two_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
 bad=c; bad.tree.region[FRACTAL_CDC_NODE_D].real_min=2.0;
 CHECK(fractal_cdc_depth_two_certificate_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC);
}

static void test_refinement_contracts(void) {
 fractal_cdc_depth_two_tree tree; fractal_cdc_refinement_contract depth,width,tokens,none,new_contract,bad;
 fractal_cdc_refinement_obligation root,leaf,mismatch; char a[768],b[768]; size_t na=0u,nb=0u;
 uint8_t rank=99u; bool allowed=true; uint64_t old_identity;
 CHECK(fractal_cdc_depth_two_tree_create(&tree)==FRACTAL_OK);
 CHECK(fractal_cdc_contract_create_max_depth(&depth,&tree.region[0],2u,FRACTAL_CDC_EXHAUST_CONTRACT_EXHAUSTED)==FRACTAL_OK);
 CHECK(fractal_cdc_contract_validate(&depth)==FRACTAL_OK);
 CHECK(fractal_cdc_contract_serialize(&depth,a,sizeof(a),&na)==FRACTAL_OK);
 CHECK(fractal_cdc_contract_serialize(&depth,b,sizeof(b),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(a,b,na)==0); /* детерминизм повторного воспроизведения */
 old_identity=depth.identity; bad=depth; bad.maximum_refinement_depth=1u;
 CHECK(fractal_cdc_contract_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC); /* неизменяемое каноническое поле */
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[0],0u,&rank)==FRACTAL_OK && rank==2u);
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[1],1u,&rank)==FRACTAL_OK && rank==1u);
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[2],1u,&rank)==FRACTAL_OK && rank==1u);
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[3],2u,&rank)==FRACTAL_OK && rank==0u);
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[4],2u,&rank)==FRACTAL_OK && rank==0u);
 CHECK(fractal_cdc_contract_derive_rank(&depth,&tree.region[0],3u,&rank)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_cdc_contract_create_min_real_width(&width,&tree.region[0],0.0625,FRACTAL_CDC_EXHAUST_FALLBACK_REQUIRED)==FRACTAL_OK);
 CHECK(width.identity!=depth.identity); /* одна цель в независимых контрактах */
 CHECK(fractal_cdc_contract_derive_rank(&width,&tree.region[0],0u,&rank)==FRACTAL_OK && rank==2u);
 CHECK(fractal_cdc_contract_derive_rank(&width,&tree.region[1],1u,&rank)==FRACTAL_OK && rank==1u);
 CHECK(fractal_cdc_contract_derive_rank(&width,&tree.region[3],2u,&rank)==FRACTAL_OK && rank==0u);
 bad=width; bad.minimum_real_width=0.07; bad.identity=old_identity;
 CHECK(fractal_cdc_contract_validate(&bad)==FRACTAL_ERROR_INVALID_SPEC); /* неточный или незарегистрированный масштаб */
 CHECK(fractal_cdc_contract_create_fixed_tokens(&tokens,&tree.region[0],2u,FRACTAL_CDC_EXHAUST_UNRESOLVED)==FRACTAL_OK);
 CHECK(tokens.identity!=depth.identity && tokens.identity!=width.identity);
 memset(&none,0,sizeof(none)); CHECK(fractal_cdc_contract_validate(&none)==FRACTAL_ERROR_INVALID_SPEC); /* контракт отсутствует */
 CHECK(fractal_cdc_obligation_create(&depth,&tree.region[0],0u,&root)==FRACTAL_OK);
 CHECK(root.creation_rank==2u && root.contract_identity==depth.identity);
 CHECK(fractal_cdc_obligation_can_split(&depth,&root,&allowed)==FRACTAL_OK && allowed);
 CHECK(fractal_cdc_obligation_create(&depth,&tree.region[3],2u,&leaf)==FRACTAL_OK);
 CHECK(fractal_cdc_obligation_can_split(&depth,&leaf,&allowed)==FRACTAL_OK && !allowed); /* нулевой ранг запрещает разделение */
 CHECK(fractal_cdc_obligation_finish(&depth,&leaf,true)==FRACTAL_OK && leaf.status==FRACTAL_CDC_OBLIGATION_DISCHARGED); /* нулевой ранг допускает снятие */
 CHECK(fractal_cdc_obligation_create(&depth,&tree.region[4],2u,&leaf)==FRACTAL_OK);
 CHECK(fractal_cdc_obligation_finish(&depth,&leaf,false)==FRACTAL_OK && leaf.status==FRACTAL_CDC_OBLIGATION_CONTRACT_EXHAUSTED);
 CHECK(fractal_cdc_obligation_create(&width,&tree.region[4],2u,&leaf)==FRACTAL_OK);
 CHECK(fractal_cdc_obligation_finish(&width,&leaf,false)==FRACTAL_OK && leaf.status==FRACTAL_CDC_OBLIGATION_FALLBACK_REQUIRED);
 CHECK(fractal_cdc_obligation_create(&tokens,&tree.region[4],2u,&leaf)==FRACTAL_OK);
 CHECK(fractal_cdc_obligation_finish(&tokens,&leaf,false)==FRACTAL_OK && leaf.status==FRACTAL_CDC_OBLIGATION_UNRESOLVED);
 mismatch=root; CHECK(fractal_cdc_obligation_can_split(&width,&mismatch,&allowed)==FRACTAL_ERROR_INVALID_SPEC); /* несоответствие контракта */
 mismatch=root; mismatch.creation_rank=1u; CHECK(fractal_cdc_obligation_can_split(&depth,&mismatch,&allowed)==FRACTAL_ERROR_INVALID_SPEC); /* перезапись ранга запрещена */
 CHECK(fractal_cdc_contract_extend(&depth,1u,&new_contract)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_cdc_contract_create_max_depth(&bad,&tree.region[0],1u,FRACTAL_CDC_EXHAUST_CONTRACT_EXHAUSTED)==FRACTAL_OK);
 CHECK(fractal_cdc_contract_extend(&bad,2u,&new_contract)==FRACTAL_OK && new_contract.identity==depth.identity && bad.identity!=new_contract.identity);
 CHECK(fractal_cdc_contract_extend(&depth,4u,&new_contract)==FRACTAL_ERROR_INVALID_SPEC); /* незарегистрированное расширение */
 CHECK(depth.identity==old_identity); /* расширение никогда не изменяет прежний контракт */
}

static void test_renderer_backends_and_manifest(void) {
 fractal_renderer legacy,cdc_renderer,invalid={0}; fractal_render_spec spec;
 fractal_render_manifest conventional,transitional_renderer,cdc,ouro;
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &mandelbrot,sizeof(mandelbrot)};
 uint64_t parameter_identity=0;
 char ja[1536],jb[1536]; size_t na=0,nb=0;
 CHECK(fractal_renderer_create(FRACTAL_RENDERER_BACKEND_LEGACY_CPP,&legacy)==FRACTAL_OK);
 CHECK(legacy.backend_kind==FRACTAL_RENDERER_BACKEND_LEGACY_CPP);
 CHECK(fractal_renderer_create(FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL,&cdc_renderer)==FRACTAL_OK);
 CHECK(fractal_renderer_validate(&cdc_renderer)==FRACTAL_OK);
 CHECK(cdc_renderer.vtable->render_tile(cdc_renderer.state,0,0,0,1,1,NULL,0)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 CHECK(fractal_renderer_create((fractal_renderer_backend_kind)99,&invalid)==FRACTAL_ERROR_INVALID_ARGUMENT);
 CHECK(fractal_render_spec_init_default(&spec)==FRACTAL_OK);
 CHECK(fractal_render_manifest_init(&conventional,&spec,FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(fractal_render_manifest_set_computation(&conventional,"fractal.compute.scalar.v1",
  1,1,"succeeded",UINT64_C(0x1234))==FRACTAL_OK);
 CHECK(fractal_render_manifest_set_numeric(&conventional,FRACTAL_NUMERIC_BINARY64_V1_ID,
  1,FRACTAL_NUMERIC_ABI_VERSION,"compatible",
  fractal_numeric_execution_identity_v1(&fractal_numeric_binary64_v1))==FRACTAL_OK);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&parameters,&parameter_identity)==FRACTAL_OK);
 CHECK(fractal_render_manifest_set_formula(&conventional,
  FRACTAL_FORMULA_MANDELBROT_V1_ID,1,FRACTAL_FORMULA_INTERFACE_VERSION,
  FRACTAL_FORMULA_CONTRACT_VERSION,"compatible",
  fractal_formula_execution_identity_v1(&fractal_formula_mandelbrot_v1),
  parameter_identity)==FRACTAL_OK);
 CHECK(fractal_render_manifest_serialize_identity_json(&conventional,ja,sizeof(ja),&na)==FRACTAL_OK&&
  strstr(ja,"\"computation_module\":\"fractal.compute.scalar.v1\"")!=NULL&&
  strstr(ja,"\"computation_assignment_count\":1")!=NULL&&
  strstr(ja,"\"numeric_module\":\"fractal.numeric.binary64.v1\"")!=NULL&&
  strstr(ja,"\"numeric_compatibility_status\":\"compatible\"")!=NULL&&
  strstr(ja,"\"formula_module\":\"fractal.formula.mandelbrot.v1\"")!=NULL&&
  strstr(ja,"\"formula_module_version\":1")!=NULL&&
  strstr(ja,"\"formula_interface_version\":1")!=NULL&&
  strstr(ja,"\"formula_contract_version\":1")!=NULL&&
  strstr(ja,"\"formula_compatibility_status\":\"compatible\"")!=NULL&&
  strstr(ja,"\"formula_execution_identity\":\"82001a07c687c7b1\"")!=NULL&&
  strstr(ja,"\"formula_parameter_identity\":\"bd014f20cbe74288\"")!=NULL);
 CHECK(conventional.cdc_reference_sha256[0]=='\0');
 CHECK(fractal_render_manifest_init(&transitional_renderer,&spec,FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,FRACTAL_RENDERER_BACKEND_CDC_EXPERIMENTAL,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(transitional_renderer.cdc_reference_sha256[0]=='\0');
 CHECK(conventional.metrics.computation_backend==FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C);
 CHECK(fractal_render_manifest_init(&cdc,&spec,FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_SYSTEM)==FRACTAL_OK);
 CHECK(fractal_render_manifest_set_numeric(&cdc,FRACTAL_NUMERIC_BINARY64_V1_ID,1,
  FRACTAL_NUMERIC_ABI_VERSION,"compatible",
  fractal_numeric_execution_identity_v1(&fractal_numeric_binary64_v1))==FRACTAL_OK);
 CHECK(strcmp(cdc.cdc_reference_sha256,FRACTAL_CDC_PDF_SHA256)==0);
 CHECK(cdc.metrics.renderer_backend==conventional.metrics.renderer_backend && cdc.metrics.memory_backend==conventional.metrics.memory_backend && cdc.metrics.computation_backend!=conventional.metrics.computation_backend);
 CHECK(fractal_render_manifest_init(&ouro,&spec,FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL,FRACTAL_RENDERER_BACKEND_LEGACY_CPP,FRACTAL_MEMORY_BACKEND_OURO)==FRACTAL_OK);
 CHECK(ouro.metrics.computation_backend==cdc.metrics.computation_backend && ouro.metrics.renderer_backend==cdc.metrics.renderer_backend && ouro.metrics.memory_backend!=cdc.metrics.memory_backend);
 CHECK(fractal_render_manifest_serialize_identity_json(&cdc,ja,sizeof(ja),&na)==FRACTAL_OK);
 CHECK(fractal_render_manifest_serialize_identity_json(&cdc,jb,sizeof(jb),&nb)==FRACTAL_OK);
 CHECK(na==nb && memcmp(ja,jb,na)==0 && strstr(ja,"\"computation_backend\":\"cdc-experimental\"")!=NULL);
}

static void test_milestone_files_preserved(void) {
 const char *paths[]={"CDC.pdf","docs/architecture/retired_standalone_renderers.md",
  "docs/architecture/standalone_renderer_knowledge_audit.md","index.html",
  "frames/frame_0000.bmp","render.mp4","docs/architecture/retired_native_http_server.md",
  "docs/architecture/retired_foreign_executable_bundle.md",
  "docs/cdc_reference_index.md","docs/cdc_interpretation_ledger.md"}; size_t i;
 for(i=0;i<sizeof(paths)/sizeof(paths[0]);++i) { FILE *f=fopen(paths[i],"rb"); long size=-1; CHECK(f!=NULL); if(f){ CHECK(fseek(f,0,SEEK_END)==0); size=ftell(f); fclose(f); CHECK(size>0); } }
}

int main(void) {
 test_spec(); test_memory(); test_adapter(); test_computation_backends(); test_experiment_0_certificate(); test_region_study(); test_two_child_study(); test_depth_two_study(); test_refinement_contracts(); test_renderer_backends_and_manifest(); test_milestone_files_preserved();
 if(failures) fprintf(stderr,"%d native checks failed\n",failures);
 return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
