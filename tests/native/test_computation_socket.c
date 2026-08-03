#include "fractal/installed_modules.h"

#include <stdio.h>
#include <string.h>

static int failures;
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);failures++;} } while(0)

static fractal_runtime_modules direct_runtime(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric,const fractal_compute_vtable *compute,
 const fractal_scheduler_vtable *scheduler,uint32_t workers){
 fractal_runtime_modules runtime={0};
 runtime.formula=formula;runtime.numeric=numeric;runtime.compute=compute;
 runtime.refinement=&fractal_refinement_none;runtime.scheduler=scheduler;
 runtime.raster=&fractal_raster_native;runtime.encoder=&fractal_encoder_bmp;
 runtime.memory_module=&fractal_memory_system_module;
 runtime.telemetry=&fractal_telemetry_noop;runtime.platform=&fractal_platform_host;
 runtime.scheduler_options.requested_worker_count=workers;
 CHECK(fractal_analysis_pipeline_init_many(&runtime.analysis,NULL,NULL,0)==FRACTAL_OK);
 return runtime;
}

static fractal_job_spec mandelbrot_job(fractal_formula_parameters parameters,
 uint32_t width,uint32_t height){
 fractal_job_spec job={{"formula.mandelbrot.quadratic",parameters,64},
  {-0.5,0.0,3.0,width,height},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","computation-socket.bmp"}};
 return job;
}

static fractal_runtime_selection selection(const char *computation){
 fractal_runtime_selection selected={0};
 selected.formula="formula.mandelbrot.quadratic";
 selected.numeric=FRACTAL_NUMERIC_BINARY64_V1_ID;selected.compute=computation;
 selected.refinement="refinement.none";selected.scheduler=FRACTAL_SCHEDULER_SERIAL_V1_ID;
 selected.raster="raster.native.iteration-bgr8";selected.encoder="encoder.bmp.v3";
 selected.memory="memory.system.scoped";selected.telemetry="telemetry.noop";
 selected.platform="platform.host.c11";selected.analysis_mode=FRACTAL_ANALYSIS_BORROW_INPUT;
 selected.scheduler_worker_count=1;return selected;
}

static void registry_and_assembly(void){
 fractal_module_registry installed,extended;
 fractal_module_registration entries[FRACTAL_REGISTRY_MAX_MODULES];
 fractal_memory_backend memory;fractal_runtime_modules runtime;
 fractal_runtime_selection selected=selection(FRACTAL_COMPUTATION_SCALAR_V1_ID);
 fractal_module_descriptor unavailable_descriptor=*fractal_compute_scalar_v1.descriptor;
 fractal_compute_vtable unavailable=fractal_compute_scalar_v1;size_t i;
 CHECK(fractal_installed_modules_registry(&installed)==FRACTAL_OK);
 CHECK(installed.count==22&&fractal_module_registry_count(&installed,FRACTAL_MODULE_COMPUTE)==2);
 CHECK(installed.identity==UINT64_C(0xd73d9e545afa7735));
 CHECK(fractal_module_registry_find(&installed,FRACTAL_MODULE_COMPUTE,
  FRACTAL_COMPUTATION_SCALAR_V1_ID)==fractal_compute_scalar_v1.descriptor);
 CHECK(fractal_module_registry_implementation(&installed,FRACTAL_MODULE_COMPUTE,
  FRACTAL_COMPUTATION_SCALAR_V1_ID)==&fractal_compute_scalar_v1);
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_OK);
 CHECK(runtime.compute==&fractal_compute_scalar_v1);
 selected.numeric="fractal.numeric.unknown.v1";
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 selected.numeric=FRACTAL_NUMERIC_BINARY64_V1_ID;
 selected.compute="fractal.compute.unknown";
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 for(i=0;i<installed.count;i++)entries[i]=installed.entries[i];
 unavailable_descriptor.module_id="fractal.compute.unavailable";
 unavailable_descriptor.display_name="Unavailable computation test";
 unavailable_descriptor.available=false;unavailable.descriptor=&unavailable_descriptor;
 entries[installed.count]=(fractal_module_registration){&unavailable_descriptor,&unavailable};
 CHECK(fractal_module_registry_init(&extended,entries,installed.count+1u)==FRACTAL_OK);
 selected.compute=unavailable_descriptor.module_id;
 CHECK(fractal_runtime_assemble(&extended,&selected,&memory,&runtime)==FRACTAL_ERROR_NOT_IMPLEMENTED);
 printf("COMPUTATION registry=%016llx modules=%zu\n",
  (unsigned long long)installed.identity,installed.count);
 fractal_memory_backend_shutdown(&memory);
}

