#ifndef FRACTAL_COMPUTATION_H
#define FRACTAL_COMPUTATION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_COMPUTATION_ID_CAPACITY 64u
#define FRACTAL_COMPUTATION_REASON_CAPACITY 192u
#define FRACTAL_POINT_TRACE_CAPACITY 16u

typedef enum fractal_computation_backend_kind {
 FRACTAL_COMPUTATION_BACKEND_LEGACY_REFERENCE=0,
 FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C,
 FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL
} fractal_computation_backend_kind;
typedef enum fractal_formula_kind { FRACTAL_FORMULA_MANDELBROT_QUADRATIC=0 } fractal_formula_kind;
typedef enum fractal_numeric_representation { FRACTAL_NUMERIC_BINARY64=0 } fractal_numeric_representation;
typedef enum fractal_point_classification {
 FRACTAL_POINT_ESCAPED=0,
 FRACTAL_POINT_PROVEN_BOUNDED,
 FRACTAL_POINT_UNRESOLVED,
 FRACTAL_POINT_NUMERICALLY_UNCERTAIN,
 FRACTAL_POINT_CANCELLED,
 FRACTAL_POINT_FAILED
} fractal_point_classification;

typedef struct fractal_computation_problem {
 fractal_formula_kind formula;
 fractal_numeric_representation numeric_representation;
 double initial_z_real, initial_z_imaginary;
 double escape_radius;
 uint64_t iteration_budget;
} fractal_computation_problem;
typedef struct fractal_point_input { double c_real, c_imaginary; } fractal_point_input;
typedef struct fractal_point_trace_entry { uint64_t step; double z_real, z_imaginary; } fractal_point_trace_entry;
typedef struct fractal_computation_cancellation { bool requested; } fractal_computation_cancellation;
typedef struct fractal_point_result {
 fractal_point_classification classification;
 fractal_numeric_representation precision_used;
 double final_z_real, final_z_imaginary;
 uint64_t conventional_iteration_count;
 uint64_t cdc_descent_step_count;
 bool fallback_required, fallback_performed;
 char evidence_identity[FRACTAL_COMPUTATION_ID_CAPACITY];
 char deterministic_identity[FRACTAL_COMPUTATION_ID_CAPACITY];
 char failure_reason[FRACTAL_COMPUTATION_REASON_CAPACITY];
 fractal_point_trace_entry trace[FRACTAL_POINT_TRACE_CAPACITY];
 size_t trace_count;
} fractal_point_result;
typedef struct fractal_computation_backend_metadata {
 fractal_computation_backend_kind kind;
 char stable_identifier[FRACTAL_COMPUTATION_ID_CAPACITY];
 char revision[FRACTAL_COMPUTATION_ID_CAPACITY];
} fractal_computation_backend_metadata;

typedef struct fractal_computation_backend fractal_computation_backend;
typedef struct fractal_computation_vtable {
 fractal_result (*validate_problem)(const fractal_computation_problem *problem);
 fractal_result (*compute_point)(const fractal_computation_problem *problem,
  const fractal_point_input *input, const fractal_computation_cancellation *cancellation,
  fractal_point_result *result);
} fractal_computation_vtable;
struct fractal_computation_backend {
 fractal_computation_backend_metadata metadata;
 const fractal_computation_vtable *vtable;
};
fractal_result fractal_computation_backend_create(fractal_computation_backend_kind kind,
 fractal_computation_backend *backend);
fractal_result fractal_computation_backend_validate(const fractal_computation_backend *backend);
fractal_result fractal_computation_problem_init_experiment_0(fractal_computation_problem *problem);
fractal_result fractal_computation_problem_validate(const fractal_computation_problem *problem);
const char *fractal_computation_backend_string(fractal_computation_backend_kind kind);
const char *fractal_point_classification_string(fractal_point_classification classification);
#ifdef __cplusplus
}
#endif
#endif
