#ifndef FRACTAL_DESKTOP_CLIENT_CORE_H
#define FRACTAL_DESKTOP_CLIENT_CORE_H
#include "fractal/local_workspace_service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct fractal_desktop_client_summary{bool compatibility_ok;bool session_ok;bool reacquired;bool replay_explicit;uint64_t revision;uint64_t generation;uint64_t semantic_digest;uint32_t semantic_drift;}fractal_desktop_client_summary;
int fractal_desktop_client_run_reference_scenario(fractal_desktop_client_summary*out);
#ifdef __cplusplus
}
#endif
#endif
