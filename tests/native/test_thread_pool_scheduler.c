#include "fractal/installed_modules.h"
#include "fractal/socketable_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures;
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);failures++;} } while(0)

#define CAPTURE_CAPACITY 65536u

typedef struct capture {
 unsigned char bytes[CAPTURE_CAPACITY];
 fractal_runtime_output output;
 fractal_artifact_result artifact;
 char runtime_manifest[4096];
 char artifact_manifest[4096];
 size_t runtime_manifest_size,artifact_manifest_size;
} capture;

static fractal_runtime_modules direct_runtime(fractal_memory_backend *memory,
 const fractal_formula_vtable *formula,const fractal_scheduler_vtable *scheduler,
 uint32_t workers){
 fractal_runtime_modules runtime={0};
 runtime.formula=formula;
 runtime.numeric=&fractal_numeric_binary64_v1;
 runtime.compute=&fractal_compute_scalar_v1;
 runtime.refinement=&fractal_refinement_none;
 runtime.scheduler=scheduler;
 runtime.raster=&fractal_raster_native;
 runtime.encoder=&fractal_encoder_bmp;
 runtime.memory_module=&fractal_memory_system_module;
 runtime.memory=memory;
 runtime.telemetry=&fractal_telemetry_noop;
 runtime.platform=&fractal_platform_host;
 runtime.scheduler_options.requested_worker_count=workers;
 CHECK(fractal_analysis_pipeline_init_many(&runtime.analysis,NULL,NULL,0)==FRACTAL_OK);
 return runtime;
}

static void install_three_analyzers(fractal_runtime_modules *runtime){
 const fractal_analyzer_vtable *analyzers[3]={
  &fractal_analyzer_escape_classification_summary,
  &fractal_analyzer_iteration_histogram,
  &fractal_analyzer_spatial_workload_grid
 };
 fractal_analysis_request requests[3]={
  {"fractal.analyzer.escape-classification-summary",FRACTAL_ANALYSIS_BORROW_INPUT,true,true,1},
  {"fractal.analyzer.iteration-histogram",FRACTAL_ANALYSIS_BORROW_INPUT,true,true,1},
  {"fractal.analyzer.spatial-workload-grid",FRACTAL_ANALYSIS_BORROW_INPUT,true,true,1}
 };
 CHECK(fractal_analysis_pipeline_init_many(&runtime->analysis,analyzers,requests,3)==FRACTAL_OK);
}

