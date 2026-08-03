#ifndef FRACTAL_FORMULA_QUADRATIC_INTERNAL_H
#define FRACTAL_FORMULA_QUADRATIC_INTERNAL_H

#include "fractal/formula.h"

typedef struct fractal_quadratic_state_v1 {
 _Alignas(8) fractal_numeric_complex z;
 fractal_numeric_complex c;
 fractal_scalar radius_squared;
 uint8_t escaped;
 uint8_t reserved[7];
} fractal_quadratic_state_v1;

#define FRACTAL_QUADRATIC_STATE_SIZE 48u
#define FRACTAL_QUADRATIC_STATE_ALIGNMENT 8u

fractal_result fractal_quadratic_positive_radius_v1(const fractal_numeric_vtable*,
 double);
fractal_result fractal_quadratic_initialize_v1(const fractal_numeric_vtable*,
 const fractal_numeric_complex*,const fractal_numeric_complex*,double,
 fractal_formula_state_v1*);
fractal_result fractal_quadratic_recurrence_v1(const fractal_numeric_vtable*,
 fractal_formula_state_v1*);
fractal_result fractal_quadratic_terminate_v1(const fractal_numeric_vtable*,
 fractal_formula_state_v1*,bool*);
fractal_result fractal_quadratic_classify_v1(const fractal_numeric_vtable*,
 const fractal_formula_state_v1*,bool,fractal_point_class*);
uint64_t fractal_quadratic_parameter_identity_v1(const char*,const double*,size_t);

#endif
