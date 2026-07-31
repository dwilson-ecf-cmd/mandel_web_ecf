#include "fractal/memory_backend.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct fractal_memory_allocation {
 struct fractal_memory_allocation *next; void *base; size_t requested;
} fractal_memory_allocation;

static bool add_u64(uint64_t *value, size_t amount) {
 if ((uintmax_t)amount > UINT64_MAX || *value > UINT64_MAX-(uint64_t)amount) { *value=UINT64_MAX; return false; }
 *value += (uint64_t)amount; return true;
}
static bool power_of_two(size_t value) { return value != 0u && (value & (value-1u)) == 0u; }

fractal_result fractal_memory_backend_init_system(fractal_memory_backend *backend) {
 if (!backend) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(backend,0,sizeof(*backend)); backend->kind=FRACTAL_MEMORY_BACKEND_SYSTEM; backend->initialized=true; return FRACTAL_OK;
}
fractal_result fractal_memory_backend_init_default(fractal_memory_backend *backend) { return fractal_memory_backend_init_system(backend); }
fractal_result fractal_memory_backend_init_ouro(fractal_memory_backend *backend) {
 if (!backend) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(backend,0,sizeof(*backend)); backend->kind=FRACTAL_MEMORY_BACKEND_OURO; return FRACTAL_ERROR_NOT_IMPLEMENTED;
}
fractal_result fractal_memory_scope_begin(fractal_memory_backend *backend, fractal_memory_scope_kind kind, fractal_memory_scope *scope) {
 if (!backend || !scope || !backend->initialized || backend->kind != FRACTAL_MEMORY_BACKEND_SYSTEM || kind > FRACTAL_MEMORY_SCOPE_LONG)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(scope,0,sizeof(*scope)); scope->backend=backend; scope->kind=kind; scope->active=true; return FRACTAL_OK;
}
void *fractal_memory_scope_alloc(fractal_memory_scope *scope, size_t size, size_t alignment) {
 fractal_memory_backend *backend; fractal_memory_allocation *entry; void *base; uintptr_t raw, aligned; size_t overhead;
 if (!scope || !scope->active || !scope->backend) return NULL;
 backend=scope->backend; add_u64(&backend->statistics.requested_bytes,size);
 if (size == 0u || !power_of_two(alignment) || alignment > SIZE_MAX-size ||
     sizeof(fractal_memory_allocation) > SIZE_MAX-size-(alignment-1u)) goto failed;
 overhead=sizeof(fractal_memory_allocation)+alignment-1u;
 base=malloc(size+overhead); if (!base) goto failed;
 entry=(fractal_memory_allocation *)base;
 raw=(uintptr_t)((unsigned char *)base+sizeof(*entry));
 if (raw > UINTPTR_MAX-(alignment-1u)) { free(base); goto failed; }
 aligned=(raw+alignment-1u)&~(uintptr_t)(alignment-1u);
 entry->base=base; entry->requested=size; entry->next=(fractal_memory_allocation *)scope->allocations;
 scope->allocations=(struct fractal_memory_allocation *)entry;
 add_u64(&backend->statistics.successful_bytes,size); add_u64(&backend->statistics.active_bytes,size);
 backend->statistics.successful_allocations++; backend->statistics.active_allocations++;
 return (void *)aligned;
failed:
 add_u64(&backend->statistics.failed_bytes,size); backend->statistics.failed_allocations++; return NULL;
}
fractal_result fractal_memory_scope_reset(fractal_memory_scope *scope) {
 fractal_memory_allocation *entry;
 if (!scope || !scope->active || !scope->backend) return FRACTAL_ERROR_INVALID_ARGUMENT;
 entry=(fractal_memory_allocation *)scope->allocations;
 while (entry) {
  fractal_memory_allocation *next=entry->next; size_t size=entry->requested;
  if (scope->backend->statistics.active_bytes >= size) scope->backend->statistics.active_bytes -= (uint64_t)size;
  else scope->backend->statistics.active_bytes=0u;
  add_u64(&scope->backend->statistics.released_bytes,size);
  if (scope->backend->statistics.active_allocations) scope->backend->statistics.active_allocations--;
  free(entry->base); entry=next;
 }
 scope->allocations=NULL; return FRACTAL_OK;
}
void fractal_memory_scope_end(fractal_memory_scope *scope) {
 if (!scope) return;
 if (scope->active) (void)fractal_memory_scope_reset(scope);
 scope->backend=NULL; scope->active=false;
}
fractal_memory_statistics fractal_memory_backend_get_statistics(const fractal_memory_backend *backend) {
 fractal_memory_statistics empty; memset(&empty,0,sizeof(empty)); return backend ? backend->statistics : empty;
}
void fractal_memory_backend_shutdown(fractal_memory_backend *backend) {
 /* Scopes are caller-owned and must end first; active bytes diagnose misuse. */
 if (!backend) return;
 backend->initialized=false;
}
