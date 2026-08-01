#ifndef FRACTAL_ARTIFACT_SINK_H
#define FRACTAL_ARTIFACT_SINK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/module.h"
#define FRACTAL_ARTIFACT_NAME_CAPACITY 128u
#define FRACTAL_ARTIFACT_PATH_CAPACITY 512u
typedef enum fractal_artifact_destination { FRACTAL_ARTIFACT_DESTINATION_MEMORY=0,FRACTAL_ARTIFACT_DESTINATION_FILE } fractal_artifact_destination;
typedef struct fractal_artifact_begin_info { const char *logical_name; const char *media_type; size_t expected_size; } fractal_artifact_begin_info;
typedef struct fractal_artifact_result { const char *sink_module_id; const char *encoder_module_id; char logical_name[FRACTAL_ARTIFACT_NAME_CAPACITY]; char media_type[64]; char path[FRACTAL_ARTIFACT_PATH_CAPACITY]; size_t byte_count,required_size; uint64_t encoded_checksum,pixel_checksum; bool committed; fractal_artifact_destination destination; fractal_result result; } fractal_artifact_result;
typedef struct fractal_artifact_sink_vtable { const fractal_module_descriptor *descriptor; fractal_artifact_destination destination; fractal_result(*begin)(void*,const fractal_artifact_begin_info*); fractal_result(*write)(void*,const void*,size_t); fractal_result(*commit)(void*,fractal_artifact_result*); void(*abort)(void*); } fractal_artifact_sink_vtable;
typedef struct fractal_artifact_sink { const fractal_artifact_sink_vtable *vtable; void *state; } fractal_artifact_sink;
typedef struct fractal_memory_sink_state { unsigned char *buffer; size_t capacity,used,required; uint64_t checksum; unsigned phase; fractal_result failure; } fractal_memory_sink_state;
typedef struct fractal_file_sink_state { char target[FRACTAL_ARTIFACT_PATH_CAPACITY],temporary[FRACTAL_ARTIFACT_PATH_CAPACITY]; void *file; size_t used; uint64_t checksum; unsigned phase; fractal_result failure; } fractal_file_sink_state;
extern const fractal_artifact_sink_vtable fractal_artifact_sink_memory;
extern const fractal_artifact_sink_vtable fractal_artifact_sink_host_file;
void fractal_memory_sink_init(fractal_memory_sink_state*,void*,size_t);
fractal_result fractal_file_sink_init(fractal_file_sink_state*,const char*);
fractal_result fractal_artifact_sink_begin(fractal_artifact_sink*,const fractal_artifact_begin_info*);
fractal_result fractal_artifact_sink_write(fractal_artifact_sink*,const void*,size_t);
fractal_result fractal_artifact_sink_commit(fractal_artifact_sink*,fractal_artifact_result*);
void fractal_artifact_sink_abort(fractal_artifact_sink*);
#endif
