#include "fractal/render_failure.h"
#include <string.h>
fractal_result fractal_render_failure_init(fractal_render_failure *f, const char *code, const char *summary, const char *subsystem, bool recoverable) {
 if (!f || !code || !summary || !subsystem || !*code || !*summary || !*subsystem ||
 strlen(code)>=sizeof(f->error_code) || strlen(summary)>=sizeof(f->summary) || strlen(subsystem)>=sizeof(f->subsystem)) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(f,0,sizeof(*f)); strcpy(f->error_code,code); strcpy(f->summary,summary); strcpy(f->subsystem,subsystem); f->recoverable=recoverable; return FRACTAL_OK;
}
