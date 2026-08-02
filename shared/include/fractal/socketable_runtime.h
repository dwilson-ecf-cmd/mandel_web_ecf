#ifndef FRACTAL_SOCKETABLE_RUNTIME_H
#define FRACTAL_SOCKETABLE_RUNTIME_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifndef __cplusplus
#include <stdatomic.h>
#endif
#include "fractal/module.h"
#include "fractal/memory_backend.h"
#include "fractal/artifact_sink.h"
#include "fractal/module_registry.h"
#include "fractal/analysis.h"
#include "fractal/scheduler.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum fractal_point_class { FRACTAL_CLASS_ESCAPED=0,FRACTAL_CLASS_BOUNDED,FRACTAL_CLASS_UNRESOLVED,FRACTAL_CLASS_CANCELLED,FRACTAL_CLASS_FAILED } fractal_point_class;
typedef enum fractal_pixel_format { FRACTAL_PIXEL_BGR8=0 } fractal_pixel_format;
typedef struct fractal_scalar { unsigned char storage[8]; } fractal_scalar;
typedef struct fractal_complex_state { fractal_scalar zr,zi,cr,ci,radius_squared; } fractal_complex_state;
typedef struct fractal_formula_parameters { const char *type_id; const void *data; size_t size; } fractal_formula_parameters;
typedef struct fractal_mandelbrot_parameters { double escape_radius; } fractal_mandelbrot_parameters;
typedef struct fractal_julia_parameters { double constant_real,constant_imaginary,escape_radius; } fractal_julia_parameters;
typedef struct fractal_point_result_compact { uint32_t steps; uint8_t classification; uint8_t reserved[3]; } fractal_point_result_compact;
typedef struct fractal_field { uint32_t width,height; size_t stride; fractal_point_result_compact *samples; uint32_t completed_rows; bool complete; } fractal_field;
typedef struct fractal_pixel_buffer { uint32_t width,height; size_t stride; fractal_pixel_format format; unsigned char *pixels; } fractal_pixel_buffer;
#ifdef __cplusplus
typedef bool fractal_cancellation_flag;
#else
typedef atomic_bool fractal_cancellation_flag;
#endif
typedef struct fractal_cancellation { fractal_cancellation_flag requested; } fractal_cancellation;
bool fractal_cancellation_is_requested(const fractal_cancellation*);
void fractal_cancellation_request(fractal_cancellation*);
void fractal_cancellation_reset(fractal_cancellation*);
struct fractal_runtime_modules;

typedef struct fractal_numeric_vtable { const fractal_module_descriptor *descriptor; uint32_t precision_bits; size_t scalar_size,scalar_alignment;
 fractal_result (*from_double)(double,fractal_scalar*); fractal_result (*to_double)(const fractal_scalar*,double*);
 fractal_result (*add)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*); fractal_result (*subtract)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*);
 fractal_result (*multiply)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*); bool (*finite)(const fractal_scalar*);
 fractal_result (*serialize)(const fractal_scalar*,char*,size_t,size_t*); } fractal_numeric_vtable;
typedef struct fractal_formula_vtable { const fractal_module_descriptor *descriptor; uint64_t required_numeric_capabilities;
 fractal_result (*validate_parameters)(const fractal_formula_parameters*); size_t (*state_size)(void); size_t (*state_alignment)(void);
 fractal_result (*initialize_state)(const fractal_numeric_vtable*,const fractal_formula_parameters*,double,double,void*);
 fractal_result (*step)(const fractal_numeric_vtable*,void*); fractal_result (*classify)(const fractal_numeric_vtable*,const void*,fractal_point_class*);
 fractal_result (*serialize_parameters)(const fractal_formula_parameters*,char*,size_t,size_t*); } fractal_formula_vtable;

#define FRACTAL_COMPUTATION_SCALAR_V1_ID "fractal.compute.scalar.v1"
#define FRACTAL_COMPUTE_CONVENTIONAL_COMPATIBILITY_ID "compute.conventional.scalar-c"
#define FRACTAL_COMPUTATION_ABI_VERSION 1u
#define FRACTAL_COMPUTATION_CONTRACT_VERSION 1u

typedef enum fractal_computation_execution_status {
 FRACTAL_COMPUTATION_NOT_STARTED=0,FRACTAL_COMPUTATION_SUCCEEDED,
 FRACTAL_COMPUTATION_CANCELLED,FRACTAL_COMPUTATION_FAILED
} fractal_computation_execution_status;