static fractal_result render_capture(const fractal_formula_vtable *formula,
 const fractal_formula_parameters *parameters,const fractal_scheduler_vtable *scheduler,
 uint32_t workers,uint32_t width,uint32_t height,bool analyzers,capture *captured){
 fractal_memory_backend memory;
 fractal_runtime_modules runtime;
 fractal_job_spec job={{formula->descriptor->module_id,*parameters,64},
  {-0.5,0.0,3.0,width,height},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","thread-pool-fixture.bmp"}};
 fractal_memory_sink_state state;
 fractal_artifact_sink sink;
 fractal_module_registry registry;
 fractal_result result;
 memset(captured,0,sizeof(*captured));
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 runtime=direct_runtime(&memory,formula,scheduler,workers);
 if(analyzers)install_three_analyzers(&runtime);
 fractal_memory_sink_init(&state,captured->bytes,sizeof(captured->bytes));
 sink=(fractal_artifact_sink){&fractal_artifact_sink_memory,&state};
 result=fractal_runtime_render_artifact(&runtime,&job,&sink,NULL,&captured->output,&captured->artifact);
 if(result==FRACTAL_OK){
  CHECK(fractal_runtime_manifest(&runtime,&job,&captured->output,captured->runtime_manifest,
   sizeof(captured->runtime_manifest),&captured->runtime_manifest_size)==FRACTAL_OK);
  CHECK(fractal_installed_modules_registry(&registry)==FRACTAL_OK);
  CHECK(fractal_runtime_artifact_manifest(&registry,&runtime,&job,&captured->output,
   &captured->artifact,captured->artifact_manifest,sizeof(captured->artifact_manifest),
   &captured->artifact_manifest_size)==FRACTAL_OK);
 }
 fractal_memory_backend_shutdown(&memory);
 return result;
}

static void decomposition_contract(void){
 fractal_sealed_work_unit_v1 a[FRACTAL_THREAD_POOL_MAX_WORKERS];
 fractal_sealed_work_unit_v1 b[FRACTAL_THREAD_POOL_MAX_WORKERS];
 size_t count_a=0,count_b=0;
 uint64_t identity;
 CHECK(fractal_scheduler_decompose_contiguous_rows_v1(11,7,3,a,
  FRACTAL_THREAD_POOL_MAX_WORKERS,&count_a)==FRACTAL_OK);
 CHECK(count_a==3&&a[0].row_begin==0&&a[0].row_end==2&&
  a[1].row_begin==2&&a[1].row_end==4&&a[2].row_begin==4&&a[2].row_end==7);
 CHECK(a[0].sample_begin==0&&a[0].sample_end==22&&a[1].sample_begin==22&&
  a[1].sample_end==44&&a[2].sample_begin==44&&a[2].sample_end==77);
 CHECK(fractal_scheduler_validate_contiguous_rows_v1(a,count_a,11,7,3)==FRACTAL_OK);
 identity=fractal_scheduler_work_unit_set_identity_v1(a,count_a);
 CHECK(identity!=0);
 CHECK(fractal_scheduler_decompose_contiguous_rows_v1(11,7,3,b,
  FRACTAL_THREAD_POOL_MAX_WORKERS,&count_b)==FRACTAL_OK);
 CHECK(count_a==count_b&&!memcmp(a,b,sizeof(a[0])*count_a));
 CHECK(fractal_scheduler_work_unit_set_identity_v1(b,count_b)==identity);

 CHECK(fractal_scheduler_decompose_contiguous_rows_v1(5,2,4,a,
  FRACTAL_THREAD_POOL_MAX_WORKERS,&count_a)==FRACTAL_OK);
 CHECK(a[0].row_begin==0&&a[0].row_end==0&&a[0].sample_begin==a[0].sample_end);
 CHECK(a[1].row_begin==0&&a[1].row_end==1);
 CHECK(a[2].row_begin==1&&a[2].row_end==1&&a[2].sample_begin==a[2].sample_end);
 CHECK(a[3].row_begin==1&&a[3].row_end==2);
 CHECK(fractal_scheduler_validate_contiguous_rows_v1(a,count_a,5,2,4)==FRACTAL_OK);
 a[1].row_begin=1;
 a[1].identity=fractal_scheduler_work_unit_identity_v1(&a[1]);
 CHECK(fractal_scheduler_validate_contiguous_rows_v1(a,count_a,5,2,4)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_scheduler_decompose_contiguous_rows_v1(5,2,0,a,
  FRACTAL_THREAD_POOL_MAX_WORKERS,&count_a)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_scheduler_decompose_contiguous_rows_v1(5,2,FRACTAL_THREAD_POOL_MAX_WORKERS+1u,
  a,FRACTAL_THREAD_POOL_MAX_WORKERS,&count_a)==FRACTAL_ERROR_INVALID_SPEC);
}

static fractal_runtime_selection selection(const char *scheduler,uint32_t workers){
 fractal_runtime_selection selected={0};
 selected.formula=FRACTAL_FORMULA_MANDELBROT_V1_ID;
 selected.numeric=FRACTAL_NUMERIC_BINARY64_V1_ID;
 selected.compute=FRACTAL_COMPUTATION_SCALAR_V1_ID;
 selected.refinement="refinement.none";
 selected.scheduler=scheduler;
 selected.raster="raster.native.iteration-bgr8";
 selected.encoder="encoder.bmp.v3";
 selected.memory="memory.system.scoped";
 selected.telemetry="telemetry.noop";
 selected.platform="platform.host.c11";
 selected.analysis_mode=FRACTAL_ANALYSIS_BORROW_INPUT;
 selected.scheduler_worker_count=workers;
 return selected;
}

static void registry_and_worker_validation(void){
 fractal_module_registry first,second;
 fractal_memory_backend memory;
 fractal_runtime_modules runtime;
 fractal_runtime_selection selected;
 CHECK(fractal_installed_modules_registry(&first)==FRACTAL_OK);
 CHECK(fractal_installed_modules_registry(&second)==FRACTAL_OK);
 CHECK(first.identity==second.identity&&first.count==22);
 CHECK(fractal_module_registry_count(&first,FRACTAL_MODULE_COMPUTE)==2);
 CHECK(fractal_module_registry_implementation(&first,FRACTAL_MODULE_COMPUTE,
  FRACTAL_COMPUTATION_SCALAR_V1_ID)==&fractal_compute_scalar_v1);
 CHECK(fractal_module_registry_count(&first,FRACTAL_MODULE_SCHEDULER)==3);
 CHECK(fractal_module_registry_implementation(&first,FRACTAL_MODULE_SCHEDULER,
  FRACTAL_SCHEDULER_SERIAL_V1_ID)==&fractal_scheduler_serial_v1);
 CHECK(fractal_module_registry_implementation(&first,FRACTAL_MODULE_SCHEDULER,
  FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)==&fractal_scheduler_thread_pool_v1);
 CHECK(fractal_module_registry_implementation(&first,FRACTAL_MODULE_SCHEDULER,
  FRACTAL_SCHEDULER_SERIAL_COMPATIBILITY_ID)==&fractal_scheduler_serial);
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 selected=selection(FRACTAL_SCHEDULER_THREAD_POOL_V1_ID,0);
 CHECK(fractal_runtime_assemble(&first,&selected,&memory,&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 selected.scheduler_worker_count=FRACTAL_THREAD_POOL_MAX_WORKERS+1u;
 CHECK(fractal_runtime_assemble(&first,&selected,&memory,&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 selected.scheduler_worker_count=1;
 CHECK(fractal_runtime_assemble(&first,&selected,&memory,&runtime)==FRACTAL_OK&&
  runtime.scheduler==&fractal_scheduler_thread_pool_v1);
 selected=selection(FRACTAL_SCHEDULER_SERIAL_V1_ID,1);
 CHECK(fractal_runtime_assemble(&first,&selected,&memory,&runtime)==FRACTAL_OK&&
  runtime.scheduler==&fractal_scheduler_serial_v1);
 fractal_memory_backend_shutdown(&memory);
}

static void compare_capture(const capture *serial,const capture *parallel){
 size_t i;
 CHECK(serial->output.field_checksum==parallel->output.field_checksum);
 CHECK(serial->output.source_field_checksum==parallel->output.source_field_checksum);
 CHECK(serial->output.analyzed_field_checksum==parallel->output.analyzed_field_checksum);
 CHECK(serial->output.pixel_checksum==parallel->output.pixel_checksum);
 CHECK(serial->output.artifact_checksum==parallel->output.artifact_checksum);
 CHECK(serial->artifact.byte_count==parallel->artifact.byte_count);
 CHECK(!memcmp(serial->bytes,parallel->bytes,serial->artifact.byte_count));
 CHECK(serial->output.analysis_result.records_produced==parallel->output.analysis_result.records_produced);
 for(i=0;i<serial->output.analysis_result.records_produced;i++){
  const fractal_analysis_record *a=&serial->output.analysis_result.records[i];
  const fractal_analysis_record *b=&parallel->output.analysis_result.records[i];
  CHECK(a->identity==b->identity&&a->chain_ordinal==b->chain_ordinal&&
   a->type_id==b->type_id&&a->payload_size==b->payload_size&&
   !memcmp(a->payload,b->payload,a->payload_size));
 }
}

static void cross_scheduler_equivalence(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_julia_parameters julia={-0.8,0.156,2.0};
 fractal_formula_parameters mp={FRACTAL_FORMULA_MANDELBROT_V1_ID,&mandelbrot,sizeof(mandelbrot)};
 fractal_formula_parameters jp={FRACTAL_FORMULA_JULIA_V1_ID,&julia,sizeof(julia)};
 capture serial,one,many,julia_serial,julia_many,small_serial,small_many,repeat;
 unsigned i;
 CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_serial_v1,1,32,24,true,&serial)==FRACTAL_OK);
 CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_thread_pool_v1,1,32,24,true,&one)==FRACTAL_OK);
 CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_thread_pool_v1,5,32,24,true,&many)==FRACTAL_OK);
 compare_capture(&serial,&one);
 compare_capture(&serial,&many);
 CHECK(serial.output.field_checksum==UINT64_C(0x99ec88c2a0f8bac3));
 CHECK(serial.output.pixel_checksum==UINT64_C(0x4866aacc38290b5f));
 CHECK(serial.output.artifact_checksum==UINT64_C(0xfb1a83bd5ca28e5f));
 CHECK(serial.artifact.byte_count==2358);
 CHECK(serial.output.scheduler_execution.sealed_work_unit_identity==UINT64_C(0x96a863c0d998c6ec));
 CHECK(many.output.scheduler_execution.sealed_work_unit_identity==UINT64_C(0xd3fb04dd47115b77));
 CHECK(many.output.scheduler_execution.computation_identity==UINT64_C(0x22473db48a22836f));
 printf("SCHEDULER mandelbrot serial=%016llx thread-pool=%016llx computation=%016llx\n",
  (unsigned long long)serial.output.scheduler_execution.sealed_work_unit_identity,
  (unsigned long long)many.output.scheduler_execution.sealed_work_unit_identity,
  (unsigned long long)many.output.scheduler_execution.computation_identity);
 CHECK(many.output.scheduler_execution.requested_worker_count==5&&
  many.output.scheduler_execution.effective_worker_count==5&&
  many.output.scheduler_execution.assignment_count==5&&
  many.output.scheduler_execution.status==FRACTAL_SCHEDULER_EXECUTION_SUCCEEDED&&
  many.output.publication_status==FRACTAL_PUBLICATION_COMMITTED);
 CHECK(strstr(many.runtime_manifest,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"computation\":\"fractal.compute.scalar.v1\"")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"computation_version\":1")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"computation_status\":\"succeeded\"")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"numeric\":\"fractal.numeric.binary64.v1\"")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"numeric_abi_version\":1")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"numeric_compatibility\":\"compatible\"")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"numeric_execution_identity\":\"2f300bf0f7ea5244\"")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"numeric_version\":1")!=NULL);
 CHECK(strstr(many.runtime_manifest,FRACTAL_SCHEDULER_WORK_UNIT_V1_ID)!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"requested_worker_count\":5")!=NULL);
 CHECK(strstr(many.runtime_manifest,"\"execution_status\":\"succeeded\"")!=NULL);
 CHECK(strstr(many.artifact_manifest,"\"publication_status\":\"committed\"")!=NULL);
 CHECK(strstr(many.artifact_manifest,"\"computation\":\"fractal.compute.scalar.v1\"")!=NULL);
 CHECK(strstr(many.artifact_manifest,"\"numeric\":\"fractal.numeric.binary64.v1\"")!=NULL);
 CHECK(strstr(many.artifact_manifest,"\"numeric_compatibility\":\"compatible\"")!=NULL);
 CHECK(many.output.analysis_result.records_produced==3&&
  many.output.analysis_result.records[0].chain_ordinal==0&&
  many.output.analysis_result.records[1].chain_ordinal==1&&
  many.output.analysis_result.records[2].chain_ordinal==2);

 CHECK(render_capture(&fractal_formula_julia,&jp,&fractal_scheduler_serial_v1,1,32,24,true,&julia_serial)==FRACTAL_OK);
 CHECK(render_capture(&fractal_formula_julia,&jp,&fractal_scheduler_thread_pool_v1,4,32,24,true,&julia_many)==FRACTAL_OK);
 compare_capture(&julia_serial,&julia_many);
 CHECK(julia_serial.output.field_checksum==UINT64_C(0x0fb4458e08bad6e1));
 CHECK(julia_serial.output.pixel_checksum==UINT64_C(0xb272f08b0bbdca2b));
 CHECK(julia_serial.output.artifact_checksum==UINT64_C(0x4d4aa95bd137ec87));
 CHECK(julia_serial.output.scheduler_execution.sealed_work_unit_identity==UINT64_C(0x21097345e25c6813));
 CHECK(julia_many.output.scheduler_execution.sealed_work_unit_identity==UINT64_C(0xde0db50a4d358e75));
 CHECK(julia_many.output.scheduler_execution.computation_identity==UINT64_C(0xca95490ab8d9c6d3));
 printf("SCHEDULER julia serial=%016llx thread-pool=%016llx computation=%016llx\n",
  (unsigned long long)julia_serial.output.scheduler_execution.sealed_work_unit_identity,
  (unsigned long long)julia_many.output.scheduler_execution.sealed_work_unit_identity,
  (unsigned long long)julia_many.output.scheduler_execution.computation_identity);

 CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_serial_v1,1,5,2,false,&small_serial)==FRACTAL_OK);
 CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_thread_pool_v1,4,5,2,false,&small_many)==FRACTAL_OK);
 compare_capture(&small_serial,&small_many);
 for(i=0;i<3;i++){
  CHECK(render_capture(&fractal_formula_mandelbrot,&mp,&fractal_scheduler_thread_pool_v1,5,32,24,true,&repeat)==FRACTAL_OK);
  compare_capture(&many,&repeat);
  CHECK(many.output.scheduler_execution.sealed_work_unit_identity==
   repeat.output.scheduler_execution.sealed_work_unit_identity);
  CHECK(many.runtime_manifest_size==repeat.runtime_manifest_size&&
   !memcmp(many.runtime_manifest,repeat.runtime_manifest,many.runtime_manifest_size));
  CHECK(many.artifact_manifest_size==repeat.artifact_manifest_size&&
   !memcmp(many.artifact_manifest,repeat.artifact_manifest,many.artifact_manifest_size));
 }
}