static fractal_cancellation *mid_cancellation;
static unsigned mid_operations;
static fractal_result mid_add(const fractal_scalar *a,const fractal_scalar *b,fractal_scalar *out){
 if(++mid_operations==8u)fractal_cancellation_request(mid_cancellation);
 return fractal_numeric_binary64_v1.real_add(a,b,out);
}
static const fractal_module_descriptor mid_numeric_descriptor={
 1,1,"fractal.numeric.test.cancelling-binary64.v1","Cancelling binary64 test numeric",
 FRACTAL_MODULE_NUMERIC,FRACTAL_CAP_SCALAR_ARITHMETIC,true
};

static void computation_contract(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={"formula.mandelbrot.quadratic",&mandelbrot,sizeof(mandelbrot)};
 fractal_job_spec job=mandelbrot_job(parameters,4,4);
 fractal_runtime_modules runtime=direct_runtime(&fractal_formula_mandelbrot,
  &fractal_numeric_binary64_v1,&fractal_compute_scalar_v1,&fractal_scheduler_serial_v1,1);
 fractal_field_descriptor descriptor={4,4,5*sizeof(fractal_point_result_compact),
  FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
 fractal_computation_problem_v1 problem,compatibility_problem;
 fractal_sealed_work_unit_v1 assignments[2],repeat[2],bad;
 fractal_point_result_compact samples[20];fractal_mutable_field_view destination;
 fractal_computation_request_v1 request;fractal_computation_result_v1 result;
 fractal_numeric_vtable mid_numeric=fractal_numeric_binary64_v1;
 fractal_cancellation cancelled={true},mid={false};size_t count=0,repeat_count=0;unsigned i;
 mid_numeric.descriptor=&mid_numeric_descriptor;mid_numeric.real_add=mid_add;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&problem)==FRACTAL_OK);
 CHECK(fractal_computation_problem_validate_v1(&fractal_compute_scalar_v1,&problem)==FRACTAL_OK);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,&problem,2,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,
  assignments,2,&count)==FRACTAL_OK&&count==2);
 CHECK(fractal_scheduler_validate_computation_v1(&fractal_compute_scalar_v1,&problem,
  assignments,count,2)==FRACTAL_OK);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,&problem,2,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,
  repeat,2,&repeat_count)==FRACTAL_OK&&repeat_count==count&&
  !memcmp(assignments,repeat,sizeof(assignments)));
 CHECK(fractal_scheduler_work_unit_set_identity_v1(assignments,count)==
  fractal_scheduler_work_unit_set_identity_v1(repeat,repeat_count));
 CHECK(problem.identity==UINT64_C(0x2ba1bf582aa6ea82));
 CHECK(assignments[0].identity==UINT64_C(0x380b6409f268c139));
 CHECK(assignments[1].identity==UINT64_C(0x13bb870b5cc81b51));
 CHECK(fractal_scheduler_work_unit_set_identity_v1(assignments,count)==UINT64_C(0x8b3dca5188141c05));
 printf("COMPUTATION assignments=%016llx,%016llx set=%016llx\n",
  (unsigned long long)assignments[0].identity,(unsigned long long)assignments[1].identity,
  (unsigned long long)fractal_scheduler_work_unit_set_identity_v1(assignments,count));

 memset(samples,0xa5,sizeof(samples));
 destination=(fractal_mutable_field_view){descriptor,samples,sizeof(samples)};
 request=(fractal_computation_request_v1){1,FRACTAL_COMPUTATION_CONTRACT_VERSION,
  &problem,&assignments[1],destination,NULL};
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_OK);
 CHECK(result.status==FRACTAL_COMPUTATION_SUCCEEDED&&result.sequence==1&&
  result.rows_completed==2&&result.samples_completed==8);
 for(i=0;i<10;i++)CHECK(samples[i].steps==UINT32_C(0xa5a5a5a5));
 for(i=0;i<4;i++)CHECK(samples[10+i].steps!=UINT32_C(0xa5a5a5a5));
 CHECK(samples[14].steps==UINT32_C(0xa5a5a5a5));
 for(i=0;i<4;i++)CHECK(samples[15+i].steps!=UINT32_C(0xa5a5a5a5));
 CHECK(samples[19].steps==UINT32_C(0xa5a5a5a5));

 memset(samples,0xa5,sizeof(samples));bad=assignments[1];bad.row_end=5;bad.sample_end=20;
 bad.identity=fractal_scheduler_work_unit_identity_v1(&bad);request.assignment=&bad;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 for(i=0;i<20;i++)CHECK(samples[i].steps==UINT32_C(0xa5a5a5a5));
 request.assignment=&assignments[1];request.destination.data_size=sizeof(samples)-1u;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 for(i=0;i<20;i++)CHECK(samples[i].steps==UINT32_C(0xa5a5a5a5));

 request.destination=destination;bad=assignments[1];bad.numeric_identity^=1u;
 bad.identity=fractal_scheduler_work_unit_identity_v1(&bad);request.assignment=&bad;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 bad=assignments[1];bad.formula_identity^=1u;
 bad.identity=fractal_scheduler_work_unit_identity_v1(&bad);request.assignment=&bad;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 bad=assignments[1];bad.cancellation_mode=99;
 bad.identity=fractal_scheduler_work_unit_identity_v1(&bad);request.assignment=&bad;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 problem.parameters.type_id="formula.julia.quadratic";request.assignment=&assignments[1];
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 problem.parameters=parameters;

 bad=assignments[0];bad.row_end=3;bad.sample_end=12;
 bad.identity=fractal_scheduler_work_unit_identity_v1(&bad);assignments[0]=bad;
 CHECK(fractal_scheduler_validate_computation_v1(&fractal_compute_scalar_v1,&problem,
  assignments,2,2)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,&problem,2,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,
  assignments,2,&count)==FRACTAL_OK);

 memset(samples,0xa5,sizeof(samples));request.assignment=&assignments[0];
 request.cancellation=&cancelled;
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_CANCELLED&&
  result.status==FRACTAL_COMPUTATION_CANCELLED);
 for(i=0;i<20;i++)CHECK(samples[i].steps==UINT32_C(0xa5a5a5a5));

 runtime.numeric=&mid_numeric;mid_operations=0;mid_cancellation=&mid;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&problem)==FRACTAL_OK);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,&problem,1,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,
  assignments,2,&count)==FRACTAL_OK);
 request=(fractal_computation_request_v1){1,FRACTAL_COMPUTATION_CONTRACT_VERSION,
  &problem,&assignments[0],destination,&mid};
 CHECK(fractal_compute_scalar_v1.execute(&request,&result)==FRACTAL_ERROR_CANCELLED&&
  fractal_cancellation_is_requested(&mid)&&result.status==FRACTAL_COMPUTATION_CANCELLED);

 descriptor.stride=1;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&problem)==FRACTAL_ERROR_INVALID_SPEC);
 descriptor=(fractal_field_descriptor){4,4,5*sizeof(fractal_point_result_compact),
  (fractal_field_format)99,0};
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&problem)==FRACTAL_ERROR_INVALID_SPEC);

 descriptor=(fractal_field_descriptor){4,4,5*sizeof(fractal_point_result_compact),
  FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};runtime.numeric=&fractal_numeric_binary64_v1;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&problem)==FRACTAL_OK);
 runtime.compute=&fractal_compute_conventional;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&descriptor,&compatibility_problem)==FRACTAL_OK);
 CHECK(problem.identity!=compatibility_problem.identity);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,&problem,1,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,assignments,2,&count)==FRACTAL_OK);
 memset(samples,0xa5,sizeof(samples));
 request=(fractal_computation_request_v1){1,FRACTAL_COMPUTATION_CONTRACT_VERSION,
  &problem,&assignments[0],destination,NULL};
 CHECK(fractal_compute_conventional.execute(&request,&result)==FRACTAL_ERROR_INVALID_SPEC);
 for(i=0;i<20;i++)CHECK(samples[i].steps==UINT32_C(0xa5a5a5a5));
 printf("COMPUTATION work-unit=%016llx\n",(unsigned long long)problem.identity);
}

