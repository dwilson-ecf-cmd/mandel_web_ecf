#ifndef FRACTAL_WORKER_STATUS_H
#define FRACTAL_WORKER_STATUS_H
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_WORKER_ID_CAPACITY 64u
#define FRACTAL_ASSIGNMENT_CAPACITY 128u
#define FRACTAL_ERROR_SUMMARY_CAPACITY 256u
typedef enum fractal_worker_health { FRACTAL_WORKER_UNKNOWN=0, FRACTAL_WORKER_HEALTHY, FRACTAL_WORKER_DEGRADED, FRACTAL_WORKER_UNHEALTHY } fractal_worker_health;
typedef struct fractal_worker_status {
 char worker_id[FRACTAL_WORKER_ID_CAPACITY]; fractal_worker_health health;
 char current_assignment[FRACTAL_ASSIGNMENT_CAPACITY]; uint64_t completed_work;
 uint64_t last_heartbeat_unix_milliseconds; char recent_error[FRACTAL_ERROR_SUMMARY_CAPACITY];
} fractal_worker_status;
fractal_result fractal_worker_status_init(fractal_worker_status *status, const char *worker_id);
#ifdef __cplusplus
}
#endif
#endif
