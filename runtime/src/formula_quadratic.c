#include "formula_quadratic_internal.h"

#include <stdio.h>
#include <string.h>

_Static_assert(sizeof(fractal_quadratic_state_v1)==FRACTAL_QUADRATIC_STATE_SIZE,
 "Размер состояния квадратичной формулы должен быть детерминированным");
_Static_assert(_Alignof(fractal_quadratic_state_v1)==FRACTAL_QUADRATIC_STATE_ALIGNMENT,
 "Выравнивание состояния квадратичной формулы должно быть явным");
_Static_assert(sizeof(fractal_formula_state_v1)==FRACTAL_FORMULA_STATE_CAPACITY,
 "Ёмкость состояния формулы изменилась");
_Static_assert(_Alignof(fractal_formula_state_v1)==FRACTAL_FORMULA_STATE_ALIGNMENT,
 "Выравнивание состояния формулы изменилось");

static void load_state(const fractal_formula_state_v1 *source,
 fractal_quadratic_state_v1 *state){
 memcpy(state,source->storage,sizeof(*state));
}

static void store_state(fractal_formula_state_v1 *destination,
 const fractal_quadratic_state_v1 *state){
 memcpy(destination->storage,state,sizeof(*state));
}

fractal_result fractal_quadratic_positive_radius_v1(
 const fractal_numeric_vtable *numeric,double value){
 fractal_scalar converted,zero;
 fractal_numeric_order_v1 order;
 if(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)!=
     FRACTAL_OK||numeric->from_binary64(value,&converted)!=FRACTAL_OK||
    numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    numeric->real_compare(&converted,&zero,&order)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return order==FRACTAL_NUMERIC_ORDER_GREATER?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

fractal_result fractal_quadratic_initialize_v1(const fractal_numeric_vtable *numeric,
 const fractal_numeric_complex *z,const fractal_numeric_complex *c,double radius,
 fractal_formula_state_v1 *output){
 fractal_quadratic_state_v1 state={0};
 fractal_scalar converted;
 if(!numeric||!z||!c||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(output,0,sizeof(*output));
 if(numeric->from_binary64(radius,&converted)!=FRACTAL_OK||
    numeric->real_multiply(&converted,&converted,&state.radius_squared)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state.z=*z;
 state.c=*c;
 store_state(output,&state);
 return FRACTAL_OK;
}

fractal_result fractal_quadratic_recurrence_v1(const fractal_numeric_vtable *numeric,
 fractal_formula_state_v1 *value){
 fractal_quadratic_state_v1 state;
 fractal_numeric_complex squared,next;
 if(!numeric||!value)return FRACTAL_ERROR_INVALID_ARGUMENT;
 load_state(value,&state);
 if(numeric->complex_square(&state.z,&squared)!=FRACTAL_OK||
    numeric->complex_add(&squared,&state.c,&next)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state.z=next;
 store_state(value,&state);
 return FRACTAL_OK;
}

fractal_result fractal_quadratic_terminate_v1(const fractal_numeric_vtable *numeric,
 fractal_formula_state_v1 *value,bool *terminated){
 fractal_quadratic_state_v1 state;
 fractal_scalar magnitude_squared;
 bool escaped;
 if(!numeric||!value||!terminated)return FRACTAL_ERROR_INVALID_ARGUMENT;
 load_state(value,&state);
 if(numeric->squared_magnitude(&state.z,&magnitude_squared)!=FRACTAL_OK||
    numeric->bailout_exceeded(&magnitude_squared,&state.radius_squared,&escaped)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state.escaped=escaped?1u:0u;
 store_state(value,&state);
 *terminated=escaped;
 return FRACTAL_OK;
}

fractal_result fractal_quadratic_classify_v1(const fractal_numeric_vtable *numeric,
 const fractal_formula_state_v1 *value,bool budget_exhausted,
 fractal_point_class *classification){
 fractal_quadratic_state_v1 state;
 if(!numeric||!value||!classification)return FRACTAL_ERROR_INVALID_ARGUMENT;
 load_state(value,&state);
 if(state.escaped)*classification=FRACTAL_CLASS_ESCAPED;
 else if(budget_exhausted)*classification=FRACTAL_CLASS_UNRESOLVED;
 else return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static void hash_bytes(uint64_t *hash,const void *data,size_t size){
 const unsigned char *bytes=data;
 while(size--){*hash^=*bytes++;*hash*=UINT64_C(1099511628211);}
}

uint64_t fractal_quadratic_parameter_identity_v1(const char *module_id,
 const double *values,size_t count){
 char text[512];
 uint64_t bits[3]={0},hash=UINT64_C(1469598103934665603);
 int length;
 size_t i;
 if(!module_id||!values||!count||count>3u)return 0;
 for(i=0;i<count;i++)memcpy(&bits[i],&values[i],sizeof(bits[i]));
 length=snprintf(text,sizeof(text),
  "fractal.formula.parameters.v1|module=%s|count=%zu|values=%016llx,%016llx,%016llx",
  module_id,count,(unsigned long long)bits[0],(unsigned long long)bits[1],
  (unsigned long long)bits[2]);
 if(length<0||(size_t)length>=sizeof(text))return 0;
 hash_bytes(&hash,text,(size_t)length);
 return hash;
}
