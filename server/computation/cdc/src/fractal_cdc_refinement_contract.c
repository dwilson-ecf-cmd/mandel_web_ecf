#include "fractal_cdc_refinement_contract.h"
#include <stdio.h>
#include <string.h>

static uint64_t bits(double value) { uint64_t result; memcpy(&result,&value,sizeof(result)); return result; }
static void hash_byte(uint64_t *h,uint8_t b) { *h^=b; *h*=UINT64_C(1099511628211); }
static void hash_u64(uint64_t *h,uint64_t value) { unsigned i; for(i=0;i<8u;++i)hash_byte(h,(uint8_t)(value>>(i*8u))); }
static uint64_t identity_of(const fractal_cdc_refinement_contract *c) {
 uint64_t h=UINT64_C(1469598103934665603); unsigned i; const char *v=FRACTAL_CDC_REFINEMENT_CONTRACT_VERSION;
 for(i=0u;v[i]!='\0';++i)hash_byte(&h,(uint8_t)v[i]);
 hash_byte(&h,(uint8_t)c->kind); hash_u64(&h,bits(c->target.real_min)); hash_u64(&h,bits(c->target.real_max));
 hash_u64(&h,bits(c->target.imaginary_min)); hash_u64(&h,bits(c->target.imaginary_max));
 hash_byte(&h,c->maximum_refinement_depth); hash_u64(&h,bits(c->minimum_real_width));
 hash_byte(&h,c->token_allowance); hash_byte(&h,(uint8_t)c->exhaustion_policy); return h;
}
static bool registered_target(const fractal_cdc_region *r) {
 return r->real_min==2.0&&r->real_max==2.25&&r->imaginary_min==0.0&&r->imaginary_max==0.25;
}
static bool contained(const fractal_cdc_region *outer,const fractal_cdc_region *inner) {
 return inner->real_min>=outer->real_min&&inner->real_max<=outer->real_max&&
  inner->imaginary_min>=outer->imaginary_min&&inner->imaginary_max<=outer->imaginary_max;
}
static fractal_result finish_create(fractal_cdc_refinement_contract *c) { c->identity=identity_of(c); return fractal_cdc_contract_validate(c); }
fractal_result fractal_cdc_contract_create_max_depth(fractal_cdc_refinement_contract *c,const fractal_cdc_region *t,uint8_t d,fractal_cdc_exhaustion_policy p) {
 if(!c||!t)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c)); c->kind=FRACTAL_CDC_CONTRACT_MAX_DEPTH;c->target=*t;c->maximum_refinement_depth=d;c->exhaustion_policy=p;return finish_create(c);
}
fractal_result fractal_cdc_contract_create_min_real_width(fractal_cdc_refinement_contract *c,const fractal_cdc_region *t,double w,fractal_cdc_exhaustion_policy p) {
 if(!c||!t)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c));c->kind=FRACTAL_CDC_CONTRACT_MIN_REAL_WIDTH;c->target=*t;c->minimum_real_width=w;c->exhaustion_policy=p;return finish_create(c);
}
fractal_result fractal_cdc_contract_create_fixed_tokens(fractal_cdc_refinement_contract *c,const fractal_cdc_region *t,uint8_t a,fractal_cdc_exhaustion_policy p) {
 if(!c||!t)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c));c->kind=FRACTAL_CDC_CONTRACT_FIXED_TOKEN_ALLOWANCE;c->target=*t;c->token_allowance=a;c->exhaustion_policy=p;return finish_create(c);
}
fractal_result fractal_cdc_contract_validate(const fractal_cdc_refinement_contract *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_region_validate(&c->target)!=FRACTAL_OK||!registered_target(&c->target)||c->exhaustion_policy>FRACTAL_CDC_EXHAUST_FALLBACK_REQUIRED)return FRACTAL_ERROR_INVALID_SPEC;
 if(c->kind==FRACTAL_CDC_CONTRACT_MAX_DEPTH) { if(c->maximum_refinement_depth>2u||c->minimum_real_width!=0.0||c->token_allowance!=0u)return FRACTAL_ERROR_INVALID_SPEC; }
 else if(c->kind==FRACTAL_CDC_CONTRACT_MIN_REAL_WIDTH) { if(c->minimum_real_width!=0.0625||c->maximum_refinement_depth!=0u||c->token_allowance!=0u)return FRACTAL_ERROR_INVALID_SPEC; }
 else if(c->kind==FRACTAL_CDC_CONTRACT_FIXED_TOKEN_ALLOWANCE) { if(c->token_allowance>2u||c->maximum_refinement_depth!=0u||c->minimum_real_width!=0.0)return FRACTAL_ERROR_INVALID_SPEC; }
 else return FRACTAL_ERROR_INVALID_SPEC;
 return c->identity==identity_of(c)?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}
