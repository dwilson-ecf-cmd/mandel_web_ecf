#include "fractal/module.h"
#include <string.h>
const char *fractal_module_kind_string(fractal_module_kind k) { static const char *n[]={"formula","numeric","compute","refinement","scheduler","raster","encoder","memory","telemetry","platform","artifact_sink","analyzer"}; return (unsigned)k<FRACTAL_MODULE_KIND_COUNT?n[k]:NULL; }
fractal_result fractal_module_descriptor_validate(const fractal_module_descriptor *d,fractal_module_kind k) {
 if(!d||!d->module_id||!d->display_name||!d->module_id[0]||!d->display_name[0])return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(d->abi_version!=FRACTAL_MODULE_ABI_VERSION||d->module_version==0u||d->kind!=k||!fractal_module_kind_string(d->kind))return FRACTAL_ERROR_INVALID_SPEC;
 for(const unsigned char *p=(const unsigned char *)d->module_id;*p;p++)if(!( (*p>='a'&&*p<='z')||(*p>='0'&&*p<='9')||*p=='.'||*p=='-' ))return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}
