#ifndef FRACTAL_CDC_REFINEMENT_CONTRACT_H
#define FRACTAL_CDC_REFINEMENT_CONTRACT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal_cdc_region_study.h"

#define FRACTAL_CDC_REFINEMENT_CONTRACT_VERSION "cdc-refinement-contract-v1"

typedef enum fractal_cdc_contract_kind {
 FRACTAL_CDC_CONTRACT_NONE=0,
 FRACTAL_CDC_CONTRACT_MAX_DEPTH=1,
 FRACTAL_CDC_CONTRACT_MIN_REAL_WIDTH=2,
 FRACTAL_CDC_CONTRACT_FIXED_TOKEN_ALLOWANCE=3
} fractal_cdc_contract_kind;

typedef enum fractal_cdc_exhaustion_policy {
 FRACTAL_CDC_EXHAUST_UNRESOLVED=0,
 FRACTAL_CDC_EXHAUST_CONTRACT_EXHAUSTED=1,
 FRACTAL_CDC_EXHAUST_FALLBACK_REQUIRED=2
} fractal_cdc_exhaustion_policy;

typedef enum fractal_cdc_obligation_status {
 FRACTAL_CDC_OBLIGATION_UNRESOLVED=0,
 FRACTAL_CDC_OBLIGATION_DISCHARGED=1,
 FRACTAL_CDC_OBLIGATION_CONTRACT_EXHAUSTED=2,
 FRACTAL_CDC_OBLIGATION_FALLBACK_REQUIRED=3,
 FRACTAL_CDC_OBLIGATION_FAILED=4,
 FRACTAL_CDC_OBLIGATION_NUMERICALLY_UNCERTAIN=5
} fractal_cdc_obligation_status;

typedef struct fractal_cdc_refinement_contract {
 fractal_cdc_contract_kind kind;
 fractal_cdc_region target;
 uint8_t maximum_refinement_depth;
 double minimum_real_width;
 uint8_t token_allowance;
 fractal_cdc_exhaustion_policy exhaustion_policy;
 uint64_t identity;
} fractal_cdc_refinement_contract;

typedef struct fractal_cdc_refinement_obligation {
 fractal_cdc_region region;
 uint8_t lineage_depth;
 uint8_t creation_rank;
 uint64_t contract_identity;
 fractal_cdc_obligation_status status;
} fractal_cdc_refinement_obligation;

fractal_result fractal_cdc_contract_create_max_depth(fractal_cdc_refinement_contract *contract,
 const fractal_cdc_region *target,uint8_t maximum_depth,fractal_cdc_exhaustion_policy policy);
fractal_result fractal_cdc_contract_create_min_real_width(fractal_cdc_refinement_contract *contract,
 const fractal_cdc_region *target,double minimum_width,fractal_cdc_exhaustion_policy policy);
fractal_result fractal_cdc_contract_create_fixed_tokens(fractal_cdc_refinement_contract *contract,
 const fractal_cdc_region *target,uint8_t allowance,fractal_cdc_exhaustion_policy policy);
fractal_result fractal_cdc_contract_validate(const fractal_cdc_refinement_contract *contract);
fractal_result fractal_cdc_contract_serialize(const fractal_cdc_refinement_contract *contract,
 char *buffer,size_t capacity,size_t *length);
fractal_result fractal_cdc_contract_derive_rank(const fractal_cdc_refinement_contract *contract,
 const fractal_cdc_region *region,uint8_t lineage_depth,uint8_t *rank);
fractal_result fractal_cdc_obligation_create(const fractal_cdc_refinement_contract *contract,
 const fractal_cdc_region *region,uint8_t lineage_depth,fractal_cdc_refinement_obligation *obligation);
fractal_result fractal_cdc_obligation_can_split(const fractal_cdc_refinement_contract *contract,
 const fractal_cdc_refinement_obligation *obligation,bool *allowed);
fractal_result fractal_cdc_obligation_finish(const fractal_cdc_refinement_contract *contract,
 fractal_cdc_refinement_obligation *obligation,bool guard_verified);
fractal_result fractal_cdc_contract_extend(const fractal_cdc_refinement_contract *old_contract,
 uint8_t new_allowance,fractal_cdc_refinement_contract *new_contract);
#endif
