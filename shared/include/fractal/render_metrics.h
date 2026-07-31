#ifndef FRACTAL_RENDER_METRICS_H
#define FRACTAL_RENDER_METRICS_H
#include <stdint.h>
#include "fractal/memory_backend.h"
#include "fractal/render_artifact.h"
#include "fractal_renderer.h"
#include "fractal_computation.h"
typedef struct fractal_render_metrics {
 fractal_computation_backend_kind computation_backend;
 fractal_renderer_backend_kind renderer_backend;
 fractal_memory_backend_kind memory_backend;
 uint64_t pixels_rendered, tiles_rendered, frames_rendered;
 uint64_t conventional_iteration_count, cdc_descent_step_count;
 uint64_t maximum_steps_for_one_point;
 uint64_t escaped_points, bounded_points, unresolved_points, failed_points;
 uint64_t render_duration_nanoseconds, retries, cancellation_latency_nanoseconds;
 uint64_t active_memory_bytes, peak_memory_bytes;
 char precision_used[FRACTAL_IDENTIFIER_CAPACITY];
 char output_checksum[FRACTAL_CHECKSUM_CAPACITY];
 char artifact_checksum[FRACTAL_CHECKSUM_CAPACITY];
} fractal_render_metrics;
#endif
