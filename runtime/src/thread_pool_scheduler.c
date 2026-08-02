#include "fractal/socketable_runtime.h"

#include <stdio.h>
#include <stdatomic.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

static const fractal_module_descriptor serial_v1_descriptor={
 1u,1u,FRACTAL_SCHEDULER_SERIAL_V1_ID,"Serial reference scheduler v1",
 FRACTAL_MODULE_SCHEDULER,FRACTAL_CAP_SERIAL_FIELD|FRACTAL_CAP_ITERATION_FIELD,true
};
static const fractal_module_descriptor thread_pool_v1_descriptor={
 1u,1u,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID,"Bounded deterministic thread-pool scheduler v1",
 FRACTAL_MODULE_SCHEDULER,FRACTAL_CAP_ITERATION_FIELD|FRACTAL_CAP_BOUNDED_THREAD_POOL,true
};

static uint64_t identity_from_values(const fractal_sealed_work_unit_v1 *u){
 char text[768];
 int n;
 if(!u)return 0;
 n=snprintf(text,sizeof(text),
  "%s|abi=%u|contract=%u|work=%016llx|compute=%016llx@%u|numeric=%016llx@%u|formula=%016llx@%u|field=%u|cancel=%u|sequence=%u|workers=%u|width=%u|height=%u|rows=%u:%u|samples=%llu:%llu",
  FRACTAL_SCHEDULER_WORK_UNIT_V1_ID,u->abi_version,u->contract_version,
  (unsigned long long)u->work_unit_identity,(unsigned long long)u->computation_identity,
  u->computation_version,(unsigned long long)u->numeric_identity,u->numeric_version,
  (unsigned long long)u->formula_identity,u->formula_version,u->field_format,u->cancellation_mode,u->sequence,u->worker_count,
  u->width,u->height,u->row_begin,u->row_end,
  (unsigned long long)u->sample_begin,(unsigned long long)u->sample_end);
 if(n<0||(size_t)n>=sizeof(text))return 0;
 return fractal_checksum64(text,(size_t)n);
}

uint64_t fractal_scheduler_work_unit_identity_v1(const fractal_sealed_work_unit_v1 *u){
 return identity_from_values(u);
}

uint64_t fractal_scheduler_work_unit_set_identity_v1(const fractal_sealed_work_unit_v1 *units,size_t count){
 uint64_t hash=UINT64_C(1469598103934665603);
 size_t i,j;
 if(!units||!count||count>FRACTAL_THREAD_POOL_MAX_WORKERS)return 0;
 for(i=0;i<count;i++){
  uint64_t value=units[i].identity;
  for(j=0;j<8u;j++){
   hash^=(unsigned char)(value>>(j*8u));
   hash*=UINT64_C(1099511628211);
  }
 }
 return hash;
}

fractal_result fractal_scheduler_decompose_contiguous_rows_v1(
 uint32_t width,uint32_t height,uint32_t workers,
 fractal_sealed_work_unit_v1 *units,size_t capacity,size_t *count){
 uint32_t i;
 if(!count||!width||!height||!workers||workers>FRACTAL_THREAD_POOL_MAX_WORKERS)
  return FRACTAL_ERROR_INVALID_SPEC;
 *count=workers;
 if(!units||capacity<workers)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 for(i=0;i<workers;i++){
  fractal_sealed_work_unit_v1 u={0};
  u.abi_version=1u;
  u.sequence=i;
  u.worker_count=workers;
  u.width=width;
  u.height=height;
  u.row_begin=(uint32_t)(((uint64_t)i*height)/workers);
  u.row_end=(uint32_t)(((uint64_t)(i+1u)*height)/workers);
  u.sample_begin=(uint64_t)u.row_begin*width;
  u.sample_end=(uint64_t)u.row_end*width;
  u.identity=identity_from_values(&u);
  units[i]=u;
 }
 return FRACTAL_OK;
}

