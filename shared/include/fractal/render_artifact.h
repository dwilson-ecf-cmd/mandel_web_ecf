#ifndef FRACTAL_RENDER_ARTIFACT_H
#define FRACTAL_RENDER_ARTIFACT_H
#include <stdint.h>
#include "fractal/render_job.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_ARTIFACT_ID_CAPACITY 64u
#define FRACTAL_MEDIA_TYPE_CAPACITY 64u
#define FRACTAL_CHECKSUM_CAPACITY 128u
typedef struct fractal_render_artifact {
 char artifact_id[FRACTAL_ARTIFACT_ID_CAPACITY], job_id[FRACTAL_JOB_ID_CAPACITY];
 char storage_key[FRACTAL_STORAGE_KEY_CAPACITY], media_type[FRACTAL_MEDIA_TYPE_CAPACITY];
 uint32_t width, height; char checksum[FRACTAL_CHECKSUM_CAPACITY];
 uint64_t created_unix_milliseconds; char manifest_key[FRACTAL_STORAGE_KEY_CAPACITY];
} fractal_render_artifact;
fractal_result fractal_render_artifact_init(fractal_render_artifact *artifact,
 const char *artifact_id, const char *job_id);
#ifdef __cplusplus
}
#endif
#endif
