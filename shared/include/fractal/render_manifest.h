#ifndef FRACTAL_RENDER_MANIFEST_H
#define FRACTAL_RENDER_MANIFEST_H
#include <stddef.h>
#include "fractal/render_metrics.h"
#define FRACTAL_REVISION_CAPACITY 96u
#define FRACTAL_NOTES_CAPACITY 256u
#define FRACTAL_CDC_PDF_SHA256 "5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c"
typedef struct fractal_render_manifest {
 fractal_render_spec spec;
 fractal_render_metrics metrics;
 char application_revision[FRACTAL_REVISION_CAPACITY];
 char renderer_revision[FRACTAL_REVISION_CAPACITY];
 char cdc_reference_sha256[65];
 char compiler_identity[FRACTAL_REVISION_CAPACITY];
 char target_platform[FRACTAL_REVISION_CAPACITY];
 char pixel_format[FRACTAL_IDENTIFIER_CAPACITY];
 char equivalence_expectation[FRACTAL_REVISION_CAPACITY];
 char notes[FRACTAL_NOTES_CAPACITY];
} fractal_render_manifest;
fractal_result fractal_render_manifest_init(fractal_render_manifest *manifest,
 const fractal_render_spec *spec, fractal_computation_backend_kind computation,
 fractal_renderer_backend_kind renderer, fractal_memory_backend_kind memory);
fractal_result fractal_render_manifest_serialize_identity_json(const fractal_render_manifest *manifest,
 char *output, size_t capacity, size_t *output_length);
#endif
