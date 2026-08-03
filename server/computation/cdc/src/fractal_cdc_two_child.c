#include "fractal_cdc_two_child.h"
#include <stdio.h>
#include <string.h>

static bool same_region(const fractal_cdc_region *a,const fractal_cdc_region *b) {
 return a->real_min==b->real_min && a->real_max==b->real_max &&
  a->imaginary_min==b->imaginary_min && a->imaginary_max==b->imaginary_max;
}

bool fractal_cdc_two_child_multiset_descends(uint64_t p,uint64_t l,uint64_t r) {
 /* Проверенное узкое кодирование: единицы площади 2 -> 1,1 и веса 3^rank: 3 -> 1+1. */
 return p==2u && l==1u && r==1u;
}

fractal_result fractal_cdc_two_child_discharge(uint32_t before,uint32_t guards,
 uint32_t *after,uint64_t *potential) {
 if(!after||!potential)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if((before & ~UINT32_C(3))!=0u || (guards & ~UINT32_C(3))!=0u)return FRACTAL_ERROR_INVALID_SPEC;
 *after=before & ~guards;
 *potential=(uint64_t)((*after & 1u)!=0u)+(uint64_t)((*after & 2u)!=0u);
 return FRACTAL_OK;
}

fractal_result fractal_cdc_two_child_split_create(fractal_cdc_two_child_split *s) {
 if(!s)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(s,0,sizeof(*s));
 s->parent=(fractal_cdc_region){2.0,2.25,0.0,0.25};
 s->left=(fractal_cdc_region){2.0,2.125,0.0,0.25};
 s->right=(fractal_cdc_region){2.125,2.25,0.0,0.25};
 s->axis=FRACTAL_CDC_SPLIT_REAL; s->split_value=2.125;
 s->parent_area_units=2u; s->left_area_units=1u; s->right_area_units=1u;
 s->potential_before=3u; s->potential_after=2u;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_two_child_split_validate(const fractal_cdc_two_child_split *s) {
 fractal_cdc_two_child_split expected;
 if(!s)return FRACTAL_ERROR_INVALID_ARGUMENT;
 fractal_cdc_two_child_split_create(&expected);
 if(fractal_cdc_region_validate(&s->parent)!=FRACTAL_OK||
    fractal_cdc_region_validate(&s->left)!=FRACTAL_OK||fractal_cdc_region_validate(&s->right)!=FRACTAL_OK||
    !same_region(&s->parent,&expected.parent)||!same_region(&s->left,&expected.left)||
    !same_region(&s->right,&expected.right)||s->axis!=FRACTAL_CDC_SPLIT_REAL||
    s->split_value!=s->left.real_max||s->split_value!=s->right.real_min||
    s->left.real_min!=s->parent.real_min||s->right.real_max!=s->parent.real_max||
    s->left.imaginary_min!=s->parent.imaginary_min||s->left.imaginary_max!=s->parent.imaginary_max||
    s->right.imaginary_min!=s->parent.imaginary_min||s->right.imaginary_max!=s->parent.imaginary_max||
    !fractal_cdc_two_child_multiset_descends(s->parent_area_units,s->left_area_units,s->right_area_units)||
    s->potential_before!=3u||s->potential_after!=2u)return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_two_child_certificate_create(fractal_cdc_two_child_certificate *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c)); fractal_cdc_two_child_split_create(&c->split);
 c->child_z2_real_lower[0]=5.9375; c->child_z2_real_lower[1]=6.46875;
 c->coverage_valid=true; c->interiors_disjoint=true; c->children_strictly_smaller=true;
 c->child_escape_guard[0]=c->child_escape_guard[1]=true;
 c->child_discharged[0]=c->child_discharged[1]=true;
 c->sequential_potential[0]=3u; c->sequential_potential[1]=2u;
 c->sequential_potential[2]=1u; c->sequential_potential[3]=0u;
 c->batch_potential[0]=3u; c->batch_potential[1]=2u; c->batch_potential[2]=0u;
 c->cdc_descent_step_count=3u; c->parent_fully_resolved=true;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_two_child_certificate_validate(const fractal_cdc_two_child_certificate *c) {
 static const uint64_t sequential[4]={3u,2u,1u,0u},batch[3]={3u,2u,0u};
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_two_child_split_validate(&c->split)!=FRACTAL_OK||
    c->child_z2_real_lower[0]!=5.9375||c->child_z2_real_lower[1]!=6.46875||
    !c->coverage_valid||!c->interiors_disjoint||!c->children_strictly_smaller||
    !c->child_escape_guard[0]||!c->child_escape_guard[1]||
    !c->child_discharged[0]||!c->child_discharged[1]||
    memcmp(c->sequential_potential,sequential,sizeof(sequential))!=0||
    memcmp(c->batch_potential,batch,sizeof(batch))!=0||c->cdc_descent_step_count!=3u||
    !c->parent_fully_resolved)return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_two_child_certificate_serialize(const fractal_cdc_two_child_certificate *c,char *buffer,size_t capacity,size_t *length) {
 int n;
 if(!length)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_two_child_certificate_validate(c)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 n=snprintf(buffer,capacity,"{\"study\":\"%s\",\"decision\":\"%s\",\"parent_bits\":\"4000000000000000,4002000000000000,0000000000000000,3fd0000000000000\",\"split\":{\"rule\":\"CDC-TWO-CHILD-SPLIT-V1\",\"axis\":\"real\",\"value_bits\":\"4001000000000000\",\"boundary\":\"left-closed-right-open-except-parent-maximum\",\"coverage\":\"EXACT\",\"interiors_disjoint\":true},\"child_bits\":[\"4000000000000000,4001000000000000,0000000000000000,3fd0000000000000\",\"4001000000000000,4002000000000000,0000000000000000,3fd0000000000000\"],\"area_units\":[2,1,1],\"multiset_potential\":[3,2],\"guards\":[\"TWO-CHILD-ESCAPE-LEFT-V1\",\"TWO-CHILD-ESCAPE-RIGHT-V1\"],\"discharges\":[\"CDC-DISCHARGE-LEFT-V1\",\"CDC-DISCHARGE-RIGHT-V1\"],\"sequential_trace\":[3,2,1,0],\"batch_trace\":[3,2,0],\"cdc_descent_steps\":3,\"terminal\":\"PARENT_FULLY_RESOLVED\",\"prior_direct_orbit\":\"NEGATIVE_RESULT\",\"object_of_descent\":\"COMPOSITE_OBJECT_REQUIRED\"}",FRACTAL_CDC_TWO_CHILD_STUDY_ID,FRACTAL_CDC_TWO_CHILD_DECISION);
 if(n<0)return FRACTAL_ERROR_INVALID_SPEC;
 *length=(size_t)n;
 if(!buffer||capacity<=(size_t)n)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 return FRACTAL_OK;
}