fractal_result fractal_cdc_contract_serialize(const fractal_cdc_refinement_contract *c,char *buffer,size_t capacity,size_t *length) {
 int n;if(!length)return FRACTAL_ERROR_INVALID_ARGUMENT;if(fractal_cdc_contract_validate(c)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 n=snprintf(buffer,capacity,"{\"version\":\"%s\",\"kind\":%u,\"target_bits\":\"%016llx,%016llx,%016llx,%016llx\",\"maximum_depth\":%u,\"minimum_real_width_bits\":\"%016llx\",\"token_allowance\":%u,\"exhaustion_policy\":%u,\"identity\":\"%016llx\"}",FRACTAL_CDC_REFINEMENT_CONTRACT_VERSION,(unsigned)c->kind,(unsigned long long)bits(c->target.real_min),(unsigned long long)bits(c->target.real_max),(unsigned long long)bits(c->target.imaginary_min),(unsigned long long)bits(c->target.imaginary_max),(unsigned)c->maximum_refinement_depth,(unsigned long long)bits(c->minimum_real_width),(unsigned)c->token_allowance,(unsigned)c->exhaustion_policy,(unsigned long long)c->identity);
 if(n<0)return FRACTAL_ERROR_INVALID_SPEC;
 *length=(size_t)n;
 if(!buffer||capacity<=(size_t)n)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 return FRACTAL_OK;
}
fractal_result fractal_cdc_contract_derive_rank(const fractal_cdc_refinement_contract *c,const fractal_cdc_region *r,uint8_t depth,uint8_t *rank) {
 double width;uint8_t remaining=0u;if(!r||!rank)return FRACTAL_ERROR_INVALID_ARGUMENT;if(fractal_cdc_contract_validate(c)!=FRACTAL_OK||fractal_cdc_region_validate(r)!=FRACTAL_OK||!contained(&c->target,r))return FRACTAL_ERROR_INVALID_SPEC;
 if(c->kind==FRACTAL_CDC_CONTRACT_MAX_DEPTH) { if(depth>c->maximum_refinement_depth)return FRACTAL_ERROR_INVALID_SPEC;remaining=(uint8_t)(c->maximum_refinement_depth-depth); }
 else if(c->kind==FRACTAL_CDC_CONTRACT_FIXED_TOKEN_ALLOWANCE) { if(depth>c->token_allowance)return FRACTAL_ERROR_INVALID_SPEC;remaining=(uint8_t)(c->token_allowance-depth); }
 else { width=r->real_max-r->real_min;while(width>c->minimum_real_width) { if(remaining==UINT8_MAX)return FRACTAL_ERROR_INVALID_SPEC;width*=0.5;++remaining; } if(width!=c->minimum_real_width)return FRACTAL_ERROR_INVALID_SPEC; }
 *rank=remaining;return FRACTAL_OK;
}
fractal_result fractal_cdc_obligation_create(const fractal_cdc_refinement_contract *c,const fractal_cdc_region *r,uint8_t depth,fractal_cdc_refinement_obligation *o) {
 uint8_t rank;if(!o)return FRACTAL_ERROR_INVALID_ARGUMENT;if(fractal_cdc_contract_derive_rank(c,r,depth,&rank)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;memset(o,0,sizeof(*o));o->region=*r;o->lineage_depth=depth;o->creation_rank=rank;o->contract_identity=c->identity;o->status=FRACTAL_CDC_OBLIGATION_UNRESOLVED;return FRACTAL_OK;
}
static fractal_result matching(const fractal_cdc_refinement_contract *c,const fractal_cdc_refinement_obligation *o) { uint8_t rank;if(!o||fractal_cdc_contract_validate(c)!=FRACTAL_OK||o->contract_identity!=c->identity||fractal_cdc_contract_derive_rank(c,&o->region,o->lineage_depth,&rank)!=FRACTAL_OK||rank!=o->creation_rank)return FRACTAL_ERROR_INVALID_SPEC;return FRACTAL_OK; }
fractal_result fractal_cdc_obligation_can_split(const fractal_cdc_refinement_contract *c,const fractal_cdc_refinement_obligation *o,bool *allowed) { if(!allowed)return FRACTAL_ERROR_INVALID_ARGUMENT;if(matching(c,o)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;*allowed=o->status==FRACTAL_CDC_OBLIGATION_UNRESOLVED&&o->creation_rank>0u;return FRACTAL_OK; }
fractal_result fractal_cdc_obligation_finish(const fractal_cdc_refinement_contract *c,fractal_cdc_refinement_obligation *o,bool guard) { if(matching(c,o)!=FRACTAL_OK||o->status!=FRACTAL_CDC_OBLIGATION_UNRESOLVED)return FRACTAL_ERROR_INVALID_SPEC;if(guard)o->status=FRACTAL_CDC_OBLIGATION_DISCHARGED;else if(o->creation_rank==0u)o->status=c->exhaustion_policy==FRACTAL_CDC_EXHAUST_FALLBACK_REQUIRED?FRACTAL_CDC_OBLIGATION_FALLBACK_REQUIRED:(c->exhaustion_policy==FRACTAL_CDC_EXHAUST_CONTRACT_EXHAUSTED?FRACTAL_CDC_OBLIGATION_CONTRACT_EXHAUSTED:FRACTAL_CDC_OBLIGATION_UNRESOLVED);return FRACTAL_OK; }
fractal_result fractal_cdc_contract_extend(const fractal_cdc_refinement_contract *old,uint8_t allowance,fractal_cdc_refinement_contract *next) { if(!next)return FRACTAL_ERROR_INVALID_ARGUMENT;if(fractal_cdc_contract_validate(old)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;if(old->kind==FRACTAL_CDC_CONTRACT_MIN_REAL_WIDTH||allowance<=(old->kind==FRACTAL_CDC_CONTRACT_MAX_DEPTH?old->maximum_refinement_depth:old->token_allowance))return FRACTAL_ERROR_INVALID_SPEC;return old->kind==FRACTAL_CDC_CONTRACT_MAX_DEPTH?fractal_cdc_contract_create_max_depth(next,&old->target,allowance,old->exhaustion_policy):fractal_cdc_contract_create_fixed_tokens(next,&old->target,allowance,old->exhaustion_policy); }
