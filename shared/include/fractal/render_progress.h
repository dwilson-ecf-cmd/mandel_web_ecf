#ifndef FRACTAL_RENDER_PROGRESS_H
#define FRACTAL_RENDER_PROGRESS_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct fractal_render_progress {
    uint32_t total_frames, completed_frames;
    uint64_t total_tiles, completed_tiles;
    uint32_t active_workers;
    uint64_t elapsed_milliseconds;
    bool estimate_available;
    uint64_t estimated_completion_unix_milliseconds;
    uint32_t retry_count, failure_count;
} fractal_render_progress;
fractal_result fractal_render_progress_init(fractal_render_progress *progress);
fractal_result fractal_render_progress_validate(const fractal_render_progress *progress);
#ifdef __cplusplus
}
#endif
#endif
