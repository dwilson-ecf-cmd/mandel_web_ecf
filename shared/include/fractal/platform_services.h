#ifndef FRACTAL_PLATFORM_SERVICES_H
#define FRACTAL_PLATFORM_SERVICES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fractal/result.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FRACTAL_PLATFORM_SERVICE_CONTRACT_VERSION 1u
#define FRACTAL_PLATFORM_FRAME_CAPACITY 1024u
typedef enum fractal_platform_service_group{FRACTAL_PLATFORM_CORE=1,FRACTAL_PLATFORM_SECURE_STORAGE,FRACTAL_PLATFORM_RANDOM,FRACTAL_PLATFORM_TRANSPORT,FRACTAL_PLATFORM_LIFECYCLE,FRACTAL_PLATFORM_DISCOVERY}fractal_platform_service_group;
typedef enum fractal_transport_status{FRACTAL_TRANSPORT_CLOSED=0,FRACTAL_TRANSPORT_READY,FRACTAL_TRANSPORT_BLOCKED,FRACTAL_TRANSPORT_FAILED}fractal_transport_status;
typedef enum fractal_lifecycle_signal{FRACTAL_LIFECYCLE_FOREGROUND=0,FRACTAL_LIFECYCLE_BACKGROUND,FRACTAL_LIFECYCLE_SUSPEND,FRACTAL_LIFECYCLE_RESUME,FRACTAL_LIFECYCLE_SHUTDOWN,FRACTAL_LIFECYCLE_NETWORK_CHANGED}fractal_lifecycle_signal;
typedef struct fractal_platform_frame{uint8_t bytes[FRACTAL_PLATFORM_FRAME_CAPACITY];uint32_t length;}fractal_platform_frame;
typedef struct fractal_platform_core{uint32_t version;uint64_t(*monotonic_tick)(void*);fractal_result(*sleep_until_tick)(void*,uint64_t);fractal_result(*yield)(void*);void*context;}fractal_platform_core;
typedef struct fractal_platform_secure_storage{uint32_t version;fractal_result(*load)(void*,uint64_t,fractal_platform_frame*);fractal_result(*store_replace)(void*,uint64_t,const fractal_platform_frame*);fractal_result(*delete_key)(void*,uint64_t);bool(*available)(void*);void*context;}fractal_platform_secure_storage;
typedef struct fractal_platform_random{uint32_t version;fractal_result(*fill)(void*,uint8_t*,uint32_t);void*context;}fractal_platform_random;
typedef struct fractal_platform_transport{uint32_t version;fractal_result(*connect)(void*);fractal_result(*close)(void*);fractal_result(*send)(void*,const fractal_platform_frame*);fractal_result(*receive)(void*,fractal_platform_frame*);fractal_transport_status(*status)(void*);void*context;}fractal_platform_transport;
typedef struct fractal_platform_lifecycle{uint32_t version;fractal_lifecycle_signal last_signal;bool wake_lock_requested;bool data_saver;bool battery_saver;}fractal_platform_lifecycle;
typedef struct fractal_platform_discovery{uint32_t version;fractal_result(*publish)(void*,const fractal_platform_frame*);fractal_result(*locate)(void*,fractal_platform_frame*);void*context;}fractal_platform_discovery;
#ifdef __cplusplus
}
#endif
#endif
