#include "fractal_cdc_depth_two.h"
#include <stdio.h>
#include <string.h>

static bool region_equal(const fractal_cdc_region *a,const fractal_cdc_region *b) {
 return a->real_min==b->real_min && a->real_max==b->real_max &&
  a->imaginary_min==b->imaginary_min && a->imaginary_max==b->imaginary_max;
}

fractal_result fractal_cdc_depth_two_potential(const uint8_t *ranks,size_t count,uint64_t *value) {
 size_t i; uint64_t sum=0u;
 if((count!=0u&&!ranks)||!value)return FRACTAL_ERROR_INVALID_ARGUMENT;
 for(i=0u;i<count;++i) {
  uint64_t weight=1u; uint8_t j;
  for(j=0u;j<ranks[i];++j) {
   if(weight>UINT64_MAX/3u)return FRACTAL_ERROR_INVALID_SPEC;
   weight*=3u;
  }
  if(sum>UINT64_MAX-weight)return FRACTAL_ERROR_INVALID_SPEC;
  sum+=weight;
 }
 *value=sum; return FRACTAL_OK;
}

fractal_result fractal_cdc_depth_two_tree_create(fractal_cdc_depth_two_tree *t) {
 if(!t)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(t,0,sizeof(*t));
 t->region[0]=(fractal_cdc_region){2.0,2.25,0.0,0.25};
 t->region[1]=(fractal_cdc_region){2.0,2.125,0.0,0.25};
 t->region[2]=(fractal_cdc_region){2.125,2.25,0.0,0.25};
 t->region[3]=(fractal_cdc_region){2.0,2.0625,0.0,0.25};
 t->region[4]=(fractal_cdc_region){2.0625,2.125,0.0,0.25};
 t->rank[0]=2u; t->rank[1]=1u; t->rank[2]=1u;
 t->rank[3]=0u; t->rank[4]=0u;
 t->parent[0]=UINT8_MAX; t->parent[1]=0u; t->parent[2]=0u;
 t->parent[3]=1u; t->parent[4]=1u;
 t->split_1=2.125; t->split_2=2.0625;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_depth_two_tree_validate(const fractal_cdc_depth_two_tree *t) {
 fractal_cdc_depth_two_tree e; size_t i;
 if(!t)return FRACTAL_ERROR_INVALID_ARGUMENT;
 fractal_cdc_depth_two_tree_create(&e);
 for(i=0u;i<5u;++i) if(fractal_cdc_region_validate(&t->region[i])!=FRACTAL_OK||
  !region_equal(&t->region[i],&e.region[i])||t->rank[i]!=e.rank[i]||t->parent[i]!=e.parent[i])
  return FRACTAL_ERROR_INVALID_SPEC;
 if(t->split_1!=e.split_1||t->split_2!=e.split_2||
  t->rank[1]>=t->rank[0]||t->rank[2]>=t->rank[0]||
  t->rank[3]>=t->rank[1]||t->rank[4]>=t->rank[1])return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_depth_two_certificate_create(fractal_cdc_depth_two_certificate *c) {
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c)); fractal_cdc_depth_two_tree_create(&c->tree);
 c->split_potential[0]=9u; c->split_potential[1]=6u; c->split_potential[2]=5u;
 { const uint64_t s[6]={9u,6u,5u,4u,3u,0u}; memcpy(c->sequential_potential,s,sizeof(s)); }
 { const uint64_t b[4]={9u,6u,5u,0u}; memcpy(c->batch_potential,b,sizeof(b)); }
 c->coverage[0]=c->coverage[1]=true; c->disjoint_interiors[0]=c->disjoint_interiors[1]=true;
 c->locally_ranked[0]=c->locally_ranked[1]=true;
 c->ranks_immutable=true; c->future_independent=true;
 c->conventional_guards[0]=c->conventional_guards[1]=c->conventional_guards[2]=true;
 c->discharged[0]=c->discharged[1]=c->discharged[2]=true; c->terminal=true;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_depth_two_certificate_validate(const fractal_cdc_depth_two_certificate *c) {
 static const uint64_t split[3]={9u,6u,5u},seq[6]={9u,6u,5u,4u,3u,0u},batch[4]={9u,6u,5u,0u};
 if(!c)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_depth_two_tree_validate(&c->tree)!=FRACTAL_OK||
  memcmp(c->split_potential,split,sizeof(split))||memcmp(c->sequential_potential,seq,sizeof(seq))||
  memcmp(c->batch_potential,batch,sizeof(batch))||!c->coverage[0]||!c->coverage[1]||
  !c->disjoint_interiors[0]||!c->disjoint_interiors[1]||!c->locally_ranked[0]||
  !c->locally_ranked[1]||!c->ranks_immutable||!c->future_independent||
  !c->conventional_guards[0]||!c->conventional_guards[1]||!c->conventional_guards[2]||
  !c->discharged[0]||!c->discharged[1]||!c->discharged[2]||!c->terminal)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

fractal_result fractal_cdc_depth_two_certificate_serialize(const fractal_cdc_depth_two_certificate *c,char *buffer,size_t capacity,size_t *length) {
 int n;
 if(!length)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(fractal_cdc_depth_two_certificate_validate(c)!=FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 n=snprintf(buffer,capacity,"{\"study\":\"%s\",\"decision\":\"%s\",\"prior_one_level\":\"%s\",\"prior_direct_orbit\":\"NEGATIVE_RESULT\",\"object\":\"COMPOSITE_OBJECT_REQUIRED\",\"rank_rule\":\"%s\",\"nodes\":[\"R:2\",\"A:1\",\"B:1\",\"C:0\",\"D:0\"],\"splits\":[\"R->A,B@2.125\",\"A->C,D@2.0625\"],\"potential\":[9,6,5],\"sequential\":[9,6,5,4,3,0],\"batch\":[9,6,5,0],\"ranks_immutable\":true,\"future_independent\":true,\"terminal\":\"ALL_LEAVES_DISCHARGED\"}",FRACTAL_CDC_DEPTH_TWO_STUDY_ID,FRACTAL_CDC_DEPTH_TWO_DECISION,FRACTAL_CDC_TWO_CHILD_DECISION,FRACTAL_CDC_DEPTH_TWO_RANK_RULE);
 if(n<0)return FRACTAL_ERROR_INVALID_SPEC;
 *length=(size_t)n;
 if(!buffer||capacity<=(size_t)n)return FRACTAL_ERROR_BUFFER_TOO_SMALL;
 return FRACTAL_OK;
}
