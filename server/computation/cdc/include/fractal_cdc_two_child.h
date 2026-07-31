#ifndef FRACTAL_CDC_TWO_CHILD_H
#define FRACTAL_CDC_TWO_CHILD_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fractal_cdc_region_study.h"

#define FRACTAL_CDC_TWO_CHILD_STUDY_ID "cdc-two-child-study-v1"
#define FRACTAL_CDC_TWO_CHILD_DECISION "SUPPORTED_BRANCHING_DESCENT"

typedef enum fractal_cdc_split_axis { FRACTAL_CDC_SPLIT_REAL=0 } fractal_cdc_split_axis;
typedef struct fractal_cdc_two_child_split {
 fractal_cdc_region parent, left, right;
 fractal_cdc_split_axis axis;
 double split_value;
 uint64_t parent_area_units, left_area_units, right_area_units;
 uint64_t potential_before, potential_after;
} fractal_cdc_two_child_split;

typedef struct fractal_cdc_two_child_certificate {
 fractal_cdc_two_child_split split;
 double child_z2_real_lower[2];
 bool coverage_valid, interiors_disjoint, children_strictly_smaller;
 bool child_escape_guard[2], child_discharged[2];
 uint64_t sequential_potential[4];
 uint64_t batch_potential[3];
 uint64_t cdc_descent_step_count;
 bool parent_fully_resolved;
} fractal_cdc_two_child_certificate;

fractal_result fractal_cdc_two_child_split_create(fractal_cdc_two_child_split *split);
fractal_result fractal_cdc_two_child_split_validate(const fractal_cdc_two_child_split *split);
bool fractal_cdc_two_child_multiset_descends(uint64_t parent_units,
 uint64_t left_units, uint64_t right_units);
fractal_result fractal_cdc_two_child_discharge(uint32_t obligations_before,
 uint32_t verified_guards, uint32_t *obligations_after, uint64_t *potential_after);
fractal_result fractal_cdc_two_child_certificate_create(fractal_cdc_two_child_certificate *certificate);
fractal_result fractal_cdc_two_child_certificate_validate(const fractal_cdc_two_child_certificate *certificate);
fractal_result fractal_cdc_two_child_certificate_serialize(const fractal_cdc_two_child_certificate *certificate,
 char *buffer, size_t capacity, size_t *length);
#endif
