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
 char text[256];
 int n;
 if(!u)return 0;
 n=snprintf(text,sizeof(text),
  "%s|abi=%u|sequence=%u|workers=%u|width=%u|height=%u|rows=%u:%u|samples=%llu:%llu",
  FRACTAL_SCHEDULER_WORK_UNIT_V1_ID,u->abi_version,u->sequence,u->worker_count,
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

const char *fractal_scheduler_execution_status_string(fractal_scheduler_execution_status status){
 switch(status){
  case FRACTAL_SCHEDULER_EXECUTION_NOT_STARTED:return "not-started";
  case FRACTAL_SCHEDULER_EXECUTION_SUCCEEDED:return "succeeded";
  case FRACTAL_SCHEDULER_EXECUTION_CANCELLED:return "cancelled";
  case FRACTAL_SCHEDULER_EXECUTION_FAILED:return "failed";
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
 &serial_v1_descriptor,FRACTAL_CAP_POINT_SCALAR,FRACTAL_CAP_ITERATION_FIELD,serial_v1_execute
};

typedef struct worker_context {
 const fractal_runtime_modules *runtime;
 const fractal_job_spec *job;
 fractal_field *field;
 const fractal_cancellation *cancellation;
 const fractal_sealed_work_unit_v1 *unit;
 atomic_uint *launch_gate;
 fractal_result result;
} worker_context;

static int worker_run(void *state){
 worker_context *worker=state;
 const fractal_job_spec *job=worker->job;
 double aspect=(double)job->view.width/(double)job->view.height;
 uint32_t y,x;
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
 if(gate!=1u){worker->result=FRACTAL_ERROR_NOT_IMPLEMENTED;return 0;}
 if(fractal_cancellation_is_requested(worker->cancellation)){
  worker->result=FRACTAL_ERROR_CANCELLED;
  return 0;
 }
 for(y=worker->unit->row_begin;y<worker->unit->row_end;y++){
  fractal_point_result_compact *row;
  if(fractal_cancellation_is_requested(worker->cancellation)){
   worker->result=FRACTAL_ERROR_CANCELLED;
   return 0;
  }
  row=(fractal_point_result_compact *)((unsigned char *)worker->field->samples+
   (size_t)y*worker->field->stride);
  for(x=0;x<job->view.width;x++){
   double re=job->view.center_real+(((double)x+0.5)/(double)job->view.width-0.5)*job->view.scale*aspect;
   double im=job->view.center_imaginary+(0.5-((double)y+0.5)/(double)job->view.height)*job->view.scale;
   fractal_result q=worker->runtime->compute->point(worker->runtime->formula,
    worker->runtime->numeric,&job->problem.parameters,re,im,job->problem.maximum_steps,
    NULL,&row[x]);
   if(q!=FRACTAL_OK){worker->result=q;return 0;}
  }
 }
 worker->result=FRACTAL_OK;
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
 if(!runtime||!job||!field||!field->samples)return FRACTAL_ERROR_INVALID_ARGUMENT;
 worker_count=runtime->scheduler_options.requested_worker_count;
 if(!worker_count||worker_count>FRACTAL_THREAD_POOL_MAX_WORKERS)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(fractal_cancellation_is_requested(cancel))return FRACTAL_ERROR_CANCELLED;
 if(field->width!=job->view.width||field->height!=job->view.height||
    field->stride<(size_t)field->width*sizeof(*field->samples)||
    (field->height&&field->stride>SIZE_MAX/field->height))return FRACTAL_ERROR_INVALID_SPEC;
 if(fractal_scheduler_decompose_contiguous_rows_v1(field->width,field->height,
    worker_count,units,FRACTAL_THREAD_POOL_MAX_WORKERS,&unit_count)!=FRACTAL_OK||
    fractal_scheduler_validate_contiguous_rows_v1(units,unit_count,field->width,
     field->height,worker_count)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 if(fractal_cancellation_is_requested(cancel))return FRACTAL_ERROR_CANCELLED;
 memset(workers,0,sizeof(workers));
 for(i=0;i<worker_count;i++){
  workers[i]=(worker_context){runtime,job,field,cancel,&units[i],&launch_gate,FRACTAL_OK};
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
 for(i=0;i<worker_count;i++)if(workers[i].result!=FRACTAL_OK){result=workers[i].result;break;}
 if(result==FRACTAL_OK&&fractal_cancellation_is_requested(cancel))result=FRACTAL_ERROR_CANCELLED;
 if(result!=FRACTAL_OK)return result;
 field->completed_rows=field->height;
 field->complete=true;
 if(progress)for(i=1;i<=field->height;i++)progress(i,field->height,progress_context);
 return FRACTAL_OK;
}

const fractal_scheduler_vtable fractal_scheduler_thread_pool_v1={
 &thread_pool_v1_descriptor,FRACTAL_CAP_POINT_SCALAR,FRACTAL_CAP_ITERATION_FIELD,
 thread_pool_execute
};
