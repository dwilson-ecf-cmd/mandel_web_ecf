#ifndef FRACTAL_MEMORY_BACKEND_H
#define FRACTAL_MEMORY_BACKEND_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum fractal_memory_backend_kind { FRACTAL_MEMORY_BACKEND_SYSTEM=0, FRACTAL_MEMORY_BACKEND_OURO } fractal_memory_backend_kind;
typedef enum fractal_memory_scope_kind { FRACTAL_MEMORY_SCOPE_SHORT=0, FRACTAL_MEMORY_SCOPE_MEDIUM, FRACTAL_MEMORY_SCOPE_LONG } fractal_memory_scope_kind;
typedef struct fractal_memory_statistics {
 uint64_t requested_bytes, successful_bytes, failed_bytes, active_bytes, released_bytes;
 uint64_t successful_allocations, failed_allocations, active_allocations;
} fractal_memory_statistics;
struct fractal_memory_allocation;
typedef struct fractal_memory_backend {
 fractal_memory_backend_kind kind; bool initialized; fractal_memory_statistics statistics;
} fractal_memory_backend;
typedef struct fractal_memory_scope {
 fractal_memory_backend *backend; fractal_memory_scope_kind kind;
 struct fractal_memory_allocation *allocations; bool active;
} fractal_memory_scope;

fractal_result fractal_memory_backend_init_default(fractal_memory_backend *backend);
fractal_result fractal_memory_backend_init_system(fractal_memory_backend *backend);
fractal_result fractal_memory_backend_init_ouro(fractal_memory_backend *backend);
fractal_result fractal_memory_scope_begin(fractal_memory_backend *backend,
 fractal_memory_scope_kind kind, fractal_memory_scope *scope);
void *fractal_memory_scope_alloc(fractal_memory_scope *scope, size_t size, size_t alignment);
fractal_result fractal_memory_scope_reset(fractal_memory_scope *scope);
void fractal_memory_scope_end(fractal_memory_scope *scope);
fractal_memory_statistics fractal_memory_backend_get_statistics(const fractal_memory_backend *backend);
void fractal_memory_backend_shutdown(fractal_memory_backend *backend);
#ifdef __cplusplus
}
#endif
#endif