static fractal_cancellation *cancellation_to_request;
static fractal_result cancellation_point(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric,const fractal_formula_parameters *parameters,
 double real,double imaginary,uint32_t budget,const fractal_cancellation *unused,
 fractal_point_result_compact *output){
 fractal_result result;
 (void)unused;
 result=fractal_compute_conventional.point(formula,numeric,parameters,real,imaginary,budget,NULL,output);
 if(result==FRACTAL_OK&&real>0.5&&imaginary>0.5)
  fractal_cancellation_request(cancellation_to_request);
 return result;
}

static fractal_result failure_point(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric,const fractal_formula_parameters *parameters,
 double real,double imaginary,uint32_t budget,const fractal_cancellation *cancel,
 fractal_point_result_compact *output){
 return fractal_compute_scalar_v1.point(formula,numeric,parameters,real,imaginary,
  budget,cancel,output);
}

static const fractal_module_descriptor cancellation_compute_descriptor={
 1,1,"compute.test.cancellation-boundary","Cancellation boundary test compute",
 FRACTAL_MODULE_COMPUTE,FRACTAL_CAP_POINT_SCALAR|FRACTAL_CAP_ITERATION_FIELD|
 FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|
 FRACTAL_CAP_COMPUTE_CANCELLATION,true
};
static const fractal_module_descriptor failure_compute_descriptor={
 1,1,"compute.test.failure-selection","Failure selection test compute",
 FRACTAL_MODULE_COMPUTE,FRACTAL_CAP_POINT_SCALAR|FRACTAL_CAP_ITERATION_FIELD|
 FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|
 FRACTAL_CAP_COMPUTE_CANCELLATION,true
};
static fractal_result cancellation_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *result){
 if(!request||!request->assignment||!result)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->assignment_identity=request->assignment->identity;
 result->sequence=request->assignment->sequence;fractal_cancellation_request(cancellation_to_request);
 result->status=FRACTAL_COMPUTATION_CANCELLED;result->result=FRACTAL_ERROR_CANCELLED;
 return result->result;
}
static fractal_result failure_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *result){
 if(!request||!request->assignment||!result)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->assignment_identity=request->assignment->identity;
 result->sequence=request->assignment->sequence;result->status=FRACTAL_COMPUTATION_FAILED;
 result->result=request->assignment->sequence==0?FRACTAL_ERROR_IO:FRACTAL_ERROR_INVALID_SPEC;
 return result->result;
}
static const fractal_compute_vtable cancellation_compute={
 &cancellation_compute_descriptor,FRACTAL_CAP_POINT_SCALAR,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 FRACTAL_CAP_ITERATION_FIELD,FRACTAL_COMPUTATION_CONTRACT_VERSION,cancellation_point,
 cancellation_execute
};
static const fractal_compute_vtable failure_compute={
 &failure_compute_descriptor,FRACTAL_CAP_POINT_SCALAR,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 FRACTAL_CAP_ITERATION_FIELD,FRACTAL_COMPUTATION_CONTRACT_VERSION,failure_point,
 failure_execute
};