fractal_result fractal_scheduler_decompose_computation_v1(
 const fractal_compute_vtable *compute,const fractal_computation_problem_v1 *problem,
 uint32_t workers,fractal_computation_cancellation_mode cancellation_mode,
 fractal_sealed_work_unit_v1 *units,size_t capacity,size_t *count){
 size_t i;fractal_result result;
 if(fractal_computation_problem_validate_v1(compute,problem)!=FRACTAL_OK||
    (cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION&&
     cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY))return FRACTAL_ERROR_INVALID_SPEC;
 result=fractal_scheduler_decompose_contiguous_rows_v1(problem->field.width,problem->field.height,
  workers,units,capacity,count);
 if(result!=FRACTAL_OK)return result;
 for(i=0;i<*count;i++){
  units[i].contract_version=FRACTAL_COMPUTATION_CONTRACT_VERSION;
  units[i].work_unit_identity=problem->identity;
  units[i].computation_identity=fractal_module_identity_v1(compute->descriptor);
  units[i].numeric_identity=fractal_module_identity_v1(problem->numeric->descriptor);
  units[i].formula_identity=fractal_module_identity_v1(problem->formula->descriptor);
  units[i].computation_version=compute->descriptor->module_version;
  units[i].numeric_version=problem->numeric->descriptor->module_version;
  units[i].formula_version=problem->formula->descriptor->module_version;
  units[i].field_format=(uint32_t)problem->field.format;
  units[i].cancellation_mode=(uint32_t)cancellation_mode;
  units[i].identity=identity_from_values(&units[i]);
 }
 return FRACTAL_OK;
}

