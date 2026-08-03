#include "fractal/formula.h"

#include <stdio.h>
#include <string.h>

static bool valid_alignment(size_t value,size_t maximum){
 return value&&value<=maximum&&(value&(value-1u))==0u;
}

static fractal_result validate_metadata(const fractal_formula_vtable *formula){
 if(!formula||!formula->descriptor)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_module_descriptor_validate(formula->descriptor,FRACTAL_MODULE_FORMULA)!=
     FRACTAL_OK||!formula->descriptor->available||
    formula->interface_version!=FRACTAL_FORMULA_INTERFACE_VERSION||
    formula->contract_version!=FRACTAL_FORMULA_CONTRACT_VERSION||
    !formula->parameter_size||formula->parameter_size>FRACTAL_FORMULA_PARAMETER_CAPACITY||
    !valid_alignment(formula->parameter_alignment,FRACTAL_FORMULA_PARAMETER_ALIGNMENT)||
    !formula->state_size||formula->state_size>FRACTAL_FORMULA_STATE_CAPACITY||
    !valid_alignment(formula->state_alignment,FRACTAL_FORMULA_STATE_ALIGNMENT)||
    !formula->execution_contract||!*formula->execution_contract||
    !formula->validate_parameters||!formula->parameter_identity||
    !formula->initialize_state||!formula->recurrence||!formula->terminate||
    !formula->classify||!formula->serialize_parameters)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_formula_validate_v1(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric){
 if(validate_metadata(formula)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 return fractal_numeric_validate_v1(numeric,formula->required_numeric_capabilities)==
  FRACTAL_OK?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

fractal_result fractal_formula_parameters_validate_v1(
 const fractal_formula_vtable *formula,const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters){
 uintptr_t address;
 if(fractal_formula_validate_v1(formula,numeric)!=FRACTAL_OK||!parameters||
    !parameters->type_id||!parameters->data||
    !fractal_formula_id_matches_v1(formula,parameters->type_id)||
    parameters->size!=formula->parameter_size)return FRACTAL_ERROR_INVALID_SPEC;
 address=(uintptr_t)parameters->data;
 if(address%formula->parameter_alignment)return FRACTAL_ERROR_INVALID_SPEC;
 return formula->validate_parameters(numeric,parameters);
}

fractal_result fractal_formula_parameter_identity_v1(
 const fractal_formula_vtable *formula,const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,uint64_t *identity){
 if(!identity)return FRACTAL_ERROR_INVALID_ARGUMENT;
 *identity=0;
 if(fractal_formula_parameters_validate_v1(formula,numeric,parameters)!=FRACTAL_OK||
    formula->parameter_identity(numeric,parameters,identity)!=FRACTAL_OK||!*identity){
  *identity=0;
  return FRACTAL_ERROR_INVALID_SPEC;
 }
 return FRACTAL_OK;
}

static void hash_bytes(uint64_t *hash,const void *data,size_t size){
 const unsigned char *bytes=data;
 while(size--){*hash^=*bytes++;*hash*=UINT64_C(1099511628211);}
}

uint64_t fractal_formula_execution_identity_v1(const fractal_formula_vtable *formula){
 char text[768];
 uint64_t hash=UINT64_C(1469598103934665603);
 int count;
 if(validate_metadata(formula)!=FRACTAL_OK)return 0;
 count=snprintf(text,sizeof(text),
  "fractal.formula.execution.v1|module=%s@%u|interface=%u|contract=%u|caps=%016llx|parameter=%zu:%zu|state=%zu:%zu|execution=%s",
  formula->descriptor->module_id,formula->descriptor->module_version,
  formula->interface_version,formula->contract_version,
  (unsigned long long)formula->required_numeric_capabilities,
  formula->parameter_size,formula->parameter_alignment,
  formula->state_size,formula->state_alignment,formula->execution_contract);
 if(count<0||(size_t)count>=sizeof(text))return 0;
 hash_bytes(&hash,text,(size_t)count);
 return hash;
}

bool fractal_formula_id_matches_v1(const fractal_formula_vtable *formula,
 const char *formula_id){
 return formula&&formula->descriptor&&formula_id&&
  (!strcmp(formula_id,formula->descriptor->module_id)||
   (formula->compatibility_id&&!strcmp(formula_id,formula->compatibility_id)));
}