typedef struct observing_sink_state {
 unsigned begins,writes,commits,aborts,phase;
} observing_sink_state;
static fractal_result observing_begin(void *state,const fractal_artifact_begin_info *info){
 observing_sink_state *sink=state;
 if(!sink||!info||sink->phase)return FRACTAL_ERROR_INVALID_ARGUMENT;
 sink->begins++;sink->phase=1;return FRACTAL_OK;
}
static fractal_result observing_write(void *state,const void *data,size_t size){
 observing_sink_state *sink=state;(void)data;(void)size;
 if(!sink||sink->phase!=1)return FRACTAL_ERROR_INVALID_ARGUMENT;
 sink->writes++;return FRACTAL_OK;
}
static fractal_result observing_commit(void *state,fractal_artifact_result *result){
 observing_sink_state *sink=state;(void)result;
 if(!sink||sink->phase!=1)return FRACTAL_ERROR_INVALID_ARGUMENT;
 sink->commits++;sink->phase=2;return FRACTAL_OK;
}
static void observing_abort(void *state){
 observing_sink_state *sink=state;
 if(sink&&sink->phase==1){sink->aborts++;sink->phase=3;}
}
static const fractal_module_descriptor observing_sink_descriptor={
 1,1,"sink.test.observing","Observing transactional sink",FRACTAL_MODULE_ARTIFACT_SINK,
 FRACTAL_CAP_SINK_BINARY_BYTES|FRACTAL_CAP_SINK_COMMIT_ABORT,true
};
static const fractal_artifact_sink_vtable observing_sink={
 &observing_sink_descriptor,FRACTAL_ARTIFACT_DESTINATION_MEMORY,
 observing_begin,observing_write,observing_commit,observing_abort
};

