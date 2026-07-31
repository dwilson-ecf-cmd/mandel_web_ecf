#ifndef FRACTAL_CDC_REGION_STUDY_H
#define FRACTAL_CDC_REGION_STUDY_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fractal_computation.h"

#define FRACTAL_CDC_REGION_STUDY_ID "cdc-region-study-0-v1"

typedef struct fractal_cdc_region {
 double real_min, real_max, imaginary_min, imaginary_max;
} fractal_cdc_region;

typedef enum fractal_region_classification {
 FRACTAL_REGION_CERTIFIED_ESCAPED=0,
 FRACTAL_REGION_CERTIFIED_BOUNDED=1,
 FRACTAL_REGION_UNRESOLVED=2,
 FRACTAL_REGION_NUMERICALLY_UNCERTAIN=3
} fractal_region_classification;

typedef struct fractal_cdc_region_certificate {
 fractal_cdc_region region;
 double escape_radius;
 uint64_t iteration_bound;
 double z2_real_lower;
 bool enclosure_valid;
 bool conventional_escape_predicate;
 bool guard_sound;
 bool cdc_transition_certified;
 bool obligation_discharged;
 uint64_t potential_before, potential_after;
 uint64_t cdc_descent_step_count;
 fractal_region_classification classification;
} fractal_cdc_region_certificate;

fractal_result fractal_cdc_region_study_0_create(fractal_cdc_region_certificate *certificate);
fractal_result fractal_cdc_region_validate(const fractal_cdc_region *region);
fractal_region_classification fractal_region_conventional_oracle(const fractal_cdc_region *region);
fractal_result fractal_cdc_region_certificate_validate(const fractal_cdc_region_certificate *certificate);
fractal_result fractal_cdc_region_certificate_serialize(const fractal_cdc_region_certificate *certificate,
 char *buffer, size_t capacity, size_t *length);
#endif
