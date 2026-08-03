#ifndef FRACTAL_SCHEDULER_H
#define FRACTAL_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include "fractal/result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_SCHEDULER_SERIAL_V1_ID "fractal.scheduler.serial.v1"
#define FRACTAL_SCHEDULER_THREAD_POOL_V1_ID "fractal.scheduler.thread-pool.v1"
#define FRACTAL_SCHEDULER_SERIAL_COMPATIBILITY_ID "scheduler.serial.row-major"
#define FRACTAL_SCHEDULER_WORK_UNIT_V1_ID "fractal.scheduler.work-unit.contiguous-rows.v1"
#define FRACTAL_SCHEDULER_SERIAL_DECOMPOSITION_V1_ID "fractal.scheduler.decomposition.serial-row-major.v1"
#define FRACTAL_SCHEDULER_THREAD_POOL_DECOMPOSITION_V1_ID "fractal.scheduler.decomposition.contiguous-row-ranges.v1"
#define FRACTAL_SCHEDULER_DECOMPOSITION_VERSION 1u
#define FRACTAL_THREAD_POOL_MAX_WORKERS 16u
#define FRACTAL_WORK_UNIT_FORMULA_ID_CAPACITY 64u
typedef enum fractal_computation_cancellation_mode {
 FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION=1,
 FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY=2
} fractal_computation_cancellation_mode;

typedef struct fractal_scheduler_options {
 uint32_t requested_worker_count;
} fractal_scheduler_options;

/* Неизменяемо после построения. Идентичность служит печатью всех остальных
 * полей; до запуска работников проверка отклоняет изменённые, перекрывающиеся
 * и выходящие за диапазон назначения. */
typedef struct fractal_sealed_work_unit_v1 {
 uint64_t identity;
 uint64_t work_unit_identity,computation_identity,numeric_identity,formula_identity;
 uint64_t formula_parameter_identity,formula_execution_identity;
 uint64_t numeric_capability_flags;
 uint64_t sample_begin, sample_end;
 uint32_t abi_version, contract_version, sequence, worker_count;
 uint32_t computation_version,numeric_version,numeric_abi_version,formula_version;
 uint32_t formula_interface_version,formula_contract_version,field_format;
 uint32_t cancellation_mode;
 uint32_t width, height, row_begin, row_end;
 char formula_id[FRACTAL_WORK_UNIT_FORMULA_ID_CAPACITY];
} fractal_sealed_work_unit_v1;

typedef enum fractal_scheduler_execution_status {
 FRACTAL_SCHEDULER_EXECUTION_NOT_STARTED=0,
 FRACTAL_SCHEDULER_EXECUTION_SUCCEEDED,
 FRACTAL_SCHEDULER_EXECUTION_CANCELLED,
 FRACTAL_SCHEDULER_EXECUTION_FAILED
} fractal_scheduler_execution_status;

typedef enum fractal_publication_status {
 FRACTAL_PUBLICATION_NOT_APPLICABLE=0,
 FRACTAL_PUBLICATION_ACTIVE,
 FRACTAL_PUBLICATION_COMMITTED,
 FRACTAL_PUBLICATION_ABORTED,
 FRACTAL_PUBLICATION_FAILED
} fractal_publication_status;

typedef struct fractal_scheduler_execution {
 uint64_t sealed_work_unit_identity,computation_identity;
 uint32_t requested_worker_count, effective_worker_count, assignment_count;
 uint32_t decomposition_version;
 fractal_scheduler_execution_status status;
 fractal_result result;
} fractal_scheduler_execution;

fractal_result fractal_scheduler_decompose_contiguous_rows_v1(
 uint32_t width, uint32_t height, uint32_t worker_count,
 fractal_sealed_work_unit_v1 *units, size_t capacity, size_t *unit_count);
fractal_result fractal_scheduler_validate_contiguous_rows_v1(
 const fractal_sealed_work_unit_v1 *units, size_t unit_count,
 uint32_t width, uint32_t height, uint32_t worker_count);
uint64_t fractal_scheduler_work_unit_identity_v1(
 const fractal_sealed_work_unit_v1 *unit);
uint64_t fractal_scheduler_work_unit_set_identity_v1(
 const fractal_sealed_work_unit_v1 *units, size_t unit_count);
const char *fractal_scheduler_execution_status_string(
 fractal_scheduler_execution_status status);
const char *fractal_publication_status_string(fractal_publication_status status);

#ifdef __cplusplus
}
#endif
#endif
