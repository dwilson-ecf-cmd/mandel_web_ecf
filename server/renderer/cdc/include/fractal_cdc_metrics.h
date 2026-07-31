#ifndef FRACTAL_CDC_METRICS_H
#define FRACTAL_CDC_METRICS_H
#include <stdint.h>
/* Transitional aggregate telemetry only; definitions belong to CDC-LEDGER-006
 * and computation results, never to rasterization mathematics. */
typedef struct fractal_cdc_metrics {
 uint64_t states_evaluated, descent_steps, conditional_branches;
 uint64_t terminal_classifications, unresolved_classifications, fallback_count;
 uint64_t maximum_evidence_depth;
} fractal_cdc_metrics;
#endif
