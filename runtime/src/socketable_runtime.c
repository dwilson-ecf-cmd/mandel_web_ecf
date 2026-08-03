#include "fractal/socketable_runtime.h"
#include <stdio.h>
#include <string.h>

bool fractal_cancellation_is_requested(const fractal_cancellation*c){
 return c?atomic_load(&c->requested):false;
}
void fractal_cancellation_request(fractal_cancellation*c){if(c)atomic_store(&c->requested,true);}
void fractal_cancellation_reset(fractal_cancellation*c){if(c)atomic_store(&c->requested,false);}

#define DESC(name,id,title,kind,caps) static const fractal_module_descriptor name={1u,1u,id,title,kind,caps,true}
DESC(d_none,"refinement.none","No refinement",FRACTAL_MODULE_REFINEMENT,0);
static const fractal_module_descriptor d_cdc={1u,1u,"refinement.cdc.unavailable","CDC refinement (unavailable)",FRACTAL_MODULE_REFINEMENT,0,false};
DESC(d_sched,"scheduler.serial.row-major","Serial row-major",FRACTAL_MODULE_SCHEDULER,FRACTAL_CAP_SERIAL_FIELD|FRACTAL_CAP_ITERATION_FIELD);
DESC(d_raster,"raster.native.iteration-bgr8","Native iteration rasterizer",FRACTAL_MODULE_RASTER,FRACTAL_CAP_ITERATION_FIELD|FRACTAL_CAP_BGR8);
DESC(d_bmp,"encoder.bmp.v3","BMP v3 encoder",FRACTAL_MODULE_ENCODER,FRACTAL_CAP_BGR8|FRACTAL_CAP_ENCODER_STREAM_OUTPUT);
DESC(d_mem,"memory.system.scoped","System scoped memory",FRACTAL_MODULE_MEMORY,FRACTAL_CAP_MEMORY_SCOPES);
static const fractal_module_descriptor d_ouro={1u,1u,"memory.ouro.unavailable","Ouro memory (unavailable)",FRACTAL_MODULE_MEMORY,0,false};
DESC(d_tel,"telemetry.noop","No-op telemetry",FRACTAL_MODULE_TELEMETRY,0);
DESC(d_host,"platform.host.c11","Host C11 platform",FRACTAL_MODULE_PLATFORM,0);

uint64_t fractal_checksum64(const void*d,size_t n){const unsigned char*p=d;uint64_t h=UINT64_C(1469598103934665603);while(n--){h^=*p++;h*=UINT64_C(1099511628211);}return h;}

uint64_t fractal_module_identity_v1(const fractal_module_descriptor*d){
 char text[512];int n;
 if(!d||fractal_module_descriptor_validate(d,d->kind)!=FRACTAL_OK)return 0;
 n=snprintf(text,sizeof(text),"module|abi=%u|version=%u|kind=%u|id=%s|caps=%016llx|available=%u",
  d->abi_version,d->module_version,(unsigned)d->kind,d->module_id,
  (unsigned long long)d->capability_flags,d->available?1u:0u);
 return n<0||(size_t)n>=sizeof(text)?0:fractal_checksum64(text,(size_t)n);
}

const fractal_refinement_vtable fractal_refinement_none={&d_none,true},fractal_refinement_cdc_unavailable={&d_cdc,false};

