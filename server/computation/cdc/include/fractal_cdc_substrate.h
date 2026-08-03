#ifndef FRACTAL_CDC_SUBSTRATE_H
#define FRACTAL_CDC_SUBSTRATE_H
#include <stdbool.h>
#include "fractal_computation.h"
typedef enum fractal_cdc_statement_classification {
 FRACTAL_CDC_SOURCE_DEFINED=0,
 FRACTAL_CDC_ENGINEERING_INTERPRETATION,
 FRACTAL_CDC_EXPERIMENTAL_HYPOTHESIS,
 FRACTAL_CDC_UNRESOLVED
} fractal_cdc_statement_classification;
/* Инженерная запись полей листа доказательства, заданных источником; ни одно
 * поле не утверждает, что итерация Mandelbrot удовлетворяет CDC.
 * Ссылка CDC: CDC-CONFIG-001, CDC-CERT-001. */
typedef struct fractal_cdc_substrate_record {
 char configuration_identity[FRACTAL_COMPUTATION_ID_CAPACITY];
 fractal_cdc_statement_classification state_status;
 fractal_cdc_statement_classification potential_status;
 fractal_cdc_statement_classification certified_step_status;
 fractal_cdc_statement_classification invariant_status;
 fractal_cdc_statement_classification terminal_claim_status;
 bool finite_precision_evidence_available;
 char unresolved_reason[FRACTAL_COMPUTATION_REASON_CAPACITY];
} fractal_cdc_substrate_record;
#endif
