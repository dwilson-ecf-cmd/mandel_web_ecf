#ifndef FRACTAL_NUMERIC_H
#define FRACTAL_NUMERIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fractal/module.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_NUMERIC_BINARY64_V1_ID "fractal.numeric.binary64.v1"
#define FRACTAL_NUMERIC_ABI_VERSION 1u
#define FRACTAL_NUMERIC_CONTRACT_VERSION 1u

typedef struct fractal_scalar {
 unsigned char storage[8];
} fractal_scalar;

typedef struct fractal_numeric_complex {
 fractal_scalar real;
 fractal_scalar imaginary;
} fractal_numeric_complex;

typedef enum fractal_numeric_constant_v1 {
 FRACTAL_NUMERIC_CONSTANT_ZERO=0,
 FRACTAL_NUMERIC_CONSTANT_ONE,
 FRACTAL_NUMERIC_CONSTANT_TWO,
 FRACTAL_NUMERIC_CONSTANT_HALF
} fractal_numeric_constant_v1;

typedef enum fractal_numeric_order_v1 {
 FRACTAL_NUMERIC_ORDER_LESS=-1,
 FRACTAL_NUMERIC_ORDER_EQUAL=0,
 FRACTAL_NUMERIC_ORDER_GREATER=1
} fractal_numeric_order_v1;

enum {
 FRACTAL_NUMERIC_CAP_REAL_ARITHMETIC=UINT64_C(1)<<0,
 FRACTAL_NUMERIC_CAP_REAL_DIVIDE=UINT64_C(1)<<1,
 FRACTAL_NUMERIC_CAP_COMPLEX_ARITHMETIC=UINT64_C(1)<<2,
 FRACTAL_NUMERIC_CAP_COMPLEX_SQUARE=UINT64_C(1)<<3,
 FRACTAL_NUMERIC_CAP_SQUARED_MAGNITUDE=UINT64_C(1)<<4,
 FRACTAL_NUMERIC_CAP_BAILOUT_COMPARISON=UINT64_C(1)<<5,
 FRACTAL_NUMERIC_CAP_EQUALITY=UINT64_C(1)<<6,
 FRACTAL_NUMERIC_CAP_CANONICAL_CONSTANTS=UINT64_C(1)<<7,
 FRACTAL_NUMERIC_CAP_BINARY64_CONVERSION=UINT64_C(1)<<8,
 FRACTAL_NUMERIC_CAP_UNSIGNED_CONVERSION=UINT64_C(1)<<9,
 FRACTAL_NUMERIC_CAP_FINITE_VALUES=UINT64_C(1)<<10,
 FRACTAL_NUMERIC_CAP_CANONICAL_SERIALIZATION=UINT64_C(1)<<11
};

#define FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES \
 (FRACTAL_NUMERIC_CAP_REAL_ARITHMETIC|FRACTAL_NUMERIC_CAP_REAL_DIVIDE| \
  FRACTAL_NUMERIC_CAP_COMPLEX_ARITHMETIC|FRACTAL_NUMERIC_CAP_COMPLEX_SQUARE| \
  FRACTAL_NUMERIC_CAP_SQUARED_MAGNITUDE|FRACTAL_NUMERIC_CAP_BAILOUT_COMPARISON| \
  FRACTAL_NUMERIC_CAP_EQUALITY|FRACTAL_NUMERIC_CAP_CANONICAL_CONSTANTS| \
  FRACTAL_NUMERIC_CAP_BINARY64_CONVERSION|FRACTAL_NUMERIC_CAP_UNSIGNED_CONVERSION| \
  FRACTAL_NUMERIC_CAP_FINITE_VALUES|FRACTAL_NUMERIC_CAP_CANONICAL_SERIALIZATION)

typedef struct fractal_numeric_vtable {
 const fractal_module_descriptor *descriptor;
 uint32_t abi_version;
 uint32_t contract_version;
 uint64_t capability_flags;
 uint32_t precision_bits;
 size_t scalar_size;
 size_t scalar_alignment;
 fractal_result (*constant)(fractal_numeric_constant_v1,fractal_scalar*);
 fractal_result (*from_binary64)(double,fractal_scalar*);
 fractal_result (*from_uint32)(uint32_t,fractal_scalar*);
 fractal_result (*to_binary64)(const fractal_scalar*,double*);
 fractal_result (*real_add)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*);
 fractal_result (*real_subtract)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*);
 fractal_result (*real_multiply)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*);
 fractal_result (*real_divide)(const fractal_scalar*,const fractal_scalar*,fractal_scalar*);
 fractal_result (*real_compare)(const fractal_scalar*,const fractal_scalar*,fractal_numeric_order_v1*);
 fractal_result (*real_equal)(const fractal_scalar*,const fractal_scalar*,bool*);
 bool (*real_is_finite)(const fractal_scalar*);
 fractal_result (*complex_construct)(const fractal_scalar*,const fractal_scalar*,fractal_numeric_complex*);
 fractal_result (*complex_add)(const fractal_numeric_complex*,const fractal_numeric_complex*,fractal_numeric_complex*);
 fractal_result (*complex_subtract)(const fractal_numeric_complex*,const fractal_numeric_complex*,fractal_numeric_complex*);
 fractal_result (*complex_multiply)(const fractal_numeric_complex*,const fractal_numeric_complex*,fractal_numeric_complex*);
 fractal_result (*complex_square)(const fractal_numeric_complex*,fractal_numeric_complex*);
 fractal_result (*squared_magnitude)(const fractal_numeric_complex*,fractal_scalar*);
 fractal_result (*bailout_exceeded)(const fractal_scalar*,const fractal_scalar*,bool*);
 fractal_result (*serialize_real)(const fractal_scalar*,char*,size_t,size_t*);
} fractal_numeric_vtable;

fractal_result fractal_numeric_validate_v1(const fractal_numeric_vtable*,uint64_t);
uint64_t fractal_numeric_execution_identity_v1(const fractal_numeric_vtable*);

extern const fractal_numeric_vtable fractal_numeric_binary64_v1;

#ifdef __cplusplus
}
#endif
#endif
