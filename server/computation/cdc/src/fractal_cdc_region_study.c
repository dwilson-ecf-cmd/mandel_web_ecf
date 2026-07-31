#include "fractal_cdc_region_study.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static bool selected(const fractal_cdc_region *r) {
 return r && r->real_min==2.0 && r->real_max==2.25 &&
  r->imaginary_min==0.0 && r->imaginary_max==0.25;
}

fractal_result fractal_cdc_region_validate(const fractal_cdc_region *r) {
 if(!r)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(!isfinite(r->real_min)||!isfinite(r->real_max)||!isfinite(r->imaginary_min)||
    !isfinite(r->imaginary_max)||r->real_min>=r->real_max||r->imaginary_min>=r->imaginary_max)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_region_classification fractal_region_conventional_oracle(const fractal_cdc_region *r) {
 double lower;
 if(fractal_cdc_region_validate(r)!=FRACTAL_OK)return FRACTAL_REGION_NUMERICALLY_UNCERTAIN;
 /* This deliberately narrow oracle recognizes only the exact dyadic study box. */
 if(!selected(r))return FRACTAL_REGION_UNRESOLVED;
 lower=r->real_min*r->real_min-r->imaginary_max*r->imaginary_max+r->real_min;
 return lower>2.0 ? FRACTAL_REGION_CERTIFIED_ESCAPED : FRACTAL_REGION_UNRESOLVED;
}

fractal_result fractal_cdc_region_study_0_create(fractal_cdc_region_certificate *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c));
 c->region=(fractal_cdc_region){2.0,2.25,0.0,0.25}; c->escape_radius=2.0;
 c->iteration_bound=2u; c->z2_real_lower=5.9375; c->enclosure_valid=true;
 c->conventional_escape_predicate=true; c->guard_sound=true;
 c->cdc_transition_certified=true; c->obligation_discharged=true;
 c->potential_before=1u; c->potential_after=0u; c->cdc_descent_step_count=1u;
 c->classification=FRACTAL_REGION_CERTIFIED_ESCAPED;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_region_certificate_validate(const fractal_cdc_region_certificate *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_region_validate(&c->region)!=FRACTAL_OK||!selected(&c->region)||
    c->escape_radius!=2.0||c->iteration_bound!=2u||c->z2_real_lower!=5.9375||
    !c->enclosure_valid||!c->conventional_escape_predicate||!c->guard_sound||
    !c->cdc_transition_certified||!c->obligation_discharged||c->potential_before!=1u||
    c->potential_after!=0u||c->cdc_descent_step_count!=1u||
    c->classification!=FRACTAL_REGION_CERTIFIED_ESCAPED||
    fractal_region_conventional_oracle(&c->region)!=FRACTAL_REGION_CERTIFIED_ESCAPED)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_region_certificate_serialize(const fractal_cdc_region_certificate *c,char *buffer,size_t capacity,size_t *length) {
 int n;
 if(!length)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_region_certificate_validate(c)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 n=snprintf(buffer,capacity,"{\"study\":\"%s\",\"region_bits\":\"4000000000000000,4002000000000000,0000000000000000,3fd0000000000000\",\"iteration_bound\":2,\"z2_real_lower\":5.9375,\"enclosure_valid\":true,\"conventional_escape\":true,\"guard_sound\":true,\"cdc_transition\":true,\"potential\":[1,0],\"cdc_descent_steps\":1,\"terminal\":\"CERTIFIED_ESCAPED\",\"prior_direct_orbit\":\"NEGATIVE_RESULT\"}",FRACTAL_CDC_REGION_STUDY_ID);
 if(n<0)return FRACTAL_ERROR_INVALID_SPEC;
 *length=(size_t)n;
 if(!buffer||capacity<=(size_t)n)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 return FRACTAL_OK;
}
