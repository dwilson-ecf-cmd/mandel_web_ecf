#ifndef FRACTAL_FORMULA_H
#define FRACTAL_FORMULA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fractal/numeric.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_FORMULA_MANDELBROT_V1_ID "fractal.formula.mandelbrot.v1"
#define FRACTAL_FORMULA_JULIA_V1_ID "fractal.formula.julia.v1"
#define FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID "formula.mandelbrot.quadratic"
#define FRACTAL_FORMULA_JULIA_COMPATIBILITY_ID "formula.julia.quadratic"
#define FRACTAL_FORMULA_INTERFACE_VERSION 1u
#define FRACTAL_FORMULA_CONTRACT_VERSION 1u
#define FRACTAL_FORMULA_STATE_CAPACITY 64u
#define FRACTAL_FORMULA_STATE_ALIGNMENT 16u
#define FRACTAL_FORMULA_PARAMETER_CAPACITY 64u
#define FRACTAL_FORMULA_PARAMETER_ALIGNMENT 16u

typedef enum fractal_point_class {
 FRACTAL_CLASS_ESCAPED=0,
 FRACTAL_CLASS_BOUNDED,
 FRACTAL_CLASS_UNRESOLVED,
 FRACTAL_CLASS_CANCELLED,
 FRACTAL_CLASS_FAILED
} fractal_point_class;

typedef struct fractal_formula_parameters {
 const char *type_id;
 const void *data;
 size_t size;
} fractal_formula_parameters;

typedef struct fractal_mandelbrot_parameters {
 double escape_radius;
} fractal_mandelbrot_parameters;

typedef struct fractal_julia_parameters {
 double constant_real,constant_imaginary,escape_radius;
} fractal_julia_parameters;

/* Состояние принадлежит вызывающей стороне и не раскрывает внутреннюю схему
 * формулы. Ёмкость и выравнивание являются частью интерфейса версии 1. */
typedef struct fractal_formula_state_v1 {
#ifdef __cplusplus
 alignas(FRACTAL_FORMULA_STATE_ALIGNMENT)
#else
 _Alignas(FRACTAL_FORMULA_STATE_ALIGNMENT)
#endif
 unsigned char storage[FRACTAL_FORMULA_STATE_CAPACITY];
} fractal_formula_state_v1;

typedef struct fractal_formula_vtable {
 const fractal_module_descriptor *descriptor;
 uint32_t interface_version;
 uint32_t contract_version;
 uint64_t required_numeric_capabilities;
 size_t parameter_size;
 size_t parameter_alignment;
 size_t state_size;
 size_t state_alignment;
 const char *execution_contract;
 const char *compatibility_id;
 fractal_result (*validate_parameters)(const fractal_numeric_vtable*,
  const fractal_formula_parameters*);
 fractal_result (*parameter_identity)(const fractal_numeric_vtable*,
  const fractal_formula_parameters*,uint64_t*);
 fractal_result (*initialize_state)(const fractal_numeric_vtable*,
  const fractal_formula_parameters*,const fractal_numeric_complex*,
  fractal_formula_state_v1*);
 fractal_result (*recurrence)(const fractal_numeric_vtable*,
  fractal_formula_state_v1*);
 fractal_result (*terminate)(const fractal_numeric_vtable*,
  fractal_formula_state_v1*,bool*);
 fractal_result (*classify)(const fractal_numeric_vtable*,
  const fractal_formula_state_v1*,bool,fractal_point_class*);
 fractal_result (*serialize_parameters)(const fractal_numeric_vtable*,
  const fractal_formula_parameters*,char*,size_t,size_t*);
} fractal_formula_vtable;

fractal_result fractal_formula_validate_v1(const fractal_formula_vtable*,
 const fractal_numeric_vtable*);
fractal_result fractal_formula_parameters_validate_v1(const fractal_formula_vtable*,
 const fractal_numeric_vtable*,const fractal_formula_parameters*);
fractal_result fractal_formula_parameter_identity_v1(const fractal_formula_vtable*,
 const fractal_numeric_vtable*,const fractal_formula_parameters*,uint64_t*);
uint64_t fractal_formula_execution_identity_v1(const fractal_formula_vtable*);
bool fractal_formula_id_matches_v1(const fractal_formula_vtable*,const char*);

extern const fractal_formula_vtable fractal_formula_mandelbrot_v1;
extern const fractal_formula_vtable fractal_formula_julia_v1;

/* Прежние имена символов сохранены как совместимые с исходным кодом псевдонимы.
 * Авторитетный путь использует символы с суффиксом `_v1`. */
#define fractal_formula_mandelbrot fractal_formula_mandelbrot_v1
#define fractal_formula_julia fractal_formula_julia_v1

#ifdef __cplusplus
}
#endif
#endif
