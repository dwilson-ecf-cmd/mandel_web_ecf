#ifndef FRACTAL_CDC_DEPTH_TWO_H
#define FRACTAL_CDC_DEPTH_TWO_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal_cdc_two_child.h"

#define FRACTAL_CDC_DEPTH_TWO_STUDY_ID "cdc-depth-two-study-v1"
#define FRACTAL_CDC_DEPTH_TWO_DECISION "SUPPORTED_DEPTH_TWO_COMPOSITION"
#define FRACTAL_CDC_DEPTH_TWO_RANK_RULE "REGISTERED-LOCAL-REFINEMENT-BUDGET-V1"

typedef enum fractal_cdc_depth_two_node {
 FRACTAL_CDC_NODE_R=0, FRACTAL_CDC_NODE_A=1, FRACTAL_CDC_NODE_B=2,
 FRACTAL_CDC_NODE_C=3, FRACTAL_CDC_NODE_D=4
} fractal_cdc_depth_two_node;

typedef struct fractal_cdc_depth_two_tree {
 fractal_cdc_region region[5];
 uint8_t rank[5];
 uint8_t parent[5];
 double split_1, split_2;
} fractal_cdc_depth_two_tree;

typedef struct fractal_cdc_depth_two_certificate {
 fractal_cdc_depth_two_tree tree;
 uint64_t split_potential[3];
 uint64_t sequential_potential[6];
 uint64_t batch_potential[4];
 bool coverage[2], disjoint_interiors[2], locally_ranked[2];
 bool ranks_immutable, future_independent;
 bool conventional_guards[3], discharged[3], terminal;
} fractal_cdc_depth_two_certificate;

fractal_result fractal_cdc_depth_two_tree_create(fractal_cdc_depth_two_tree *tree);
fractal_result fractal_cdc_depth_two_tree_validate(const fractal_cdc_depth_two_tree *tree);
fractal_result fractal_cdc_depth_two_potential(const uint8_t *ranks,size_t count,uint64_t *value);
fractal_result fractal_cdc_depth_two_certificate_create(fractal_cdc_depth_two_certificate *certificate);
fractal_result fractal_cdc_depth_two_certificate_validate(const fractal_cdc_depth_two_certificate *certificate);
fractal_result fractal_cdc_depth_two_certificate_serialize(const fractal_cdc_depth_two_certificate *certificate,
 char *buffer,size_t capacity,size_t *length);
#endif
