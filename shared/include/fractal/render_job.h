#ifndef FRACTAL_RENDER_JOB_H
#define FRACTAL_RENDER_JOB_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/render_progress.h"
#include "fractal/render_spec.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_JOB_ID_CAPACITY 64u
#define FRACTAL_STORAGE_KEY_CAPACITY 256u
typedef enum fractal_render_job_state { FRACTAL_JOB_QUEUED=0, FRACTAL_JOB_RUNNING, FRACTAL_JOB_COMPLETED, FRACTAL_JOB_FAILED, FRACTAL_JOB_CANCELLED } fractal_render_job_state;
typedef struct fractal_render_job {
    char job_id[FRACTAL_JOB_ID_CAPACITY];
    fractal_render_spec spec;
    uint64_t submitted_unix_milliseconds;
    fractal_render_job_state state;
    fractal_render_progress progress;
    bool cancellation_requested;
    char output_destination[FRACTAL_STORAGE_KEY_CAPACITY];
} fractal_render_job;
fractal_result fractal_render_job_init(fractal_render_job *job, const char *job_id,
    const fractal_render_spec *spec, uint64_t submitted_unix_milliseconds,
    const char *output_destination);
#ifdef __cplusplus
}
#endif
#endif
