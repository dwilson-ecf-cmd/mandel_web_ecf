#ifndef FRACTAL_CDC_TRACE_H
#define FRACTAL_CDC_TRACE_H
#include <stddef.h>
#include "fractal_cdc_substrate.h"
/* Transitional renderer-layer view only. Mathematical evidence is produced by
 * computation backends, not by rasterization. */
typedef struct fractal_cdc_trace {
 const fractal_cdc_substrate_record *records;
 size_t record_count;
 fractal_point_classification classification;
} fractal_cdc_trace;
#endif
