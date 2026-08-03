#ifndef FRACTAL_CDC_METRICS_H
#define FRACTAL_CDC_METRICS_H
#include <stdint.h>
/* Только переходная агрегированная телеметрия; определения принадлежат
 * CDC-LEDGER-006 и результатам вычисления, но не математике растеризации. */
typedef struct fractal_cdc_metrics {
 uint64_t states_evaluated, descent_steps, conditional_branches;
 uint64_t terminal_classifications, unresolved_classifications, fallback_count;
 uint64_t maximum_evidence_depth;
} fractal_cdc_metrics;
#endif
