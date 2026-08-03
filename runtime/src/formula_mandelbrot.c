#include "formula_quadratic_internal.h"

#include <stdio.h>
#include <string.h>

static const fractal_module_descriptor descriptor={
 FRACTAL_MODULE_ABI_VERSION,1u,FRACTAL_FORMULA_MANDELBROT_V1_ID,
 "Mandelbrot formula v1",FRACTAL_MODULE_FORMULA,FRACTAL_CAP_POINT_SCALAR,true
};

static bool parameter_shape(const fractal_formula_parameters *parameters){
 return parameters&&parameters->type_id&&parameters->data&&
  (!strcmp(parameters->type_id,FRACTAL_FORMULA_MANDELBROT_V1_ID)||
   !strcmp(parameters->type_id,FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID))&&
  parameters->size==sizeof(fractal_mandelbrot_parameters)&&
  (uintptr_t)parameters->data%_Alignof(fractal_mandelbrot_parameters)==0u;
}

static fractal_result validate_parameters(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters){
 const fractal_mandelbrot_parameters *value;
 if(!parameter_shape(parameters))return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 return fractal_quadratic_positive_radius_v1(numeric,value->escape_radius);
}

static fractal_result parameter_identity(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,uint64_t *identity){
 const fractal_mandelbrot_parameters *value;
 if(!identity)return FRACTAL_ERROR_INVALID_ARGUMENT;
 *identity=0;
 if(validate_parameters(numeric,parameters)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 *identity=fractal_quadratic_parameter_identity_v1(FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &value->escape_radius,1u);
 return *identity?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

static fractal_result initialize_state(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,const fractal_numeric_complex *sample,
 fractal_formula_state_v1 *state){
 const fractal_mandelbrot_parameters *value;
 fractal_numeric_complex zero_complex;
 fractal_scalar zero;
 if(!sample||!state||validate_parameters(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    numeric->complex_construct(&zero,&zero,&zero_complex)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return fractal_quadratic_initialize_v1(numeric,&zero_complex,sample,
  value->escape_radius,state);
}

static fractal_result serialize_parameters(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,char *output,size_t capacity,
 size_t *length){
 const fractal_mandelbrot_parameters *value;
 int count;
 if(!length||validate_parameters(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 count=snprintf(output,capacity,"{\"escape_radius\":%.17g}",value->escape_radius);
 *length=count<0?0u:(size_t)count;
 return count<0||!output||capacity<=(size_t)count?
  FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

const fractal_formula_vtable fractal_formula_mandelbrot_v1={
 &descriptor,FRACTAL_FORMULA_INTERFACE_VERSION,FRACTAL_FORMULA_CONTRACT_VERSION,
 FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 sizeof(fractal_mandelbrot_parameters),_Alignof(fractal_mandelbrot_parameters),
 FRACTAL_QUADRATIC_STATE_SIZE,FRACTAL_QUADRATIC_STATE_ALIGNMENT,
 "fractal.formula.execution.mandelbrot.quadratic.v1",
 FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID,
 validate_parameters,parameter_identity,initialize_state,
 fractal_quadratic_recurrence_v1,fractal_quadratic_terminate_v1,
 fractal_quadratic_classify_v1,serialize_parameters
};
