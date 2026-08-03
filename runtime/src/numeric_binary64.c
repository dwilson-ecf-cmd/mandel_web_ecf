#include "fractal/numeric.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#if FLT_RADIX != 2 || DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024
#error "fractal.numeric.binary64.v1 requires IEEE binary64 double semantics"
#endif
_Static_assert(sizeof(double)==8,"fractal.numeric.binary64.v1 requires 8-byte double");

static const fractal_module_descriptor binary64_descriptor={
 FRACTAL_MODULE_ABI_VERSION,1u,FRACTAL_NUMERIC_BINARY64_V1_ID,
 "IEEE-754 Binary64 numeric contract v1",FRACTAL_MODULE_NUMERIC,
 FRACTAL_CAP_SCALAR_ARITHMETIC,true
};

static double load_real(const fractal_scalar *value){
 double result;
 memcpy(&result,value->storage,sizeof(result));
 return result;
}

static void store_real(fractal_scalar *output,double value){
 memcpy(output->storage,&value,sizeof(value));
}

static fractal_result binary64_result(double value,fractal_scalar *output){
 if(!output||!isfinite(value))return FRACTAL_ERROR_INVALID_SPEC;
 store_real(output,value);
 return FRACTAL_OK;
}

static fractal_result binary64_constant(fractal_numeric_constant_v1 constant,
 fractal_scalar *output){
 double value;
 if(!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 switch(constant){
  case FRACTAL_NUMERIC_CONSTANT_ZERO:value=0.0;break;
  case FRACTAL_NUMERIC_CONSTANT_ONE:value=1.0;break;
  case FRACTAL_NUMERIC_CONSTANT_TWO:value=2.0;break;
  case FRACTAL_NUMERIC_CONSTANT_HALF:value=0.5;break;
  default:return FRACTAL_ERROR_INVALID_ARGUMENT;
 }
 store_real(output,value);
 return FRACTAL_OK;
}

static fractal_result binary64_from_binary64(double value,fractal_scalar *output){
 if(!output||!isfinite(value))return FRACTAL_ERROR_INVALID_ARGUMENT;
 store_real(output,value);
 return FRACTAL_OK;
}

static fractal_result binary64_from_uint32(uint32_t value,fractal_scalar *output){
 if(!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 store_real(output,(double)value);
 return FRACTAL_OK;
}

static fractal_result binary64_to_binary64(const fractal_scalar *value,double *output){
 if(!value||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 *output=load_real(value);
 return FRACTAL_OK;
}

static fractal_result binary64_add(const fractal_scalar *left,
 const fractal_scalar *right,fractal_scalar *output){
 double value;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 value=load_real(left)+load_real(right);
 return binary64_result(value,output);
}

static fractal_result binary64_subtract(const fractal_scalar *left,
 const fractal_scalar *right,fractal_scalar *output){
 double value;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 value=load_real(left)-load_real(right);
 return binary64_result(value,output);
}

static fractal_result binary64_multiply(const fractal_scalar *left,
 const fractal_scalar *right,fractal_scalar *output){
 double value;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 value=load_real(left)*load_real(right);
 return binary64_result(value,output);
}

static fractal_result binary64_divide(const fractal_scalar *left,
 const fractal_scalar *right,fractal_scalar *output){
 double value;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 value=load_real(left)/load_real(right);
 return binary64_result(value,output);
}

static fractal_result binary64_compare(const fractal_scalar *left,
 const fractal_scalar *right,fractal_numeric_order_v1 *output){
 double a,b;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 a=load_real(left);b=load_real(right);
 if(!isfinite(a)||!isfinite(b))return FRACTAL_ERROR_INVALID_SPEC;
 *output=a<b?FRACTAL_NUMERIC_ORDER_LESS:
  a>b?FRACTAL_NUMERIC_ORDER_GREATER:FRACTAL_NUMERIC_ORDER_EQUAL;
 return FRACTAL_OK;
}

static fractal_result binary64_equal(const fractal_scalar *left,
 const fractal_scalar *right,bool *output){
 double a,b;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 a=load_real(left);b=load_real(right);
 if(!isfinite(a)||!isfinite(b))return FRACTAL_ERROR_INVALID_SPEC;
 *output=a==b;
 return FRACTAL_OK;
}

static bool binary64_is_finite(const fractal_scalar *value){
 return value&&isfinite(load_real(value));
}

static fractal_result binary64_complex_construct(const fractal_scalar *real,
 const fractal_scalar *imaginary,fractal_numeric_complex *output){
 if(!real||!imaginary||!output||!binary64_is_finite(real)||
    !binary64_is_finite(imaginary))return FRACTAL_ERROR_INVALID_ARGUMENT;
 output->real=*real;
 output->imaginary=*imaginary;
 return FRACTAL_OK;
}

static fractal_result binary64_complex_add(const fractal_numeric_complex *left,
 const fractal_numeric_complex *right,fractal_numeric_complex *output){
 fractal_numeric_complex result;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(binary64_add(&left->real,&right->real,&result.real)!=FRACTAL_OK||
    binary64_add(&left->imaginary,&right->imaginary,&result.imaginary)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 *output=result;
 return FRACTAL_OK;
}

static fractal_result binary64_complex_subtract(const fractal_numeric_complex *left,
 const fractal_numeric_complex *right,fractal_numeric_complex *output){
 fractal_numeric_complex result;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(binary64_subtract(&left->real,&right->real,&result.real)!=FRACTAL_OK||
    binary64_subtract(&left->imaginary,&right->imaginary,&result.imaginary)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 *output=result;
 return FRACTAL_OK;
}

static fractal_result binary64_complex_multiply(const fractal_numeric_complex *left,
 const fractal_numeric_complex *right,fractal_numeric_complex *output){
 fractal_scalar ac,bd,ad,bc;
 fractal_numeric_complex result;
 if(!left||!right||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(binary64_multiply(&left->real,&right->real,&ac)!=FRACTAL_OK||
    binary64_multiply(&left->imaginary,&right->imaginary,&bd)!=FRACTAL_OK||
    binary64_subtract(&ac,&bd,&result.real)!=FRACTAL_OK||
    binary64_multiply(&left->real,&right->imaginary,&ad)!=FRACTAL_OK||
    binary64_multiply(&left->imaginary,&right->real,&bc)!=FRACTAL_OK||
    binary64_add(&ad,&bc,&result.imaginary)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 *output=result;
 return FRACTAL_OK;
}

static fractal_result binary64_complex_square(const fractal_numeric_complex *value,
 fractal_numeric_complex *output){
 fractal_scalar real_squared,imaginary_squared,cross,two;
 fractal_numeric_complex result;
 if(!value||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(binary64_multiply(&value->real,&value->real,&real_squared)!=FRACTAL_OK||
    binary64_multiply(&value->imaginary,&value->imaginary,&imaginary_squared)!=FRACTAL_OK||
    binary64_subtract(&real_squared,&imaginary_squared,&result.real)!=FRACTAL_OK||
    binary64_multiply(&value->real,&value->imaginary,&cross)!=FRACTAL_OK||
    binary64_constant(FRACTAL_NUMERIC_CONSTANT_TWO,&two)!=FRACTAL_OK||
    binary64_multiply(&two,&cross,&result.imaginary)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 *output=result;
 return FRACTAL_OK;
}

static fractal_result binary64_squared_magnitude(const fractal_numeric_complex *value,
 fractal_scalar *output){
 fractal_scalar real_squared,imaginary_squared;
 if(!value||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(binary64_multiply(&value->real,&value->real,&real_squared)!=FRACTAL_OK||
    binary64_multiply(&value->imaginary,&value->imaginary,&imaginary_squared)!=FRACTAL_OK||
    binary64_add(&real_squared,&imaginary_squared,output)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static fractal_result binary64_bailout_exceeded(const fractal_scalar *magnitude_squared,
 const fractal_scalar *radius_squared,bool *output){
 double magnitude,radius;
 if(!magnitude_squared||!radius_squared||!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 magnitude=load_real(magnitude_squared);radius=load_real(radius_squared);
 if(!isfinite(magnitude)||!isfinite(radius))return FRACTAL_ERROR_INVALID_SPEC;
 *output=magnitude>radius;
 return FRACTAL_OK;
}

static fractal_result binary64_serialize(const fractal_scalar *value,char *output,
 size_t capacity,size_t *length){
 uint64_t bits;
 int count;
 if(!value||!length)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memcpy(&bits,value->storage,sizeof(bits));
 count=snprintf(output,capacity,"%016llx",(unsigned long long)bits);
 *length=16u;
 return count<0||!output||capacity<17u?FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

const fractal_numeric_vtable fractal_numeric_binary64_v1={
 &binary64_descriptor,FRACTAL_NUMERIC_ABI_VERSION,FRACTAL_NUMERIC_CONTRACT_VERSION,
 FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,53u,sizeof(double),_Alignof(double),
 binary64_constant,binary64_from_binary64,binary64_from_uint32,binary64_to_binary64,
 binary64_add,binary64_subtract,binary64_multiply,binary64_divide,binary64_compare,
 binary64_equal,binary64_is_finite,binary64_complex_construct,binary64_complex_add,
 binary64_complex_subtract,binary64_complex_multiply,binary64_complex_square,
 binary64_squared_magnitude,binary64_bailout_exceeded,binary64_serialize
};

fractal_result fractal_numeric_validate_v1(const fractal_numeric_vtable *numeric,
 uint64_t required_capabilities){
 if(!numeric||!numeric->descriptor)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_module_descriptor_validate(numeric->descriptor,FRACTAL_MODULE_NUMERIC)!=FRACTAL_OK||
    !numeric->descriptor->available||
    numeric->abi_version!=FRACTAL_NUMERIC_ABI_VERSION||
    numeric->contract_version!=FRACTAL_NUMERIC_CONTRACT_VERSION||
    (numeric->descriptor->capability_flags&FRACTAL_CAP_SCALAR_ARITHMETIC)==0||
    (numeric->capability_flags&required_capabilities)!=required_capabilities||
    !numeric->precision_bits||!numeric->scalar_size||
    numeric->scalar_size>sizeof(fractal_scalar)||!numeric->scalar_alignment||
    !numeric->constant||!numeric->from_binary64||!numeric->from_uint32||
    !numeric->to_binary64||!numeric->real_add||!numeric->real_subtract||
    !numeric->real_multiply||!numeric->real_divide||!numeric->real_compare||
    !numeric->real_equal||!numeric->real_is_finite||!numeric->complex_construct||
    !numeric->complex_add||!numeric->complex_subtract||!numeric->complex_multiply||
    !numeric->complex_square||!numeric->squared_magnitude||
    !numeric->bailout_exceeded||!numeric->serialize_real)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static void hash_bytes(uint64_t *hash,const void *data,size_t size){
 const unsigned char *bytes=data;
 while(size--){*hash^=*bytes++;*hash*=UINT64_C(1099511628211);}
}

uint64_t fractal_numeric_execution_identity_v1(const fractal_numeric_vtable *numeric){
 char text[512];
 uint64_t hash=UINT64_C(1469598103934665603);
 int count;
 if(fractal_numeric_validate_v1(numeric,0)!=FRACTAL_OK)return 0;
 count=snprintf(text,sizeof(text),
  "fractal.numeric.execution.v1|module=%s@%u|abi=%u|contract=%u|caps=%016llx|precision=%u|scalar=%zu|alignment=%zu",
  numeric->descriptor->module_id,numeric->descriptor->module_version,numeric->abi_version,
  numeric->contract_version,(unsigned long long)numeric->capability_flags,
  numeric->precision_bits,numeric->scalar_size,numeric->scalar_alignment);
 if(count<0||(size_t)count>=sizeof(text))return 0;
 hash_bytes(&hash,text,(size_t)count);
 return hash;
}