static void compatibility_and_metadata_validation(void){
 fractal_memory_backend memory;fractal_runtime_modules runtime;
 fractal_module_descriptor bad_descriptor=*fractal_compute_scalar_v1.descriptor;
 fractal_module_descriptor bad_numeric_descriptor=*fractal_numeric_binary64_v1.descriptor;
 fractal_module_descriptor bad_formula_descriptor=*fractal_formula_mandelbrot.descriptor;
 fractal_compute_vtable bad_compute=fractal_compute_scalar_v1;
 fractal_numeric_vtable bad_numeric=fractal_numeric_binary64_v1;
 fractal_formula_vtable bad_formula=fractal_formula_mandelbrot;
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 runtime=direct_runtime(&fractal_formula_mandelbrot,&fractal_numeric_binary64_v1,
  &fractal_compute_scalar_v1,&fractal_scheduler_serial_v1,1);runtime.memory=&memory;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_OK);
 bad_descriptor.abi_version=2;bad_compute.descriptor=&bad_descriptor;runtime.compute=&bad_compute;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 bad_descriptor=*fractal_compute_scalar_v1.descriptor;bad_descriptor.kind=FRACTAL_MODULE_FORMULA;
 bad_compute=fractal_compute_scalar_v1;bad_compute.descriptor=&bad_descriptor;runtime.compute=&bad_compute;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 bad_descriptor=*fractal_compute_scalar_v1.descriptor;bad_compute=fractal_compute_scalar_v1;
 bad_compute.descriptor=&bad_descriptor;bad_compute.contract_version=2;runtime.compute=&bad_compute;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 runtime.compute=&fractal_compute_scalar_v1;bad_numeric_descriptor.capability_flags=0;
 bad_numeric.descriptor=&bad_numeric_descriptor;runtime.numeric=&bad_numeric;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 bad_numeric=fractal_numeric_binary64_v1;bad_numeric.abi_version=2;runtime.numeric=&bad_numeric;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 bad_numeric=fractal_numeric_binary64_v1;
 bad_numeric.capability_flags&=~FRACTAL_NUMERIC_CAP_REAL_DIVIDE;runtime.numeric=&bad_numeric;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 runtime.numeric=&fractal_numeric_binary64_v1;bad_formula_descriptor.capability_flags=0;
 bad_formula.descriptor=&bad_formula_descriptor;runtime.formula=&bad_formula;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 runtime.formula=&fractal_formula_mandelbrot;bad_compute=fractal_compute_scalar_v1;
 bad_compute.required_field_capabilities=FRACTAL_CAP_BGR8;runtime.compute=&bad_compute;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_compute_conventional.point(&fractal_formula_mandelbrot,
  &fractal_numeric_binary64_v1,NULL,0.0,0.0,64,NULL,NULL)==FRACTAL_ERROR_INVALID_ARGUMENT);
 fractal_memory_backend_shutdown(&memory);
}

int main(void){
 registry_and_assembly();computation_contract();compatibility_and_metadata_validation();
 return failures?1:0;
}
