#include "fractal/render_progress.h"
#include <string.h>
fractal_result fractal_render_progress_init(fractal_render_progress *progress) {
    if (!progress) return FRACTAL_ERROR_INVALID_ARGUMENT;
    memset(progress, 0, sizeof(*progress)); return FRACTAL_OK;
}
fractal_result fractal_render_progress_validate(const fractal_render_progress *progress) {
    if (!progress) return FRACTAL_ERROR_INVALID_ARGUMENT;
    if (progress->completed_frames > progress->total_frames || progress->completed_tiles > progress->total_tiles)
        return FRACTAL_ERROR_INVALID_ARGUMENT;
    return FRACTAL_OK;
}
