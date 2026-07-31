#ifndef FRACTAL_RENDER_FAILURE_H
#define FRACTAL_RENDER_FAILURE_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/render_job.h"
#include "fractal/worker_status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_ERROR_CODE_CAPACITY 64u
#define FRACTAL_SUBSYSTEM_CAPACITY 64u
#define FRACTAL_DIAGNOSTICS_CAPACITY 512u
typedef struct fractal_render_failure {
 char error_code[FRACTAL_ERROR_CODE_CAPACITY], summary[FRACTAL_ERROR_SUMMARY_CAPACITY];
 char subsystem[FRACTAL_SUBSYSTEM_CAPACITY], job_id[FRACTAL_JOB_ID_CAPACITY];
 bool recoverable, has_frame, has_tile, has_worker;
 uint32_t frame_index; char tile_id[FRACTAL_ASSIGNMENT_CAPACITY];
 char worker_id[FRACTAL_WORKER_ID_CAPACITY], diagnostics[FRACTAL_DIAGNOSTICS_CAPACITY];
} fractal_render_failure;
fractal_result fractal_render_failure_init(fractal_render_failure *failure,
 const char *error_code, const char *summary, const char *subsystem, bool recoverable);
#ifdef __cplusplus
}
#endif
#endif
