#include "fractal/render_job.h"
#include <stdio.h>
#include <string.h>
fractal_result fractal_render_job_init(fractal_render_job *job, const char *job_id,
 const fractal_render_spec *spec, uint64_t submitted, const char *destination) {
    if (!job || !job_id || !spec || !destination || !*job_id || !*destination ||
        strlen(job_id) >= sizeof(job->job_id) || strlen(destination) >= sizeof(job->output_destination))
        return FRACTAL_ERROR_INVALID_ARGUMENT;
    if (fractal_render_spec_validate(spec) != FRACTAL_OK) return FRACTAL_ERROR_INVALID_SPEC;
    memset(job, 0, sizeof(*job)); strcpy(job->job_id, job_id); job->spec = *spec;
    job->submitted_unix_milliseconds = submitted; job->state = FRACTAL_JOB_QUEUED;
    strcpy(job->output_destination, destination); return FRACTAL_OK;
}