fractal_result fractal_scheduler_validate_contiguous_rows_v1(
 const fractal_sealed_work_unit_v1 *units,size_t count,
 uint32_t width,uint32_t height,uint32_t workers){
 size_t i;
 uint64_t total=(uint64_t)width*height;
 if(!units||!count||count!=workers||!width||!height||!workers||
    workers>FRACTAL_THREAD_POOL_MAX_WORKERS)return FRACTAL_ERROR_INVALID_SPEC;
 for(i=0;i<count;i++){
  const fractal_sealed_work_unit_v1 *u=&units[i];
  uint32_t begin=(uint32_t)(((uint64_t)i*height)/workers);
  uint32_t end=(uint32_t)(((uint64_t)(i+1u)*height)/workers);
  if(u->abi_version!=1u||u->sequence!=i||u->worker_count!=workers||
     u->width!=width||u->height!=height||u->row_begin!=begin||u->row_end!=end||
     u->row_begin>u->row_end||u->row_end>height||
     u->sample_begin!=(uint64_t)begin*width||u->sample_end!=(uint64_t)end*width||
     u->sample_begin>u->sample_end||u->sample_end>total||
     u->identity!=identity_from_values(u))return FRACTAL_ERROR_INVALID_SPEC;
  if(i&&units[i-1u].row_end!=u->row_begin)return FRACTAL_ERROR_INVALID_SPEC;
 }
 return units[0].row_begin==0u&&units[count-1u].row_end==height
  ?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

fractal_result fractal_scheduler_validate_computation_v1(
 const fractal_compute_vtable *compute,const fractal_computation_problem_v1 *problem,
 const fractal_sealed_work_unit_v1 *units,size_t count,uint32_t workers){
 size_t i;
 if(fractal_computation_problem_validate_v1(compute,problem)!=FRACTAL_OK||
    fractal_scheduler_validate_contiguous_rows_v1(units,count,problem->field.width,
     problem->field.height,workers)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 for(i=0;i<count;i++)if(units[i].contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    units[i].work_unit_identity!=problem->identity||
    units[i].computation_identity!=fractal_module_identity_v1(compute->descriptor)||
    units[i].numeric_identity!=fractal_module_identity_v1(problem->numeric->descriptor)||
    units[i].formula_identity!=fractal_module_identity_v1(problem->formula->descriptor)||
    units[i].computation_version!=compute->descriptor->module_version||
    units[i].numeric_version!=problem->numeric->descriptor->module_version||
    units[i].formula_version!=problem->formula->descriptor->module_version||
    units[i].field_format!=(uint32_t)problem->field.format||
    (units[i].cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION&&
     units[i].cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY))return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

const char *fractal_scheduler_execution_status_string(fractal_scheduler_execution_status status){
 switch(status){
  case FRACTAL_SCHEDULER_EXECUTION_NOT_STARTED:return "not-started";
  case FRACTAL_SCHEDULER_EXECUTION_SUCCEEDED:return "succeeded";
  case FRACTAL_SCHEDULER_EXECUTION_CANCELLED:return "cancelled";
  case FRACTAL_SCHEDULER_EXECUTION_FAILED:return "failed";
 }
 return NULL;
}

const char *fractal_computation_execution_status_string(fractal_computation_execution_status status){
 switch(status){
  case FRACTAL_COMPUTATION_NOT_STARTED:return "not-started";
  case FRACTAL_COMPUTATION_SUCCEEDED:return "succeeded";
  case FRACTAL_COMPUTATION_CANCELLED:return "cancelled";
  case FRACTAL_COMPUTATION_FAILED:return "failed";
 }
 return NULL;
}

const char *fractal_publication_status_string(fractal_publication_status status){
 switch(status){
  case FRACTAL_PUBLICATION_NOT_APPLICABLE:return "not-applicable";
  case FRACTAL_PUBLICATION_ACTIVE:return "active";
  case FRACTAL_PUBLICATION_COMMITTED:return "committed";
  case FRACTAL_PUBLICATION_ABORTED:return "aborted";
  case FRACTAL_PUBLICATION_FAILED:return "failed";
 }
 return NULL;
}

static fractal_result serial_v1_execute(const fractal_runtime_modules *runtime,
 const fractal_job_spec *job,fractal_field *field,const fractal_cancellation *cancel,
 fractal_progress_fn progress,void *context){
 return fractal_scheduler_serial.execute(runtime,job,field,cancel,progress,context);
}

const fractal_scheduler_vtable fractal_scheduler_serial_v1={
 &serial_v1_descriptor,FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|FRACTAL_CAP_COMPUTE_CANCELLATION,FRACTAL_CAP_ITERATION_FIELD,serial_v1_execute
};

typedef struct worker_context {
 const fractal_compute_vtable *compute;
 const fractal_computation_problem_v1 *problem;
 fractal_mutable_field_view destination;
 const fractal_sealed_work_unit_v1 *unit;
 const fractal_cancellation *cancellation;
 atomic_uint *launch_gate;
 fractal_computation_result_v1 execution;
} worker_context;

static int worker_run(void *state){
 worker_context *worker=state;
 fractal_computation_request_v1 request;
 unsigned gate;
 do{
  gate=atomic_load_explicit(worker->launch_gate,memory_order_acquire);
  if(!gate){
#ifdef _WIN32
   (void)SwitchToThread();
#else
   (void)sched_yield();
#endif
  }
 }while(!gate);
 if(gate!=1u){worker->execution.result=FRACTAL_ERROR_NOT_IMPLEMENTED;worker->execution.status=FRACTAL_COMPUTATION_FAILED;return 0;}
 if(fractal_cancellation_is_requested(worker->cancellation)){
  worker->execution.result=FRACTAL_ERROR_CANCELLED;worker->execution.status=FRACTAL_COMPUTATION_CANCELLED;
  return 0;
 }
 request=(fractal_computation_request_v1){FRACTAL_COMPUTATION_ABI_VERSION,
  FRACTAL_COMPUTATION_CONTRACT_VERSION,worker->problem,worker->unit,
  worker->destination,worker->cancellation};
 (void)worker->compute->execute(&request,&worker->execution);
 return 0;
}

#ifdef _WIN32
typedef HANDLE host_thread;
static DWORD WINAPI worker_entry(LPVOID state){return (DWORD)worker_run(state);}
static bool host_thread_create(host_thread *thread,worker_context *worker){
 *thread=CreateThread(NULL,0,worker_entry,worker,0,NULL);
 return *thread!=NULL;
}
static void host_thread_join(host_thread thread){
 (void)WaitForSingleObject(thread,INFINITE);
 (void)CloseHandle(thread);
}
#else
typedef pthread_t host_thread;
static void *worker_entry(void *state){(void)worker_run(state);return NULL;}
static bool host_thread_create(host_thread *thread,worker_context *worker){
 return pthread_create(thread,NULL,worker_entry,worker)==0;
}
static void host_thread_join(host_thread thread){(void)pthread_join(thread,NULL);}
#endif

static fractal_result thread_pool_execute(const fractal_runtime_modules *runtime,
 const fractal_job_spec *job,fractal_field *field,const fractal_cancellation *cancel,
 fractal_progress_fn progress,void *progress_context){
 fractal_sealed_work_unit_v1 units[FRACTAL_THREAD_POOL_MAX_WORKERS];
 worker_context workers[FRACTAL_THREAD_POOL_MAX_WORKERS];
 host_thread threads[FRACTAL_THREAD_POOL_MAX_WORKERS];
 atomic_uint launch_gate=ATOMIC_VAR_INIT(0u);
 uint32_t worker_count,i,created=0;
 size_t unit_count=0;
 fractal_result result=FRACTAL_OK;
 fractal_computation_problem_v1 problem;
 fractal_field_descriptor descriptor;
 fractal_mutable_field_view destination;
 if(!runtime||!job||!field||!field->samples)return FRACTAL_ERROR_INVALID_ARGUMENT;
 worker_count=runtime->scheduler_options.requested_worker_count;
 if(!worker_count||worker_count>FRACTAL_THREAD_POOL_MAX_WORKERS)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(fractal_cancellation_is_requested(cancel))return FRACTAL_ERROR_CANCELLED;
 if(field->width!=job->view.width||field->height!=job->view.height||
    field->stride<(size_t)field->width*sizeof(*field->samples)||
    (field->height&&field->stride>SIZE_MAX/field->height))return FRACTAL_ERROR_INVALID_SPEC;
 descriptor=(fractal_field_descriptor){field->width,field->height,field->stride,
  FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
 destination=(fractal_mutable_field_view){descriptor,field->samples,field->stride*field->height};
 if(fractal_computation_problem_init_v1(runtime,job,&descriptor,&problem)!=FRACTAL_OK||
    fractal_scheduler_decompose_computation_v1(runtime->compute,&problem,worker_count,
    FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY,units,FRACTAL_THREAD_POOL_MAX_WORKERS,&unit_count)!=FRACTAL_OK||
    fractal_scheduler_validate_computation_v1(runtime->compute,&problem,units,unit_count,
     worker_count)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 if(fractal_cancellation_is_requested(cancel))return FRACTAL_ERROR_CANCELLED;
 memset(workers,0,sizeof(workers));
 for(i=0;i<worker_count;i++){
  workers[i]=(worker_context){runtime->compute,&problem,destination,&units[i],cancel,&launch_gate,{0}};
  if(!host_thread_create(&threads[i],&workers[i])){
   result=FRACTAL_ERROR_OUT_OF_MEMORY;
   break;
  }
  created++;
 }
 if(created!=worker_count){
  atomic_store_explicit(&launch_gate,2u,memory_order_release);
  for(i=0;i<created;i++)host_thread_join(threads[i]);
  return result;
 }
 atomic_store_explicit(&launch_gate,1u,memory_order_release);
 for(i=0;i<worker_count;i++)host_thread_join(threads[i]);
 for(i=0;i<worker_count;i++)if(workers[i].execution.result!=FRACTAL_OK){result=workers[i].execution.result;break;}
 if(result==FRACTAL_OK&&fractal_cancellation_is_requested(cancel))result=FRACTAL_ERROR_CANCELLED;
 if(result!=FRACTAL_OK)return result;
 field->completed_rows=field->height;
 field->complete=true;
 if(progress)for(i=1;i<=field->height;i++)progress(i,field->height,progress_context);
 return FRACTAL_OK;
}

const fractal_scheduler_vtable fractal_scheduler_thread_pool_v1={
 &thread_pool_v1_descriptor,FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|FRACTAL_CAP_COMPUTE_CANCELLATION,FRACTAL_CAP_ITERATION_FIELD,
 thread_pool_execute
};
