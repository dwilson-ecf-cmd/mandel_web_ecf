#include "fractal/render_artifact.h"
#include <string.h>
fractal_result fractal_render_artifact_init(fractal_render_artifact *a, const char *aid, const char *jid) {
 if (!a || !aid || !jid || !*aid || !*jid || strlen(aid)>=sizeof(a->artifact_id) || strlen(jid)>=sizeof(a->job_id)) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(a,0,sizeof(*a)); strcpy(a->artifact_id,aid); strcpy(a->job_id,jid); return FRACTAL_OK;
}
