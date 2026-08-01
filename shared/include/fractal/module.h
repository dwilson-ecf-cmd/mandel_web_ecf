#ifndef FRACTAL_MODULE_H
#define FRACTAL_MODULE_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_MODULE_ABI_VERSION 1u
typedef enum fractal_module_kind { FRACTAL_MODULE_FORMULA=0, FRACTAL_MODULE_NUMERIC,
 FRACTAL_MODULE_COMPUTE, FRACTAL_MODULE_REFINEMENT, FRACTAL_MODULE_SCHEDULER,
 FRACTAL_MODULE_RASTER, FRACTAL_MODULE_ENCODER, FRACTAL_MODULE_MEMORY,
 FRACTAL_MODULE_TELEMETRY, FRACTAL_MODULE_PLATFORM, FRACTAL_MODULE_KIND_COUNT } fractal_module_kind;
typedef struct fractal_module_descriptor { uint32_t abi_version, module_version;
 const char *module_id, *display_name; fractal_module_kind kind; uint64_t capability_flags;
} fractal_module_descriptor;
enum { FRACTAL_CAP_SCALAR_ARITHMETIC=UINT64_C(1)<<0, FRACTAL_CAP_POINT_SCALAR=UINT64_C(1)<<1,
 FRACTAL_CAP_ITERATION_FIELD=UINT64_C(1)<<2, FRACTAL_CAP_BGR8=UINT64_C(1)<<3,
 FRACTAL_CAP_MEMORY_SCOPES=UINT64_C(1)<<4, FRACTAL_CAP_SERIAL_FIELD=UINT64_C(1)<<5 };
fractal_result fractal_module_descriptor_validate(const fractal_module_descriptor *descriptor,
 fractal_module_kind expected_kind);
const char *fractal_module_kind_string(fractal_module_kind kind);
#ifdef __cplusplus
}
#endif
#endif