static void cancellation_failure_and_publication(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,&mandelbrot,sizeof(mandelbrot)};
 fractal_job_spec job={{FRACTAL_FORMULA_MANDELBROT_V1_ID,parameters,64},
  {0.0,0.0,2.0,4,4},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","failure.bmp"}};
 fractal_point_result_compact samples[16];
 fractal_field field={4,4,4*sizeof(samples[0]),samples,0,false};
 fractal_memory_backend memory;
 fractal_runtime_modules runtime;
 fractal_cancellation cancel={false},cancelled={true};
 observing_sink_state observed={0};
 observing_sink_state cancelled_observed={0};
 fractal_artifact_sink sink={&observing_sink,&observed};
 fractal_artifact_sink cancelled_sink={&observing_sink,&cancelled_observed};
 fractal_runtime_output output;
 fractal_artifact_result artifact;
 memset(samples,0xa5,sizeof(samples));
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 runtime=direct_runtime(&memory,&fractal_formula_mandelbrot,&fractal_scheduler_thread_pool_v1,2);
 CHECK(runtime.scheduler->execute(&runtime,&job,&field,&cancelled,NULL,NULL)==FRACTAL_ERROR_CANCELLED);
 CHECK(field.completed_rows==0&&!field.complete&&samples[0].steps==UINT32_C(0xa5a5a5a5));
 CHECK(fractal_runtime_render_artifact(&runtime,&job,&cancelled_sink,&cancelled,
  &output,&artifact)==FRACTAL_ERROR_CANCELLED);
 CHECK(cancelled_observed.begins==1&&cancelled_observed.writes==0&&
  cancelled_observed.commits==0&&cancelled_observed.aborts==1&&
  output.publication_status==FRACTAL_PUBLICATION_ABORTED);

 runtime.scheduler_options.requested_worker_count=1;
 runtime.compute=&cancellation_compute;
 cancellation_to_request=&cancel;
 memset(samples,0,sizeof(samples));field.completed_rows=0;field.complete=false;
 CHECK(runtime.scheduler->execute(&runtime,&job,&field,&cancel,NULL,NULL)==FRACTAL_ERROR_CANCELLED);
 CHECK(fractal_cancellation_is_requested(&cancel)&&field.completed_rows==0&&!field.complete);

 runtime.scheduler_options.requested_worker_count=2;
 runtime.compute=&failure_compute;
 install_three_analyzers(&runtime);
 fractal_cancellation_reset(&cancel);
 memset(samples,0,sizeof(samples));field.completed_rows=0;field.complete=false;
 runtime.scheduler=&fractal_scheduler_serial_v1;
 CHECK(runtime.scheduler->execute(&runtime,&job,&field,&cancel,NULL,NULL)==FRACTAL_ERROR_IO);
 CHECK(field.completed_rows==0&&!field.complete);
 runtime.scheduler=&fractal_scheduler_thread_pool_v1;
 CHECK(runtime.scheduler->execute(&runtime,&job,&field,&cancel,NULL,NULL)==FRACTAL_ERROR_IO);
 CHECK(field.completed_rows==0&&!field.complete);
 CHECK(fractal_runtime_render_artifact(&runtime,&job,&sink,NULL,&output,&artifact)==FRACTAL_ERROR_IO);
 CHECK(observed.begins==1&&observed.writes==0&&observed.commits==0&&observed.aborts==1);
 CHECK(!artifact.committed&&output.publication_status==FRACTAL_PUBLICATION_ABORTED&&
  output.scheduler_execution.status==FRACTAL_SCHEDULER_EXECUTION_FAILED&&
  output.analysis_result.records_produced==0&&output.analysis_result.samples_examined==0&&
  output.field_checksum==0&&output.pixel_checksum==0&&output.artifact_bytes==0);
 fractal_memory_backend_shutdown(&memory);
}

