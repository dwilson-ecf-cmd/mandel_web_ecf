#ifndef FRACTAL_WORKSPACE_H
#define FRACTAL_WORKSPACE_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/render_spec.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_WORKSPACE_CONTRACT_VERSION 1u
#define FRACTAL_WORKSPACE_NAME_CAPACITY 96u
#define FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY 8u
#define FRACTAL_WORKSPACE_ARTIFACT_CAPACITY 16u
#define FRACTAL_WORKSPACE_ANALYZER_CAPACITY 8u
#define FRACTAL_WORKSPACE_PARAMETER_CAPACITY 8u

typedef struct fractal_workspace_id { uint64_t high; uint64_t low; } fractal_workspace_id;
typedef fractal_workspace_id fractal_participant_id;
typedef fractal_workspace_id fractal_intent_id;

typedef enum fractal_workspace_lifecycle { FRACTAL_WORKSPACE_OPEN = 0, FRACTAL_WORKSPACE_CLOSED } fractal_workspace_lifecycle;
typedef enum fractal_generation_lifecycle {
    FRACTAL_GENERATION_PENDING = 0, FRACTAL_GENERATION_QUEUED, FRACTAL_GENERATION_RUNNING,
    FRACTAL_GENERATION_SUPERSEDED, FRACTAL_GENERATION_CANCELLED, FRACTAL_GENERATION_FAILED,
    FRACTAL_GENERATION_COMPLETED
} fractal_generation_lifecycle;
typedef enum fractal_participant_role {
    FRACTAL_ROLE_OWNER = 0, FRACTAL_ROLE_OPERATOR, FRACTAL_ROLE_EDITOR, FRACTAL_ROLE_OBSERVER
} fractal_participant_role;
typedef enum fractal_participant_status {
    FRACTAL_PARTICIPANT_CONNECTED = 0, FRACTAL_PARTICIPANT_DISCONNECTED, FRACTAL_PARTICIPANT_REVOKED
} fractal_participant_status;
typedef enum fractal_lease_status {
    FRACTAL_LEASE_RELEASED = 0, FRACTAL_LEASE_ACTIVE, FRACTAL_LEASE_REVOKED, FRACTAL_LEASE_EXPIRED
} fractal_lease_status;

typedef uint64_t fractal_workspace_capabilities;
#define FRACTAL_CAP_WORKSPACE_READ       (UINT64_C(1) << 0)
#define FRACTAL_CAP_WORKSPACE_EDIT       (UINT64_C(1) << 1)
#define FRACTAL_CAP_CONTROL_ACQUIRE      (UINT64_C(1) << 2)
#define FRACTAL_CAP_RENDER_CONFIGURE     (UINT64_C(1) << 3)
#define FRACTAL_CAP_RENDER_START         (UINT64_C(1) << 4)
#define FRACTAL_CAP_RENDER_CANCEL        (UINT64_C(1) << 5)
#define FRACTAL_CAP_PREVIEW_READ         (UINT64_C(1) << 6)
#define FRACTAL_CAP_ANALYSIS_READ        (UINT64_C(1) << 7)
#define FRACTAL_CAP_ARTIFACT_CREATE      (UINT64_C(1) << 8)
#define FRACTAL_CAP_ARTIFACT_READ        (UINT64_C(1) << 9)
#define FRACTAL_CAP_PARTICIPANT_OBSERVE  (UINT64_C(1) << 10)
#define FRACTAL_CAP_ADMIN                (UINT64_C(1) << 63)

typedef struct fractal_workspace_participant {
    fractal_participant_id id;
    fractal_participant_role role;
    fractal_participant_status status;
    fractal_workspace_capabilities capabilities;
    uint64_t latest_accepted_command_sequence;
} fractal_workspace_participant;

typedef struct fractal_controller_lease {
    fractal_participant_id owner;
    uint64_t epoch;
    uint64_t acquisition_revision;
    uint64_t expiry_tick;
    fractal_lease_status status;
} fractal_controller_lease;

typedef struct fractal_workspace_generation {
    uint64_t number;
    uint64_t parent_revision;
    uint64_t identity;
    fractal_render_spec render_spec;
    char numeric_id[FRACTAL_IDENTIFIER_CAPACITY];
    char compute_id[FRACTAL_IDENTIFIER_CAPACITY];
    char scheduler_id[FRACTAL_IDENTIFIER_CAPACITY];
    char analyzer_ids[FRACTAL_WORKSPACE_ANALYZER_CAPACITY][FRACTAL_IDENTIFIER_CAPACITY];
    uint32_t analyzer_count;
    fractal_generation_lifecycle execution;
    fractal_generation_lifecycle preview;
    fractal_generation_lifecycle analysis;
    fractal_generation_lifecycle artifact;
    bool superseded;
    fractal_participant_id requested_by;
    bool accepted;
} fractal_workspace_generation;

typedef struct fractal_workspace_artifact_summary {
    uint64_t generation;
    uint64_t identity;
    char name[FRACTAL_WORKSPACE_NAME_CAPACITY];
} fractal_workspace_artifact_summary;

typedef struct fractal_workspace_state {
    uint32_t contract_version;
    fractal_workspace_id id;
    char display_name[FRACTAL_WORKSPACE_NAME_CAPACITY];
    fractal_workspace_lifecycle lifecycle;
    uint64_t revision;
    uint64_t created_tick;
    uint64_t modified_tick;
    fractal_workspace_generation generation;
    uint64_t preview_reference;
    uint64_t analysis_reference;
    fractal_workspace_artifact_summary artifacts[FRACTAL_WORKSPACE_ARTIFACT_CAPACITY];
    uint32_t artifact_count;
    fractal_workspace_participant participants[FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY];
    uint32_t participant_count;
    fractal_controller_lease lease;
    uint32_t status_code;
    uint32_t error_code;
} fractal_workspace_state;

