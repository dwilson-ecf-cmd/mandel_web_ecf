#ifndef FRACTAL_WORK_UNIT_H
#define FRACTAL_WORK_UNIT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/socketable_runtime.h"
#define FRACTAL_WORK_UNIT_SCHEMA_VERSION 1u
#define FRACTAL_WORK_UNIT_PARAMETER_CAPACITY 128u
#define FRACTAL_WORK_UNIT_ID_CAPACITY 96u
typedef struct fractal_work_unit {
 uint32_t schema_version;
 uint64_t identity;
 fractal_job_spec job;
 unsigned char parameter_storage[FRACTAL_WORK_UNIT_PARAMETER_CAPACITY];
 char formula_id[FRACTAL_WORK_UNIT_ID_CAPACITY],numeric_id[FRACTAL_WORK_UNIT_ID_CAPACITY];
 char compute_id[FRACTAL_WORK_UNIT_ID_CAPACITY],scheduler_id[FRACTAL_WORK_UNIT_ID_CAPACITY];
 char renderer_id[FRACTAL_WORK_UNIT_ID_CAPACITY],encoder_id[FRACTAL_WORK_UNIT_ID_CAPACITY];
 char sink_id[FRACTAL_WORK_UNIT_ID_CAPACITY],output_name[FRACTAL_ARTIFACT_NAME_CAPACITY];
 uint32_t formula_version,numeric_version,compute_version,scheduler_version,renderer_version,encoder_version,sink_version;
 uint64_t analysis_chain_identity;
 const fractal_cancellation *cancellation;
 bool sealed;
} fractal_work_unit;
fractal_result fractal_work_unit_init(fractal_work_unit*,const fractal_runtime_modules*,const fractal_job_spec*,const fractal_artifact_sink*,const fractal_cancellation*);
fractal_result fractal_work_unit_validate(const fractal_work_unit*,const fractal_runtime_modules*,const fractal_artifact_sink*);
#endif
