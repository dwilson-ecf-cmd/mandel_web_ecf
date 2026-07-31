#include "fractal/worker_status.h"
#include <string.h>
fractal_result fractal_worker_status_init(fractal_worker_status *status, const char *id) {
 if (!status || !id || !*id || strlen(id) >= sizeof(status->worker_id)) return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(status, 0, sizeof(*status)); strcpy(status->worker_id, id); status->health=FRACTAL_WORKER_UNKNOWN; return FRACTAL_OK;
}