typedef enum fractal_client_intent_kind {
    FRACTAL_INTENT_CREATE = 0, FRACTAL_INTENT_OPEN, FRACTAL_INTENT_RENAME, FRACTAL_INTENT_CLOSE,
    FRACTAL_INTENT_REQUEST_SNAPSHOT, FRACTAL_INTENT_ACQUIRE_LEASE, FRACTAL_INTENT_RELEASE_LEASE,
    FRACTAL_INTENT_TRANSFER_LEASE, FRACTAL_INTENT_PAN_VIEWPORT, FRACTAL_INTENT_ZOOM_VIEWPORT,
    FRACTAL_INTENT_SET_VIEWPORT, FRACTAL_INTENT_SELECT_FORMULA, FRACTAL_INTENT_UPDATE_FORMULA_PARAMETERS,
    FRACTAL_INTENT_SELECT_NUMERIC, FRACTAL_INTENT_SET_ITERATION_BUDGET, FRACTAL_INTENT_SET_BAILOUT,
    FRACTAL_INTENT_SET_RESOLUTION, FRACTAL_INTENT_SELECT_SCHEDULER, FRACTAL_INTENT_SELECT_ANALYZERS,
    FRACTAL_INTENT_UPDATE_PALETTE, FRACTAL_INTENT_START, FRACTAL_INTENT_SUPERSEDE,
    FRACTAL_INTENT_PAUSE, FRACTAL_INTENT_RESUME, FRACTAL_INTENT_CANCEL,
    FRACTAL_INTENT_REQUEST_ARTIFACT, FRACTAL_INTENT_REQUEST_ANALYSIS, FRACTAL_INTENT_MARK_DISCOVERY,
    FRACTAL_INTENT_REQUEST_GENERATION_HISTORY, FRACTAL_INTENT_ADMIN_REVOKE_LEASE
} fractal_client_intent_kind;

typedef union fractal_client_intent_payload {
    char text[FRACTAL_WORKSPACE_NAME_CAPACITY];
    fractal_render_spec render_spec;
    fractal_participant_id participant;
    struct { uint32_t width; uint32_t height; } resolution;
    uint64_t unsigned_value;
} fractal_client_intent_payload;

typedef struct fractal_client_intent {
    uint32_t contract_version;
    fractal_workspace_id workspace_id;
    fractal_participant_id participant_id;
    uint64_t expected_revision;
    uint64_t command_sequence;
    fractal_client_intent_kind kind;
    fractal_client_intent_payload payload;
    fractal_intent_id identity;
} fractal_client_intent;

typedef enum fractal_workspace_rejection {
    FRACTAL_REJECT_NONE = 0, FRACTAL_REJECT_STRUCTURE, FRACTAL_REJECT_WORKSPACE,
    FRACTAL_REJECT_STALE_REVISION, FRACTAL_REJECT_DUPLICATE_COMMAND,
    FRACTAL_REJECT_UNAUTHORIZED, FRACTAL_REJECT_LEASE, FRACTAL_REJECT_CAPACITY,
    FRACTAL_REJECT_STATE
} fractal_workspace_rejection;

typedef struct fractal_workspace_transition {
    bool accepted;
    bool authoritative_change;
    bool generation_created;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    uint64_t resulting_generation;
    fractal_workspace_rejection rejection;
} fractal_workspace_transition;

typedef struct fractal_workspace_snapshot {
    uint32_t contract_version;
    fractal_workspace_id workspace_id;
    uint64_t revision;
    fractal_workspace_generation generation;
    uint32_t progress_basis_points;
    uint64_t preview_reference;
    uint64_t analysis_reference;
    fractal_workspace_artifact_summary artifacts[FRACTAL_WORKSPACE_ARTIFACT_CAPACITY];
    uint32_t artifact_count;
    fractal_workspace_participant participants[FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY];
    uint32_t participant_count;
    fractal_controller_lease lease;
    fractal_workspace_capabilities actor_capabilities;
    uint64_t latest_accepted_command_sequence;
    uint64_t runtime_capabilities;
    uint32_t compatibility_status;
} fractal_workspace_snapshot;

fractal_workspace_capabilities fractal_workspace_role_capabilities(fractal_participant_role role);
bool fractal_workspace_id_equal(fractal_workspace_id left, fractal_workspace_id right);
fractal_workspace_id fractal_workspace_identity(uint64_t creation_tick, fractal_participant_id owner,
                                                 const char *display_name);
uint64_t fractal_workspace_generation_identity(const fractal_workspace_generation *generation);
bool fractal_client_intent_validate(const fractal_client_intent *intent);
int fractal_workspace_init(fractal_workspace_state *state, uint64_t creation_tick,
                           fractal_participant_id owner, const char *display_name);
int fractal_workspace_add_participant(fractal_workspace_state *state, fractal_participant_id id,
                                      fractal_participant_role role);
fractal_workspace_transition fractal_workspace_reduce(fractal_workspace_state *state,
                                                       const fractal_client_intent *intent);
int fractal_workspace_snapshot_build(const fractal_workspace_state *state,
                                     fractal_participant_id actor,
                                     fractal_workspace_snapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
