#ifndef FRACTAL_ANALYSIS_H
#define FRACTAL_ANALYSIS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/module.h"
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_ANALYSIS_MAX_ANALYZERS 1u
#define FRACTAL_ANALYSIS_MAX_RECORDS 1u
#define FRACTAL_ANALYSIS_MAX_RECORD_PAYLOAD 584u
#define FRACTAL_ANALYSIS_RECORD_ESCAPE_SUMMARY 1u
#define FRACTAL_ANALYSIS_RECORD_ITERATION_HISTOGRAM 2u
#define FRACTAL_ESCAPE_SUMMARY_SCHEMA_VERSION 1u
#define FRACTAL_ESCAPE_SUMMARY_SCHEMA_ID "fractal.analysis.escape-classification-summary.v1"
#define FRACTAL_ITERATION_HISTOGRAM_SCHEMA_VERSION 1u
#define FRACTAL_ITERATION_HISTOGRAM_SCHEMA_ID "fractal.analysis.iteration-histogram.v1"
#define FRACTAL_ITERATION_HISTOGRAM_BIN_COUNT 64u

typedef enum fractal_field_format {
 FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1=1
} fractal_field_format;
typedef struct fractal_field_descriptor {
 uint32_t width,height; size_t stride; fractal_field_format format; uint32_t flags;
} fractal_field_descriptor;
typedef struct fractal_field_view {
 fractal_field_descriptor descriptor; const void *data; size_t data_size;
} fractal_field_view;
typedef struct fractal_mutable_field_view {
 fractal_field_descriptor descriptor; void *data; size_t data_size;
} fractal_mutable_field_view;
typedef enum fractal_analysis_output_mode {
 FRACTAL_ANALYSIS_BORROW_INPUT=0,FRACTAL_ANALYSIS_COPY_OUTPUT=1
} fractal_analysis_output_mode;
typedef struct fractal_analysis_request {
 const char *analyzer_id; fractal_analysis_output_mode output_mode;
 bool require_field_preservation,require_records; size_t record_capacity;
} fractal_analysis_request;

typedef struct fractal_escape_classification_summary_v1 {
 uint64_t samples_total,escaped_samples,bounded_samples,unresolved_samples;
 uint64_t maximum_iteration_samples,cancelled_samples,failed_samples;
 uint64_t other_classified_samples,iteration_samples;
 uint64_t iterations_min,iterations_max,iterations_sum;
 bool iteration_statistics_valid;
} fractal_escape_classification_summary_v1;
typedef struct fractal_iteration_histogram_v1 {
 uint64_t bins[FRACTAL_ITERATION_HISTOGRAM_BIN_COUNT];
 uint64_t eligible_samples,excluded_samples,total_samples;
 uint64_t minimum_iteration,maximum_iteration,summed_iterations;
 uint64_t overflow_bin_count;
 bool histogram_valid;
} fractal_iteration_histogram_v1;
typedef struct fractal_analysis_record {
 uint32_t type_id,schema_version,payload_size;
 unsigned char payload[FRACTAL_ANALYSIS_MAX_RECORD_PAYLOAD];
 uint64_t identity;
} fractal_analysis_record;
typedef struct fractal_analysis_result {
 const char *analyzer_id; uint32_t analyzer_version;
 uint64_t source_field_checksum,output_field_checksum,samples_examined,identity;
 size_t records_produced; bool field_preserved,cancelled; fractal_result result;
 fractal_analysis_record records[FRACTAL_ANALYSIS_MAX_RECORDS];
} fractal_analysis_result;
struct fractal_cancellation;
typedef struct fractal_analyzer_state {
 unsigned phase; fractal_analysis_request request; fractal_analysis_result result;
} fractal_analyzer_state;
typedef struct fractal_analyzer_vtable {
 const fractal_module_descriptor *descriptor;
 fractal_result (*validate)(const fractal_analysis_request*,const fractal_field_descriptor*);
 fractal_result (*begin)(fractal_analyzer_state*,const fractal_analysis_request*,uint64_t);
 fractal_result (*process)(fractal_analyzer_state*,const fractal_field_view*,fractal_mutable_field_view*,fractal_field_view*,const struct fractal_cancellation*);
 fractal_result (*finish)(fractal_analyzer_state*,fractal_analysis_result*);
 void (*abort)(fractal_analyzer_state*);
} fractal_analyzer_vtable;
typedef struct fractal_analysis_pipeline {
 size_t count; const fractal_analyzer_vtable *analyzers[FRACTAL_ANALYSIS_MAX_ANALYZERS];
 fractal_analysis_request requests[FRACTAL_ANALYSIS_MAX_ANALYZERS]; uint64_t identity;
} fractal_analysis_pipeline;

fractal_result fractal_field_descriptor_validate(const fractal_field_descriptor*,size_t,size_t*);
fractal_result fractal_field_view_validate(const fractal_field_view*,size_t);
fractal_result fractal_mutable_field_view_validate(const fractal_mutable_field_view*,size_t);
fractal_result fractal_analysis_request_serialize(const fractal_analysis_request*,char*,size_t,size_t*);
fractal_result fractal_analysis_pipeline_init(fractal_analysis_pipeline*,const fractal_analyzer_vtable*,const fractal_analysis_request*);
fractal_result fractal_analysis_pipeline_serialize(const fractal_analysis_pipeline*,char*,size_t,size_t*);
fractal_result fractal_analysis_result_serialize(const fractal_analysis_result*,char*,size_t,size_t*);
fractal_result fractal_analysis_record_serialize(const fractal_analysis_record*,char*,size_t,size_t*);
fractal_result fractal_analysis_records_serialize(const fractal_analysis_record*,size_t,char*,size_t,size_t*);
fractal_result fractal_escape_summary_record_decode(const fractal_analysis_record*,fractal_escape_classification_summary_v1*);
/* v1: 0, 1, and 2 have dedicated bins.  For 3 <= b <= 62, bin b is
 * [2^(b-2)+1, 2^(b-1)] (inclusive).  Bin 63 is the overflow bin for values
 * greater than 2^61. */
size_t fractal_iteration_histogram_bucket_v1(uint64_t iteration);
fractal_result fractal_iteration_histogram_record_decode(const fractal_analysis_record*,fractal_iteration_histogram_v1*);
extern const fractal_analyzer_vtable fractal_analyzer_passthrough;
extern const fractal_analyzer_vtable fractal_analyzer_escape_classification_summary;
extern const fractal_analyzer_vtable fractal_analyzer_iteration_histogram;
#ifdef __cplusplus
}
#endif
#endif
