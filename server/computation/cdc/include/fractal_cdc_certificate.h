#ifndef FRACTAL_CDC_CERTIFICATE_H
#define FRACTAL_CDC_CERTIFICATE_H
#include <stddef.h>
#include <stdint.h>
#include "fractal_computation.h"

#define FRACTAL_CDC_EXPERIMENT_0_REQUIRED_FIELDS UINT32_C(0x1ff)

typedef enum fractal_cdc_mapping_decision {
 FRACTAL_CDC_MAPPING_NEGATIVE_RESULT=0
} fractal_cdc_mapping_decision;

typedef struct fractal_cdc_experiment_0_certificate {
 uint32_t present_fields;
 fractal_formula_kind formula;
 fractal_numeric_representation numeric_representation;
 double c_real, c_imaginary, initial_z_real, initial_z_imaginary;
 double escape_radius;
 double trace_real[3], trace_imaginary[3];
 uint64_t trace_steps[3];
 bool strict_escape;
 uint64_t conventional_iteration_count;
 uint64_t cdc_descent_step_count;
 fractal_cdc_mapping_decision mapping_decision;
} fractal_cdc_experiment_0_certificate;

fractal_result fractal_cdc_experiment_0_certificate_create(fractal_cdc_experiment_0_certificate *certificate);
fractal_result fractal_cdc_experiment_0_certificate_validate(const fractal_cdc_experiment_0_certificate *certificate);
fractal_result fractal_cdc_experiment_0_certificate_serialize(const fractal_cdc_experiment_0_certificate *certificate,
 char *buffer, size_t capacity, size_t *length);
const char *fractal_cdc_mapping_decision_string(fractal_cdc_mapping_decision decision);
#endif
