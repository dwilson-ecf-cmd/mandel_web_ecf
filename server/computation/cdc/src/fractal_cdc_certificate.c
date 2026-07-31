#include "fractal_cdc_certificate.h"
#include <stdio.h>
#include <string.h>

static uint64_t bits(double value) { uint64_t result; memcpy(&result,&value,sizeof(result)); return result; }
const char *fractal_cdc_mapping_decision_string(fractal_cdc_mapping_decision d) {
 return d==FRACTAL_CDC_MAPPING_NEGATIVE_RESULT ? "NEGATIVE_RESULT" : NULL;
}
fractal_result fractal_cdc_experiment_0_certificate_create(fractal_cdc_experiment_0_certificate *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c)); c->present_fields=FRACTAL_CDC_EXPERIMENT_0_REQUIRED_FIELDS;
 c->formula=FRACTAL_FORMULA_MANDELBROT_QUADRATIC; c->numeric_representation=FRACTAL_NUMERIC_BINARY64;
 c->c_real=2.0; c->escape_radius=2.0; c->trace_real[0]=0.0; c->trace_real[1]=2.0; c->trace_real[2]=6.0;
 c->trace_steps[0]=0u; c->trace_steps[1]=1u; c->trace_steps[2]=2u; c->strict_escape=true;
 c->conventional_iteration_count=2u; c->mapping_decision=FRACTAL_CDC_MAPPING_NEGATIVE_RESULT;
 return FRACTAL_OK;
}
fractal_result fractal_cdc_experiment_0_certificate_validate(const fractal_cdc_experiment_0_certificate *c) {
 static const uint64_t expected_real[3]={UINT64_C(0),UINT64_C(0x4000000000000000),UINT64_C(0x4018000000000000)};
 size_t i;
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(c->present_fields!=FRACTAL_CDC_EXPERIMENT_0_REQUIRED_FIELDS || c->formula!=FRACTAL_FORMULA_MANDELBROT_QUADRATIC ||
    c->numeric_representation!=FRACTAL_NUMERIC_BINARY64 || bits(c->c_real)!=expected_real[1] || bits(c->c_imaginary)!=0u ||
    bits(c->initial_z_real)!=0u || bits(c->initial_z_imaginary)!=0u || bits(c->escape_radius)!=expected_real[1] ||
    !c->strict_escape || c->conventional_iteration_count!=2u || c->cdc_descent_step_count!=0u ||
    c->mapping_decision!=FRACTAL_CDC_MAPPING_NEGATIVE_RESULT)return FRACTAL_ERROR_INVALID_SPEC;
 for(i=0;i<3u;++i)if(bits(c->trace_real[i])!=expected_real[i] || bits(c->trace_imaginary[i])!=0u || c->trace_steps[i]!=i)return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}
fractal_result fractal_cdc_experiment_0_certificate_serialize(const fractal_cdc_experiment_0_certificate *c,char *buffer,size_t capacity,size_t *length) {
 int n;
 if(!length)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_experiment_0_certificate_validate(c)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 n=snprintf(buffer,capacity,"{\"experiment\":0,\"formula\":\"z(n+1)=z(n)^2+c\",\"c_bits\":\"4000000000000000,0000000000000000\",\"radius_bits\":\"4000000000000000\",\"trace_bits\":[\"0000000000000000\",\"4000000000000000\",\"4018000000000000\"],\"strict_escape\":true,\"conventional_iterations\":2,\"cdc_descent_steps\":0,\"mapping_decision\":\"NEGATIVE_RESULT\"}");
 if(n<0)return FRACTAL_ERROR_INVALID_SPEC;
 *length=(size_t)n;
 if(!buffer || capacity<=(size_t)n)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 return FRACTAL_OK;
}
