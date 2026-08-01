#ifndef FRACTAL_MODULE_REGISTRY_H
#define FRACTAL_MODULE_REGISTRY_H
#include <stddef.h>
#include <stdint.h>
#include "fractal/module.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_REGISTRY_MAX_MODULES 32u
typedef struct fractal_module_registration {
 const fractal_module_descriptor *descriptor;
 const void *implementation;
} fractal_module_registration;
typedef struct fractal_module_registry {
 fractal_module_registration entries[FRACTAL_REGISTRY_MAX_MODULES];
 size_t count;
 uint64_t identity;
} fractal_module_registry;
fractal_result fractal_module_registry_init(fractal_module_registry *,const fractal_module_registration *,size_t);
size_t fractal_module_registry_total_count(const fractal_module_registry *);
size_t fractal_module_registry_count(const fractal_module_registry *,fractal_module_kind);
const fractal_module_descriptor *fractal_module_registry_at(const fractal_module_registry *,fractal_module_kind,size_t);
const fractal_module_descriptor *fractal_module_registry_find(const fractal_module_registry *,fractal_module_kind,const char *);
const void *fractal_module_registry_implementation(const fractal_module_registry *,fractal_module_kind,const char *);
uint64_t fractal_module_registry_identity(const fractal_module_registry *);
fractal_result fractal_module_registry_serialize(const fractal_module_registry *,char *,size_t,size_t *);
#ifdef __cplusplus
}
#endif
#endif
