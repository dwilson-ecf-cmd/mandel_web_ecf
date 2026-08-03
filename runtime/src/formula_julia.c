#include "formula_quadratic_internal.h"

#include <stdio.h>
#include <string.h>

static const fractal_module_descriptor descriptor={
 FRACTAL_MODULE_ABI_VERSION,1u,FRACTAL_FORMULA_JULIA_V1_ID,
 "Julia formula v1",FRACTAL_MODULE_FORMULA,FRACTAL_CAP_POINT_SCALAR,true
};

static bool parameter_shape(const fractal_formula_parameters *parameters){
 return parameters&&parameters->type_id&&parameters->data&&
  (!strcmp(parameters->type_id,FRACTAL_FORMULA_JULIA_V1_ID)||
   !strcmp(parameters->type_id,FRACTAL_FORMULA_JULIA_COMPATIBILITY_ID))&&
  parameters->size==sizeof(fractal_julia_parameters)&&
  (uintptr_t)parameters->data%_Alignof(fractal_julia_parameters)==0u;
}

static fractal_result validate_parameters(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters){
 const fractal_julia_parameters *value;
 fractal_scalar converted;
 if(!parameter_shape(parameters))return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)!=
     FRACTAL_OK||numeric->from_binary64(value->constant_real,&converted)!=FRACTAL_OK||
    numeric->from_binary64(value->constant_imaginary,&converted)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return fractal_quadratic_positive_radius_v1(numeric,value->escape_radius);
}

static fractal_result parameter_identity(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,uint64_t *identity){
 const fractal_julia_parameters *value;
 double values[3];
 if(!identity)return FRACTAL_ERROR_INVALID_ARGUMENT;
 *identity=0;
 if(validate_parameters(numeric,parameters)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 values[0]=value->constant_real;
 values[1]=value->constant_imaginary;
 values[2]=value->escape_radius;
 *identity=fractal_quadratic_parameter_identity_v1(FRACTAL_FORMULA_JULIA_V1_ID,
  values,3u);
 return *identity?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

static fractal_result initialize_state(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,const fractal_numeric_complex *sample,
 fractal_formula_state_v1 *state){
 const fractal_julia_parameters *value;
 fractal_numeric_complex constant;
 fractal_scalar real,imaginary;
 if(!sample||!state||validate_parameters(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(numeric->from_binary64(value->constant_real,&real)!=FRACTAL_OK||
    numeric->from_binary64(value->constant_imaginary,&imaginary)!=FRACTAL_OK||
    numeric->complex_construct(&real,&imaginary,&constant)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return fractal_quadratic_initialize_v1(numeric,sample,&constant,
  value->escape_radius,state);
}

static fractal_result serialize_parameters(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,char *output,size_t capacity,
 size_t *length){
 const fractal_julia_parameters *value;
 int count;
 if(!length||validate_parameters(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 count=snprintf(output,capacity,
  "{\"constant_imaginary\":%.17g,\"constant_real\":%.17g,\"escape_radius\":%.17g}",
  value->constant_imaginary,value->constant_real,value->escape_radius);
 *length=count<0?0u:(size_t)count;
 return count<0||!output||capacity<=(size_t)count?
  FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

const fractal_formula_vtable fractal_formula_julia_v1={
 &descriptor,FRACTAL_FORMULA_INTERFACE_VERSION,FRACTAL_FORMULA_CONTRACT_VERSION,
 FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 sizeof(fractal_julia_parameters),_Alignof(fractal_julia_parameters),
 FRACTAL_QUADRATIC_STATE_SIZE,FRACTAL_QUADRATIC_STATE_ALIGNMENT,
 "fractal.formula.execution.julia.quadratic.v1",
 FRACTAL_FORMULA_JULIA_COMPATIBILITY_ID,
 validate_parameters,parameter_identity,initialize_state,
 fractal_quadratic_recurrence_v1,fractal_quadratic_terminate_v1,
 fractal_quadratic_classify_v1,serialize_parameters
};