/* All pointers are borrowed for one synchronous invocation.  The destination
 * remains caller-owned and only the assignment's half-open row range is writable. */
typedef struct fractal_computation_problem_v1 {
 uint32_t abi_version,contract_version;
 const fractal_formula_vtable *formula;
 const fractal_numeric_vtable *numeric;
 fractal_formula_parameters parameters;
 uint32_t maximum_steps;
 double center_real,center_imaginary,scale;
 fractal_field_descriptor field;
 uint64_t identity;
} fractal_computation_problem_v1;

typedef struct fractal_computation_request_v1 {
 uint32_t abi_version,contract_version;
 const fractal_computation_problem_v1 *problem;
 const fractal_sealed_work_unit_v1 *assignment;
 fractal_mutable_field_view destination;
 const fractal_cancellation *cancellation;
} fractal_computation_request_v1;

typedef struct fractal_computation_result_v1 {
 uint64_t assignment_identity;
 uint32_t sequence,rows_completed;
 uint64_t samples_completed;
 fractal_computation_execution_status status;
 fractal_result result;
} fractal_computation_result_v1;

typedef struct fractal_compute_vtable { const fractal_module_descriptor *descriptor; uint64_t required_formula_capabilities,required_numeric_capabilities;
 uint64_t required_field_capabilities; uint32_t contract_version;
 fractal_result (*point)(const fractal_formula_vtable*,const fractal_numeric_vtable*,const fractal_formula_parameters*,double,double,uint32_t,const fractal_cancellation*,fractal_point_result_compact*);
 fractal_result (*execute)(const fractal_computation_request_v1*,fractal_computation_result_v1*); } fractal_compute_vtable;
typedef struct fractal_refinement_vtable { const fractal_module_descriptor *descriptor; bool available; } fractal_refinement_vtable;
typedef void (*fractal_progress_fn)(uint32_t completed,uint32_t total,void *context);
struct fractal_job_spec;
typedef struct fractal_scheduler_vtable { const fractal_module_descriptor *descriptor; uint64_t required_compute_capabilities,produced_capabilities;
 fractal_result (*execute)(const struct fractal_runtime_modules*,const struct fractal_job_spec*,fractal_field*,const fractal_cancellation*,fractal_progress_fn,void*); } fractal_scheduler_vtable;
typedef struct fractal_raster_vtable { const fractal_module_descriptor *descriptor; uint64_t accepted_field_capabilities,produced_capabilities;
 fractal_result (*rasterize)(const fractal_field_view*,uint32_t,fractal_pixel_buffer*); } fractal_raster_vtable;
typedef struct fractal_write_sink { void *context; fractal_result (*write)(void*,const void*,size_t); } fractal_write_sink;
typedef struct fractal_encoder_vtable { const fractal_module_descriptor *descriptor; uint64_t accepted_pixel_capabilities;
 fractal_result (*encode)(const fractal_pixel_buffer*,fractal_write_sink*); } fractal_encoder_vtable;
typedef enum fractal_telemetry_event_kind { FRACTAL_EVENT_JOB_START=0,FRACTAL_EVENT_PROGRESS,FRACTAL_EVENT_JOB_END,FRACTAL_EVENT_COMPATIBILITY_REJECTION,FRACTAL_EVENT_ARTIFACT_COMPLETE,FRACTAL_EVENT_ANALYSIS_BEGIN,FRACTAL_EVENT_ANALYSIS_PROGRESS,FRACTAL_EVENT_ANALYSIS_COMPLETE,FRACTAL_EVENT_ANALYSIS_CANCELLED,FRACTAL_EVENT_ANALYSIS_FAILED } fractal_telemetry_event_kind;
typedef struct fractal_telemetry_event { fractal_telemetry_event_kind kind; uint64_t value; fractal_result result; } fractal_telemetry_event;
typedef struct fractal_telemetry_vtable { const fractal_module_descriptor *descriptor; void (*emit)(void*,const fractal_telemetry_event*); } fractal_telemetry_vtable;
typedef struct fractal_platform_vtable { const fractal_module_descriptor *descriptor; } fractal_platform_vtable;
typedef struct fractal_memory_vtable { const fractal_module_descriptor *descriptor; } fractal_memory_vtable;

