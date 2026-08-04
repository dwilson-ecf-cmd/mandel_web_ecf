#include "fractal/installed_modules.h"
#include "fractal/render_manifest.h"
#include "fractal/scheduler.h"
#include "fractal/socketable_runtime.h"

#include <stddef.h>

_Static_assert(FRACTAL_THREAD_POOL_MAX_WORKERS > 0u,
 "Предел числа работников должен быть положительным");
_Static_assert(FRACTAL_PUBLICATION_FAILED > FRACTAL_PUBLICATION_NOT_APPLICABLE,
 "Состояния публикации должны сохранять полный жизненный цикл");

static void verify_runtime_contract(fractal_runtime_modules *modules,
 fractal_runtime_output *output)
{
 modules->scheduler_options.requested_worker_count=1u;
 output->scheduler_execution.requested_worker_count=1u;
 output->publication_status=FRACTAL_PUBLICATION_ACTIVE;
 output->pipeline_result=FRACTAL_OK;
}

int main(void)
{
 fractal_runtime_modules modules={0};
 fractal_runtime_output output={0};
 verify_runtime_contract(&modules,&output);
 return modules.scheduler_options.requested_worker_count==
   output.scheduler_execution.requested_worker_count&&
   output.publication_status==FRACTAL_PUBLICATION_ACTIVE&&
   output.pipeline_result==FRACTAL_OK?0:1;
}
