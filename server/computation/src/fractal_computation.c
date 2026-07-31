#include "fractal_computation.h"
#include "fractal_cdc_computation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static fractal_result conventional_compute(const fractal_computation_problem *, const fractal_point_input *, const fractal_computation_cancellation *, fractal_point_result *);
static fractal_result unavailable_compute(const fractal_computation_problem *p, const fractal_point_input *i, const fractal_computation_cancellation *c, fractal_point_result *r) { (void)p;(void)i;(void)c;(void)r; return FRACTAL_ERROR_NOT_IMPLEMENTED; }
static const fractal_computation_vtable conventional_table={fractal_computation_problem_validate,conventional_compute};
static const fractal_computation_vtable legacy_table={fractal_computation_problem_validate,unavailable_compute};
const char *fractal_computation_backend_string(fractal_computation_backend_kind kind) {
 switch(kind) { case FRACTAL_COMPUTATION_BACKEND_LEGACY_REFERENCE:return "legacy-reference"; case FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C:return "conventional-c"; case FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL:return "cdc-experimental"; default:return NULL; }
}
const char *fractal_point_classification_string(fractal_point_classification c) {
 switch(c) { case FRACTAL_POINT_ESCAPED:return "escaped"; case FRACTAL_POINT_PROVEN_BOUNDED:return "proven-bounded"; case FRACTAL_POINT_UNRESOLVED:return "unresolved"; case FRACTAL_POINT_NUMERICALLY_UNCERTAIN:return "numerically-uncertain"; case FRACTAL_POINT_CANCELLED:return "cancelled"; case FRACTAL_POINT_FAILED:return "failed"; default:return NULL; }
}
fractal_result fractal_computation_problem_init_experiment_0(fractal_computation_problem *p) {
 if (!p) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(p,0,sizeof(*p)); p->formula=FRACTAL_FORMULA_MANDELBROT_QUADRATIC;
 p->numeric_representation=FRACTAL_NUMERIC_BINARY64; p->escape_radius=2.0; p->iteration_budget=4u;
 return FRACTAL_OK;
}
fractal_result fractal_computation_problem_validate(const fractal_computation_problem *p) {
 if(!p)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(p->formula!=FRACTAL_FORMULA_MANDELBROT_QUADRATIC || p->numeric_representation!=FRACTAL_NUMERIC_BINARY64 || !isfinite(p->initial_z_real) || !isfinite(p->initial_z_imaginary) || !isfinite(p->escape_radius) || p->escape_radius<=0.0 || p->iteration_budget==0u) return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}
fractal_result fractal_computation_backend_create(fractal_computation_backend_kind kind, fractal_computation_backend *b) {
 const char *id=fractal_computation_backend_string(kind); if(!b || !id)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(b,0,sizeof(*b)); b->metadata.kind=kind; strcpy(b->metadata.stable_identifier,id); strcpy(b->metadata.revision,"substrate-v1");
 if(kind==FRACTAL_COMPUTATION_BACKEND_CDC_EXPERIMENTAL)return fractal_cdc_computation_backend_create(b);
 b->vtable=kind==FRACTAL_COMPUTATION_BACKEND_CONVENTIONAL_C?&conventional_table:&legacy_table; return FRACTAL_OK;
}
fractal_result fractal_computation_backend_validate(const fractal_computation_backend *b) {
 if (!b || !fractal_computation_backend_string(b->metadata.kind) || !b->vtable ||
     !b->vtable->validate_problem || !b->vtable->compute_point ||
     b->metadata.stable_identifier[0]=='\0' || b->metadata.revision[0]=='\0')
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 return FRACTAL_OK;
}
static fractal_result conventional_compute(const fractal_computation_problem *p,const fractal_point_input *in,const fractal_computation_cancellation *cancel,fractal_point_result *out) {
 uint64_t n; double zr,zi,r2;
 if(!in || !out || fractal_computation_problem_validate(p)!=FRACTAL_OK || !isfinite(in->c_real) || !isfinite(in->c_imaginary))return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out)); out->classification=FRACTAL_POINT_UNRESOLVED; out->precision_used=p->numeric_representation; zr=p->initial_z_real; zi=p->initial_z_imaginary; out->trace[0]=(fractal_point_trace_entry){0u,zr,zi}; out->trace_count=1u;
 for(n=1u;n<=p->iteration_budget;++n) {
  double next_real, next_imaginary;
  if(cancel && cancel->requested) { out->classification=FRACTAL_POINT_CANCELLED; strcpy(out->failure_reason,"cancellation requested"); return FRACTAL_OK; }
  next_real=zr*zr-zi*zi+in->c_real; next_imaginary=2.0*zr*zi+in->c_imaginary;
  if(!isfinite(next_real)||!isfinite(next_imaginary)) { out->classification=FRACTAL_POINT_NUMERICALLY_UNCERTAIN; strcpy(out->failure_reason,"binary64 state became non-finite before escape evidence was recorded"); return FRACTAL_OK; }
  zr=next_real; zi=next_imaginary; out->conventional_iteration_count=n; if(out->trace_count<FRACTAL_POINT_TRACE_CAPACITY)out->trace[out->trace_count++]=(fractal_point_trace_entry){n,zr,zi};
  r2=zr*zr+zi*zi; if(!isfinite(r2)) { out->classification=FRACTAL_POINT_NUMERICALLY_UNCERTAIN; strcpy(out->failure_reason,"binary64 magnitude overflowed"); return FRACTAL_OK; }
  if(r2>p->escape_radius*p->escape_radius) { out->classification=FRACTAL_POINT_ESCAPED; strcpy(out->evidence_identity,"escape-radius-strict-greater"); break; }
 }
 out->final_z_real=zr; out->final_z_imaginary=zi;
 if(out->classification!=FRACTAL_POINT_ESCAPED) { out->classification=FRACTAL_POINT_UNRESOLVED; strcpy(out->failure_reason,"iteration budget exhausted without escape; boundedness is not proven"); }
 (void)snprintf(out->deterministic_identity,sizeof(out->deterministic_identity),"mandelbrot-binary64-%llu",(unsigned long long)out->conventional_iteration_count); return FRACTAL_OK;
}