typedef struct fractal_problem_spec { const char *formula_id; fractal_formula_parameters parameters; uint32_t maximum_steps; } fractal_problem_spec;
typedef struct fractal_view_spec { double center_real,center_imaginary,scale; uint32_t width,height; } fractal_view_spec;
typedef struct fractal_raster_spec { const char *palette_id; fractal_pixel_format pixel_format; } fractal_raster_spec;
typedef struct fractal_artifact_spec { const char *encoder_id,*output_name; } fractal_artifact_spec;
typedef struct fractal_job_spec { fractal_problem_spec problem; fractal_view_spec view; fractal_raster_spec raster; fractal_artifact_spec artifact; } fractal_job_spec;
typedef struct fractal_runtime_modules { const fractal_formula_vtable *formula; const fractal_numeric_vtable *numeric; const fractal_compute_vtable *compute;
 const fractal_refinement_vtable *refinement; const fractal_scheduler_vtable *scheduler; const fractal_raster_vtable *raster; const fractal_encoder_vtable *encoder;
 const fractal_memory_vtable *memory_module; fractal_memory_backend *memory; const fractal_telemetry_vtable *telemetry; void *telemetry_state; const fractal_platform_vtable *platform; fractal_analysis_pipeline analysis; fractal_scheduler_options scheduler_options; } fractal_runtime_modules;
typedef struct fractal_runtime_output { fractal_field field; fractal_pixel_buffer pixels; uint64_t field_checksum,source_field_checksum,analyzed_field_checksum,pixel_checksum,artifact_checksum; size_t artifact_bytes; fractal_analysis_result analysis_result; uint64_t analysis_pipeline_identity; size_t analyzer_count; fractal_scheduler_execution scheduler_execution; fractal_publication_status publication_status; fractal_result pipeline_result; } fractal_runtime_output;

extern const fractal_formula_vtable fractal_formula_mandelbrot,fractal_formula_julia;
extern const fractal_numeric_vtable fractal_numeric_binary64;
extern const fractal_compute_vtable fractal_compute_scalar_v1,fractal_compute_conventional;
extern const fractal_refinement_vtable fractal_refinement_none,fractal_refinement_cdc_unavailable;
extern const fractal_scheduler_vtable fractal_scheduler_serial,fractal_scheduler_serial_v1,fractal_scheduler_thread_pool_v1;
extern const fractal_raster_vtable fractal_raster_native;
extern const fractal_encoder_vtable fractal_encoder_bmp;
extern const fractal_memory_vtable fractal_memory_system_module,fractal_memory_ouro_unavailable_module;
extern const fractal_telemetry_vtable fractal_telemetry_noop;
extern const fractal_platform_vtable fractal_platform_host;
fractal_result fractal_runtime_validate(const fractal_runtime_modules*);
fractal_result fractal_job_spec_validate(const fractal_runtime_modules*,const fractal_job_spec*);
fractal_result fractal_runtime_render(const fractal_runtime_modules*,const fractal_job_spec*,fractal_write_sink*,const fractal_cancellation*,fractal_runtime_output*);
fractal_result fractal_runtime_render_artifact(const fractal_runtime_modules*,const fractal_job_spec*,fractal_artifact_sink*,const fractal_cancellation*,fractal_runtime_output*,fractal_artifact_result*);
uint64_t fractal_checksum64(const void*,size_t);
uint64_t fractal_module_identity_v1(const fractal_module_descriptor*);
fractal_result fractal_computation_problem_init_v1(const fractal_runtime_modules*,const fractal_job_spec*,const fractal_field_descriptor*,fractal_computation_problem_v1*);
fractal_result fractal_computation_problem_validate_v1(const fractal_compute_vtable*,const fractal_computation_problem_v1*);
fractal_result fractal_scheduler_decompose_computation_v1(const fractal_compute_vtable*,const fractal_computation_problem_v1*,uint32_t,fractal_computation_cancellation_mode,fractal_sealed_work_unit_v1*,size_t,size_t*);
fractal_result fractal_scheduler_validate_computation_v1(const fractal_compute_vtable*,const fractal_computation_problem_v1*,const fractal_sealed_work_unit_v1*,size_t,uint32_t);
const char *fractal_computation_execution_status_string(fractal_computation_execution_status);
fractal_result fractal_runtime_manifest(const fractal_runtime_modules*,const fractal_job_spec*,const fractal_runtime_output*,char*,size_t,size_t*);
fractal_result fractal_runtime_artifact_manifest(const fractal_module_registry*,const fractal_runtime_modules*,const fractal_job_spec*,const fractal_runtime_output*,const fractal_artifact_result*,char*,size_t,size_t*);
#ifdef __cplusplus
}
#endif
#endif
