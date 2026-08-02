#ifndef FRACTAL_INSTALLED_MODULES_H
#define FRACTAL_INSTALLED_MODULES_H
#include "fractal/module_registry.h"
#include "fractal/socketable_runtime.h"
typedef struct fractal_runtime_selection { const char *formula,*numeric,*compute,*refinement,*scheduler,*raster,*encoder,*memory,*telemetry,*platform,*analyzer; fractal_analysis_output_mode analysis_mode; size_t analyzer_count; const char *analyzers[FRACTAL_ANALYZER_CHAIN_MAX]; uint32_t scheduler_worker_count; } fractal_runtime_selection;
fractal_result fractal_installed_modules_registry(fractal_module_registry *);
fractal_result fractal_runtime_assemble(const fractal_module_registry *,const fractal_runtime_selection *,fractal_memory_backend *,fractal_runtime_modules *);
#endif