static void memory_file_parity(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,&mandelbrot,sizeof(mandelbrot)};
 fractal_job_spec job={{FRACTAL_FORMULA_MANDELBROT_V1_ID,parameters,64},
  {-0.5,0.0,3.0,32,24},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","thread_pool_file_fixture.bmp"}};
 fractal_memory_backend memory;
 fractal_runtime_modules runtime;
 fractal_memory_sink_state memory_state;
 fractal_file_sink_state file_state;
 fractal_artifact_sink memory_sink,file_sink;
 fractal_runtime_output memory_output,file_output;
 fractal_artifact_result memory_artifact,file_artifact;
 unsigned char bytes[4096],disk[4096];
 size_t count=0;
 FILE *file;
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 runtime=direct_runtime(&memory,&fractal_formula_mandelbrot,&fractal_scheduler_thread_pool_v1,4);
 fractal_memory_sink_init(&memory_state,bytes,sizeof(bytes));
 memory_sink=(fractal_artifact_sink){&fractal_artifact_sink_memory,&memory_state};
 CHECK(fractal_runtime_render_artifact(&runtime,&job,&memory_sink,NULL,&memory_output,&memory_artifact)==FRACTAL_OK);
 CHECK(fractal_file_sink_init(&file_state,"thread_pool_file_fixture.bmp")==FRACTAL_OK);
 file_sink=(fractal_artifact_sink){&fractal_artifact_sink_host_file,&file_state};
 CHECK(fractal_runtime_render_artifact(&runtime,&job,&file_sink,NULL,&file_output,&file_artifact)==FRACTAL_OK);
 file=fopen("thread_pool_file_fixture.bmp","rb");
 CHECK(file!=NULL);
 if(file){count=fread(disk,1,sizeof(disk),file);fclose(file);}
 CHECK(count==memory_artifact.byte_count&&count==file_artifact.byte_count&&
  !memcmp(bytes,disk,count)&&memory_artifact.encoded_checksum==file_artifact.encoded_checksum);
 CHECK(memory_output.field_checksum==file_output.field_checksum&&
  memory_output.pixel_checksum==file_output.pixel_checksum);
 (void)remove("thread_pool_file_fixture.bmp");
 fractal_memory_backend_shutdown(&memory);
}

int main(void){
 decomposition_contract();
 registry_and_worker_validation();
 cross_scheduler_equivalence();
 cancellation_failure_and_publication();
 memory_file_parity();
 return failures?1:0;
}