static fractal_result execute(const fractal_runtime_modules*r,const fractal_job_spec*j,fractal_field*f,const fractal_cancellation*c,fractal_progress_fn progress,void*ctx){fractal_field_descriptor d;fractal_mutable_field_view destination;fractal_computation_problem_v1 problem;fractal_sealed_work_unit_v1 assignment;fractal_computation_request_v1 request;fractal_computation_result_v1 result;size_t count=0;uint32_t y;if(!r||!j||!f||!f->samples)return FRACTAL_ERROR_INVALID_ARGUMENT;if(fractal_cancellation_is_requested(c))return FRACTAL_ERROR_CANCELLED;if(f->width!=j->view.width||f->height!=j->view.height||f->stride<(size_t)f->width*sizeof(*f->samples)||(f->height&&f->stride>SIZE_MAX/f->height))return FRACTAL_ERROR_INVALID_SPEC;d=(fractal_field_descriptor){f->width,f->height,f->stride,FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};destination=(fractal_mutable_field_view){d,f->samples,f->stride*f->height};if(fractal_computation_problem_init_v1(r,j,&d,&problem)!=FRACTAL_OK||fractal_scheduler_decompose_computation_v1(r->compute,&problem,1,FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,&assignment,1,&count)!=FRACTAL_OK||count!=1)return FRACTAL_ERROR_INVALID_SPEC;request=(fractal_computation_request_v1){FRACTAL_COMPUTATION_ABI_VERSION,FRACTAL_COMPUTATION_CONTRACT_VERSION,&problem,&assignment,destination,c};if(r->compute->execute(&request,&result)!=FRACTAL_OK)return result.result;f->completed_rows=f->height;f->complete=true;if(progress)for(y=1;y<=f->height;y++)progress(y,f->height,ctx);return FRACTAL_OK;}
const fractal_scheduler_vtable fractal_scheduler_serial={&d_sched,FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|FRACTAL_CAP_COMPUTE_CANCELLATION,FRACTAL_CAP_ITERATION_FIELD,execute};
static fractal_result rasterize(const fractal_field_view*f,uint32_t max,fractal_pixel_buffer*p){uint32_t y,x;if(!f||!p||!p->pixels||!max||fractal_field_view_validate(f,sizeof(fractal_point_result_compact))!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_ARGUMENT;for(y=0;y<f->descriptor.height;y++)for(x=0;x<f->descriptor.width;x++){const fractal_point_result_compact*s=(const fractal_point_result_compact*)((const unsigned char*)f->data+(size_t)y*f->descriptor.stride)+(size_t)x;unsigned char*v=&p->pixels[(size_t)y*p->stride+(size_t)x*3u];if(s->classification==FRACTAL_CLASS_ESCAPED){v[0]=(unsigned char)((s->steps*29u)&255u);v[1]=(unsigned char)((s->steps*13u)&255u);v[2]=(unsigned char)(255u-(s->steps*7u&255u));}else v[0]=v[1]=v[2]=0; }return FRACTAL_OK;}
const fractal_raster_vtable fractal_raster_native={&d_raster,FRACTAL_CAP_ITERATION_FIELD,FRACTAL_CAP_BGR8,rasterize};
static void le32(unsigned char*b,uint32_t v){b[0]=(unsigned char)v;b[1]=(unsigned char)(v>>8);b[2]=(unsigned char)(v>>16);b[3]=(unsigned char)(v>>24);} static void le16(unsigned char*b,uint16_t v){b[0]=(unsigned char)v;b[1]=(unsigned char)(v>>8);}
static fractal_result encode(const fractal_pixel_buffer*p,fractal_write_sink*s){unsigned char h[54]={0},pad[3]={0};uint32_t y,row,file;fractal_result q;if(!p||!s||!s->write||p->format!=FRACTAL_PIXEL_BGR8||!p->pixels||p->width>(UINT32_MAX-3u)/3u)return FRACTAL_ERROR_INVALID_ARGUMENT;row=(p->width*3u+3u)&~3u;if(p->height>(UINT32_MAX-54u)/row)return FRACTAL_ERROR_INVALID_SPEC;file=54u+row*p->height;h[0]='B';h[1]='M';le32(h+2,file);le32(h+10,54);le32(h+14,40);le32(h+18,p->width);le32(h+22,p->height);le16(h+26,1);le16(h+28,24);le32(h+34,row*p->height);if((q=s->write(s->context,h,54))!=FRACTAL_OK)return q;for(y=p->height;y>0;y--){if((q=s->write(s->context,p->pixels+(size_t)(y-1)*p->stride,p->width*3u))!=FRACTAL_OK)return q;if(row>p->width*3u&&(q=s->write(s->context,pad,row-p->width*3u))!=FRACTAL_OK)return q;}return FRACTAL_OK;}
const fractal_encoder_vtable fractal_encoder_bmp={&d_bmp,FRACTAL_CAP_BGR8,encode};
const fractal_memory_vtable fractal_memory_system_module={&d_mem},fractal_memory_ouro_unavailable_module={&d_ouro};
static void noop(void*s,const fractal_telemetry_event*e){(void)s;(void)e;} const fractal_telemetry_vtable fractal_telemetry_noop={&d_tel,noop}; const fractal_platform_vtable fractal_platform_host={&d_host};

fractal_result fractal_runtime_validate(const fractal_runtime_modules*r){
 const fractal_module_descriptor*d[10];
 uint64_t required_numeric;
 unsigned i;
 if(!r||!r->formula||!r->numeric||!r->compute||!r->refinement||!r->scheduler||
    !r->raster||!r->encoder||!r->memory_module||!r->memory||!r->telemetry||
    !r->platform)return FRACTAL_ERROR_INVALID_ARGUMENT;
 d[0]=r->formula->descriptor;d[1]=r->numeric->descriptor;d[2]=r->compute->descriptor;
 d[3]=r->refinement->descriptor;d[4]=r->scheduler->descriptor;d[5]=r->raster->descriptor;
 d[6]=r->encoder->descriptor;d[7]=r->memory_module->descriptor;d[8]=r->telemetry->descriptor;
 d[9]=r->platform->descriptor;
 for(i=0;i<10;i++)if(fractal_module_descriptor_validate(d[i],(fractal_module_kind)i)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(r->compute->contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    !r->compute->execute||!r->compute->point)return FRACTAL_ERROR_INVALID_SPEC;
 required_numeric=r->formula->required_numeric_capabilities|
  r->compute->required_numeric_capabilities;
 if(fractal_numeric_validate_v1(r->numeric,required_numeric)!=FRACTAL_OK||
    fractal_formula_validate_v1(r->formula,r->numeric)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(!strcmp(r->scheduler->descriptor->module_id,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)&&
    (!r->scheduler_options.requested_worker_count||
     r->scheduler_options.requested_worker_count>FRACTAL_THREAD_POOL_MAX_WORKERS))
  return FRACTAL_ERROR_INVALID_SPEC;
 if(r->analysis.count>FRACTAL_ANALYZER_CHAIN_MAX)return FRACTAL_ERROR_INVALID_SPEC;
 for(i=0;i<r->analysis.count;i++)if(!r->analysis.analyzers[i]||
    fractal_module_descriptor_validate(r->analysis.analyzers[i]->descriptor,
     FRACTAL_MODULE_ANALYZER)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 if(!r->refinement->available||r->memory_module==&fractal_memory_ouro_unavailable_module||
    r->memory->kind!=FRACTAL_MEMORY_BACKEND_SYSTEM)return FRACTAL_ERROR_NOT_IMPLEMENTED;
 if((r->formula->descriptor->capability_flags&r->compute->required_formula_capabilities)!=
     r->compute->required_formula_capabilities||
    (r->compute->descriptor->capability_flags&r->scheduler->required_compute_capabilities)!=
     r->scheduler->required_compute_capabilities||
    (r->compute->descriptor->capability_flags&r->compute->required_field_capabilities)!=
     r->compute->required_field_capabilities||
    (r->scheduler->produced_capabilities&r->raster->accepted_field_capabilities)!=
     r->raster->accepted_field_capabilities||
    (r->raster->produced_capabilities&r->encoder->accepted_pixel_capabilities)!=
     r->encoder->accepted_pixel_capabilities)return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_job_spec_validate(const fractal_runtime_modules*r,
 const fractal_job_spec*j){
 fractal_scalar value,zero;
 fractal_numeric_order_v1 order;
 if(fractal_runtime_validate(r)!=FRACTAL_OK||!j||!j->problem.formula_id||
    !fractal_formula_id_matches_v1(r->formula,j->problem.formula_id)||
    !j->artifact.encoder_id||strcmp(j->artifact.encoder_id,r->encoder->descriptor->module_id)||
    !j->raster.palette_id||strcmp(j->raster.palette_id,"palette.socket-v1")||
    j->raster.pixel_format!=FRACTAL_PIXEL_BGR8||!j->view.width||!j->view.height||
    j->view.width>4096||j->view.height>4096||!j->problem.maximum_steps)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(r->numeric->from_binary64(j->view.center_real,&value)!=FRACTAL_OK||
    r->numeric->from_binary64(j->view.center_imaginary,&value)!=FRACTAL_OK||
    r->numeric->from_binary64(j->view.scale,&value)!=FRACTAL_OK||
    r->numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    r->numeric->real_compare(&value,&zero,&order)!=FRACTAL_OK||
    order!=FRACTAL_NUMERIC_ORDER_GREATER)return FRACTAL_ERROR_INVALID_SPEC;
 if(r->analysis.count){
  fractal_field_descriptor d={j->view.width,j->view.height,
   (size_t)j->view.width*sizeof(fractal_point_result_compact),
   FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
  if(fractal_analysis_pipeline_validate(&r->analysis,&d,FRACTAL_ANALYSIS_MAX_RECORDS)!=
     FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 }
 return fractal_formula_parameters_validate_v1(r->formula,r->numeric,
  &j->problem.parameters);
}
typedef struct counting_sink {fractal_write_sink *next;uint64_t hash;size_t bytes;} counting_sink;static fractal_result count_write(void*c,const void*d,size_t n){counting_sink*x=c;const unsigned char*p=d;x->bytes+=n;while(n--){x->hash^=*p++;x->hash*=UINT64_C(1099511628211);}return x->next->write(x->next->context,d,(size_t)(p-(const unsigned char*)d));}
static void scheduler_execution_prepare(const fractal_runtime_modules*r,const fractal_job_spec*j,fractal_scheduler_execution*e){
 fractal_sealed_work_unit_v1 units[FRACTAL_THREAD_POOL_MAX_WORKERS];fractal_computation_problem_v1 problem;fractal_field_descriptor d;size_t count=0;uint32_t workers;
 memset(e,0,sizeof(*e));e->decomposition_version=FRACTAL_SCHEDULER_DECOMPOSITION_VERSION;e->status=FRACTAL_SCHEDULER_EXECUTION_NOT_STARTED;e->result=FRACTAL_OK;
 workers=!strcmp(r->scheduler->descriptor->module_id,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)?r->scheduler_options.requested_worker_count:1u;
 e->requested_worker_count=!strcmp(r->scheduler->descriptor->module_id,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)?r->scheduler_options.requested_worker_count:(r->scheduler_options.requested_worker_count?r->scheduler_options.requested_worker_count:1u);
 e->effective_worker_count=workers;e->assignment_count=workers;
 d=(fractal_field_descriptor){j->view.width,j->view.height,(size_t)j->view.width*sizeof(fractal_point_result_compact),FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
 if(fractal_computation_problem_init_v1(r,j,&d,&problem)==FRACTAL_OK&&fractal_scheduler_decompose_computation_v1(r->compute,&problem,workers,!strcmp(r->scheduler->descriptor->module_id,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)?FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY:FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,units,FRACTAL_THREAD_POOL_MAX_WORKERS,&count)==FRACTAL_OK){e->computation_identity=problem.identity;e->sealed_work_unit_identity=fractal_scheduler_work_unit_set_identity_v1(units,count);}
}
static void scheduler_execution_finish(fractal_scheduler_execution*e,fractal_result q){e->result=q;e->status=q==FRACTAL_OK?FRACTAL_SCHEDULER_EXECUTION_SUCCEEDED:q==FRACTAL_ERROR_CANCELLED?FRACTAL_SCHEDULER_EXECUTION_CANCELLED:FRACTAL_SCHEDULER_EXECUTION_FAILED;}
fractal_result fractal_runtime_render(const fractal_runtime_modules*r,const fractal_job_spec*j,fractal_write_sink*s,const fractal_cancellation*c,fractal_runtime_output*o){fractal_memory_scope scope;counting_sink cs;fractal_write_sink wrapped;fractal_result q;fractal_field_view source;if(!o||!s||fractal_job_spec_validate(r,j)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;memset(o,0,sizeof(*o));o->publication_status=FRACTAL_PUBLICATION_NOT_APPLICABLE;scheduler_execution_prepare(r,j,&o->scheduler_execution);if(fractal_memory_scope_begin(r->memory,FRACTAL_MEMORY_SCOPE_SHORT,&scope)!=FRACTAL_OK){o->pipeline_result=FRACTAL_ERROR_OUT_OF_MEMORY;return FRACTAL_ERROR_OUT_OF_MEMORY;}o->field.width=j->view.width;o->field.height=j->view.height;o->field.stride=(size_t)j->view.width*sizeof(*o->field.samples);o->field.samples=fractal_memory_scope_alloc(&scope,o->field.stride*j->view.height,_Alignof(fractal_point_result_compact));o->pixels.width=j->view.width;o->pixels.height=j->view.height;o->pixels.stride=(size_t)j->view.width*3u;o->pixels.format=FRACTAL_PIXEL_BGR8;o->pixels.pixels=fractal_memory_scope_alloc(&scope,o->pixels.stride*j->view.height,1u);if(!o->field.samples||!o->pixels.pixels){fractal_memory_scope_end(&scope);o->pipeline_result=FRACTAL_ERROR_OUT_OF_MEMORY;return FRACTAL_ERROR_OUT_OF_MEMORY;}q=r->scheduler->execute(r,j,&o->field,c,NULL,NULL);scheduler_execution_finish(&o->scheduler_execution,q);source=(fractal_field_view){{o->field.width,o->field.height,o->field.stride,FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0},o->field.samples,o->field.stride*o->field.height};o->analyzer_count=r->analysis.count;o->analysis_pipeline_identity=r->analysis.identity;if(q==FRACTAL_OK){o->source_field_checksum=o->field_checksum=fractal_checksum64(source.data,source.data_size);if(fractal_cancellation_is_requested(c))q=FRACTAL_ERROR_CANCELLED;}if(q==FRACTAL_OK)q=fractal_analysis_pipeline_run(&r->analysis,&source,c,&o->analysis_result,FRACTAL_ANALYSIS_MAX_RECORDS);if(q==FRACTAL_OK){o->analyzed_field_checksum=fractal_checksum64(source.data,source.data_size);q=r->raster->rasterize(&source,j->problem.maximum_steps,&o->pixels);}if(q==FRACTAL_OK)o->pixel_checksum=fractal_checksum64(o->pixels.pixels,o->pixels.stride*o->pixels.height);cs=(counting_sink){s,UINT64_C(1469598103934665603),0};wrapped=(fractal_write_sink){&cs,count_write};if(q==FRACTAL_OK)q=r->encoder->encode(&o->pixels,&wrapped);o->artifact_checksum=cs.hash;o->artifact_bytes=cs.bytes;o->pipeline_result=q;fractal_memory_scope_end(&scope);o->field.samples=NULL;o->pixels.pixels=NULL;return q;}
static fractal_result artifact_write(void*c,const void*d,size_t n){return fractal_artifact_sink_write((fractal_artifact_sink*)c,d,n);}
fractal_result fractal_runtime_render_artifact(const fractal_runtime_modules*r,const fractal_job_spec*j,fractal_artifact_sink*s,const fractal_cancellation*c,fractal_runtime_output*o,fractal_artifact_result*a){fractal_artifact_begin_info begin;fractal_write_sink writer;fractal_result q;if(!s||!s->vtable||!a||!j||!o)return FRACTAL_ERROR_INVALID_ARGUMENT;memset(a,0,sizeof(*a));memset(o,0,sizeof(*o));begin=(fractal_artifact_begin_info){j->artifact.output_name,"image/bmp",0};q=fractal_artifact_sink_begin(s,&begin);if(q!=FRACTAL_OK){o->publication_status=FRACTAL_PUBLICATION_FAILED;o->pipeline_result=q;return q;}o->publication_status=FRACTAL_PUBLICATION_ACTIVE;writer=(fractal_write_sink){s,artifact_write};q=fractal_runtime_render(r,j,&writer,c,o);if(q!=FRACTAL_OK){a->result=q;fractal_artifact_sink_abort(s);o->publication_status=FRACTAL_PUBLICATION_ABORTED;return q;}o->publication_status=FRACTAL_PUBLICATION_ACTIVE;q=fractal_artifact_sink_commit(s,a);if(q!=FRACTAL_OK){a->committed=false;fractal_artifact_sink_abort(s);o->publication_status=FRACTAL_PUBLICATION_ABORTED;o->pipeline_result=q;return q;}a->encoder_module_id=r->encoder->descriptor->module_id;a->pixel_checksum=o->pixel_checksum;strncpy(a->logical_name,j->artifact.output_name,sizeof(a->logical_name)-1u);strcpy(a->media_type,"image/bmp");if(a->byte_count!=o->artifact_bytes||a->encoded_checksum!=o->artifact_checksum){a->committed=false;a->result=FRACTAL_ERROR_IO;o->publication_status=FRACTAL_PUBLICATION_FAILED;o->pipeline_result=FRACTAL_ERROR_IO;return FRACTAL_ERROR_IO;}o->publication_status=FRACTAL_PUBLICATION_COMMITTED;return FRACTAL_OK;}
static const char *scheduler_decomposition_id(const fractal_runtime_modules*r){return !strcmp(r->scheduler->descriptor->module_id,FRACTAL_SCHEDULER_THREAD_POOL_V1_ID)?FRACTAL_SCHEDULER_THREAD_POOL_DECOMPOSITION_V1_ID:FRACTAL_SCHEDULER_SERIAL_DECOMPOSITION_V1_ID;}
fractal_result fractal_runtime_manifest(const fractal_runtime_modules*r,
 const fractal_job_spec*j,const fractal_runtime_output*o,char*b,size_t c,size_t*l){
 char params[192];size_t pl;int n;
 fractal_escape_classification_summary_v1 v={0};
 const char*schema="";uint32_t schema_version=0;uint64_t record_identity=0;
 uint64_t numeric_identity,formula_identity,parameter_identity;
 if(!l||!o||fractal_job_spec_validate(r,j)!=FRACTAL_OK||
    r->formula->serialize_parameters(r->numeric,&j->problem.parameters,params,
     sizeof(params),&pl)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 numeric_identity=fractal_numeric_execution_identity_v1(r->numeric);
 formula_identity=fractal_formula_execution_identity_v1(r->formula);
 if(!numeric_identity||!formula_identity||
    fractal_formula_parameter_identity_v1(r->formula,r->numeric,
     &j->problem.parameters,&parameter_identity)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(o->analysis_result.records_produced==1&&
    fractal_escape_summary_record_decode(&o->analysis_result.records[0],&v)==FRACTAL_OK){
  schema=FRACTAL_ESCAPE_SUMMARY_SCHEMA_ID;schema_version=FRACTAL_ESCAPE_SUMMARY_SCHEMA_VERSION;
  record_identity=o->analysis_result.records[0].identity;
 }else if(o->analysis_result.records_produced==1&&
    o->analysis_result.records[0].type_id==FRACTAL_ANALYSIS_RECORD_ITERATION_HISTOGRAM){
  schema=FRACTAL_ITERATION_HISTOGRAM_SCHEMA_ID;schema_version=FRACTAL_ITERATION_HISTOGRAM_SCHEMA_VERSION;
  record_identity=o->analysis_result.records[0].identity;
 }else if(o->analysis_result.records_produced==1&&
    o->analysis_result.records[0].type_id==FRACTAL_ANALYSIS_RECORD_SPATIAL_WORKLOAD_GRID){
  schema=FRACTAL_SPATIAL_WORKLOAD_SCHEMA_ID;schema_version=FRACTAL_SPATIAL_WORKLOAD_SCHEMA_VERSION;
  record_identity=o->analysis_result.records[0].identity;
 }
 n=snprintf(b,c,"{\"abi\":1,\"analysis_pipeline_identity\":\"%016llx\",\"analysis_record_count\":%zu,\"analysis_record_identity\":\"%016llx\",\"analysis_result\":%u,\"analysis_samples_examined\":%llu,\"analyzed_field_checksum\":\"%016llx\",\"analyzer\":\"%s\",\"analyzer_count\":%zu,\"analyzer_version\":%u,\"artifact_checksum\":\"%016llx\",\"computation\":\"%s\",\"computation_identity\":\"%016llx\",\"computation_status\":\"%s\",\"computation_version\":%u,\"assignment_count\":%u,\"bounded_samples\":%llu,\"cancelled_samples\":%llu,\"decomposition\":\"%s\",\"decomposition_version\":%u,\"effective_worker_count\":%u,\"escaped_samples\":%llu,\"execution_result\":%u,\"execution_status\":\"%s\",\"failed_samples\":%llu,\"field_checksum\":\"%016llx\",\"field_preserved\":%s,\"formula\":\"%s\",\"formula_compatibility\":\"compatible\",\"formula_contract_version\":%u,\"formula_execution_identity\":\"%016llx\",\"formula_interface_version\":%u,\"formula_parameter_identity\":\"%016llx\",\"formula_version\":%u,\"formula_parameters\":%s,\"iteration_samples\":%llu,\"iteration_statistics_valid\":%s,\"maximum_iteration_samples\":%llu,\"numeric\":\"%s\",\"numeric_abi_version\":%u,\"numeric_compatibility\":\"compatible\",\"numeric_execution_identity\":\"%016llx\",\"numeric_version\":%u,\"pixel_checksum\":\"%016llx\",\"publication_status\":\"%s\",\"record_schema\":\"%s\",\"record_schema_version\":%u,\"requested_worker_count\":%u,\"scheduler\":\"%s\",\"scheduler_version\":%u,\"sealed_work_unit\":\"%s\",\"sealed_work_unit_identity\":\"%016llx\",\"source_field_checksum\":\"%016llx\",\"unresolved_samples\":%llu}",
  (unsigned long long)o->analysis_pipeline_identity,o->analysis_result.records_produced,
  (unsigned long long)record_identity,(unsigned)o->analysis_result.result,
  (unsigned long long)o->analysis_result.samples_examined,
  (unsigned long long)o->analyzed_field_checksum,
  r->analysis.count?r->analysis.requests[0].analyzer_id:"",o->analyzer_count,
  o->analysis_result.analyzer_version,(unsigned long long)o->artifact_checksum,
  r->compute->descriptor->module_id,(unsigned long long)o->scheduler_execution.computation_identity,
  fractal_scheduler_execution_status_string(o->scheduler_execution.status),
  r->compute->descriptor->module_version,o->scheduler_execution.assignment_count,
  (unsigned long long)v.bounded_samples,(unsigned long long)v.cancelled_samples,
  scheduler_decomposition_id(r),o->scheduler_execution.decomposition_version,
  o->scheduler_execution.effective_worker_count,(unsigned long long)v.escaped_samples,
  (unsigned)o->scheduler_execution.result,
  fractal_scheduler_execution_status_string(o->scheduler_execution.status),
  (unsigned long long)v.failed_samples,(unsigned long long)o->field_checksum,
  o->analyzer_count?(o->analysis_result.field_preserved?"true":"false"):"true",
  r->formula->descriptor->module_id,r->formula->contract_version,
  (unsigned long long)formula_identity,r->formula->interface_version,
  (unsigned long long)parameter_identity,r->formula->descriptor->module_version,params,
  (unsigned long long)v.iteration_samples,
  v.iteration_statistics_valid?"true":"false",
  (unsigned long long)v.maximum_iteration_samples,r->numeric->descriptor->module_id,
  r->numeric->abi_version,(unsigned long long)numeric_identity,
  r->numeric->descriptor->module_version,(unsigned long long)o->pixel_checksum,
  fractal_publication_status_string(o->publication_status),schema,schema_version,
  o->scheduler_execution.requested_worker_count,r->scheduler->descriptor->module_id,
  r->scheduler->descriptor->module_version,FRACTAL_SCHEDULER_WORK_UNIT_V1_ID,
  (unsigned long long)o->scheduler_execution.sealed_work_unit_identity,
  (unsigned long long)o->source_field_checksum,(unsigned long long)v.unresolved_samples);
 *l=n<0?0u:(size_t)n;
 return n<0||!b||c<=(size_t)n?FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

fractal_result fractal_runtime_artifact_manifest(const fractal_module_registry*g,
 const fractal_runtime_modules*r,const fractal_job_spec*j,const fractal_runtime_output*o,
 const fractal_artifact_result*a,char*b,size_t c,size_t*l){
 int n;const char*schema="";uint32_t sv=0;
 uint64_t ri=0,numeric_identity,formula_identity,parameter_identity;
 if(!g||!r||!j||!o||!a||!l||!a->committed||fractal_job_spec_validate(r,j)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 numeric_identity=fractal_numeric_execution_identity_v1(r->numeric);
 formula_identity=fractal_formula_execution_identity_v1(r->formula);
 if(!numeric_identity||!formula_identity||
    fractal_formula_parameter_identity_v1(r->formula,r->numeric,
     &j->problem.parameters,&parameter_identity)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 if(o->analysis_result.records_produced==1){
  const fractal_analysis_record*record=&o->analysis_result.records[0];
  ri=record->identity;
  if(record->type_id==FRACTAL_ANALYSIS_RECORD_ESCAPE_SUMMARY){
   schema=FRACTAL_ESCAPE_SUMMARY_SCHEMA_ID;sv=1;
  }else if(record->type_id==FRACTAL_ANALYSIS_RECORD_ITERATION_HISTOGRAM){
   schema=FRACTAL_ITERATION_HISTOGRAM_SCHEMA_ID;sv=1;
  }else if(record->type_id==FRACTAL_ANALYSIS_RECORD_SPATIAL_WORKLOAD_GRID){
   schema=FRACTAL_SPATIAL_WORKLOAD_SCHEMA_ID;sv=1;
  }
 }
 n=snprintf(b,c,"{\"analysis_pipeline_identity\":\"%016llx\",\"analysis_record_count\":%zu,\"analysis_record_identity\":\"%016llx\",\"analysis_result\":%u,\"analysis_samples_examined\":%llu,\"analyzed_field_checksum\":\"%016llx\",\"analyzer\":\"%s\",\"analyzer_count\":%zu,\"artifact_bytes\":%zu,\"artifact_checksum\":\"%016llx\",\"computation\":\"%s\",\"computation_identity\":\"%016llx\",\"computation_status\":\"%s\",\"computation_version\":%u,\"assignment_count\":%u,\"decomposition\":\"%s\",\"decomposition_version\":%u,\"destination\":\"%s\",\"effective_worker_count\":%u,\"execution_result\":%u,\"execution_status\":\"%s\",\"field_preserved\":%s,\"formula\":\"%s\",\"formula_compatibility\":\"compatible\",\"formula_contract_version\":%u,\"formula_execution_identity\":\"%016llx\",\"formula_interface_version\":%u,\"formula_parameter_identity\":\"%016llx\",\"formula_version\":%u,\"numeric\":\"%s\",\"numeric_abi_version\":%u,\"numeric_compatibility\":\"compatible\",\"numeric_execution_identity\":\"%016llx\",\"numeric_version\":%u,\"pixel_checksum\":\"%016llx\",\"publication_status\":\"%s\",\"record_schema\":\"%s\",\"record_schema_version\":%u,\"registry_identity\":\"%016llx\",\"requested_worker_count\":%u,\"scheduler\":\"%s\",\"scheduler_version\":%u,\"sealed_work_unit\":\"%s\",\"sealed_work_unit_identity\":\"%016llx\",\"sink\":\"%s\",\"source_field_checksum\":\"%016llx\"}",
  (unsigned long long)o->analysis_pipeline_identity,o->analysis_result.records_produced,
  (unsigned long long)ri,(unsigned)o->analysis_result.result,
  (unsigned long long)o->analysis_result.samples_examined,
  (unsigned long long)o->analyzed_field_checksum,
  r->analysis.count?r->analysis.requests[0].analyzer_id:"",o->analyzer_count,
  a->byte_count,(unsigned long long)a->encoded_checksum,r->compute->descriptor->module_id,
  (unsigned long long)o->scheduler_execution.computation_identity,
  fractal_scheduler_execution_status_string(o->scheduler_execution.status),
  r->compute->descriptor->module_version,o->scheduler_execution.assignment_count,
  scheduler_decomposition_id(r),o->scheduler_execution.decomposition_version,
  a->destination==FRACTAL_ARTIFACT_DESTINATION_FILE?"file":"memory",
  o->scheduler_execution.effective_worker_count,(unsigned)o->scheduler_execution.result,
  fractal_scheduler_execution_status_string(o->scheduler_execution.status),
  o->analyzer_count?(o->analysis_result.field_preserved?"true":"false"):"true",
  r->formula->descriptor->module_id,r->formula->contract_version,
  (unsigned long long)formula_identity,r->formula->interface_version,
  (unsigned long long)parameter_identity,r->formula->descriptor->module_version,
  r->numeric->descriptor->module_id,r->numeric->abi_version,
  (unsigned long long)numeric_identity,r->numeric->descriptor->module_version,
  (unsigned long long)o->pixel_checksum,
  fractal_publication_status_string(o->publication_status),schema,sv,
  (unsigned long long)g->identity,o->scheduler_execution.requested_worker_count,
  r->scheduler->descriptor->module_id,r->scheduler->descriptor->module_version,
  FRACTAL_SCHEDULER_WORK_UNIT_V1_ID,
  (unsigned long long)o->scheduler_execution.sealed_work_unit_identity,a->sink_module_id,
  (unsigned long long)o->source_field_checksum);
 *l=n<0?0u:(size_t)n;
 return n<0||!b||c<=(size_t)n?FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}
