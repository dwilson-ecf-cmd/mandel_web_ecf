#include "fractal/workspace.h"
#include <stddef.h>
#include <string.h>

static uint64_t hash_bytes(uint64_t hash, const void *data, size_t size) {
    const unsigned char *bytes = (const unsigned char *)data;
    size_t index;
    for (index = 0u; index < size; ++index) { hash ^= bytes[index]; hash *= UINT64_C(1099511628211); }
    return hash;
}
static uint64_t hash_u64(uint64_t hash, uint64_t value) {
    unsigned int shift;
    for (shift = 0u; shift < 64u; shift += 8u) { unsigned char byte = (unsigned char)(value >> shift); hash = hash_bytes(hash, &byte, 1u); }
    return hash;
}
static bool bounded_text(const char *text, size_t capacity) {
    return text != NULL && text[0] != '\0' && memchr(text, '\0', capacity) != NULL;
}
static size_t bounded_length(const char *text, size_t capacity) {
    const char *end = (const char *)memchr(text, '\0', capacity);
    return end == NULL ? capacity : (size_t)(end - text);
}
bool fractal_workspace_id_equal(fractal_workspace_id left, fractal_workspace_id right) {
    return left.high == right.high && left.low == right.low;
}
static bool id_zero(fractal_workspace_id id) { return id.high == 0u && id.low == 0u; }

fractal_workspace_capabilities fractal_workspace_role_capabilities(fractal_participant_role role) {
    const uint64_t read = FRACTAL_CAP_WORKSPACE_READ | FRACTAL_CAP_PREVIEW_READ |
        FRACTAL_CAP_ANALYSIS_READ | FRACTAL_CAP_ARTIFACT_READ | FRACTAL_CAP_PARTICIPANT_OBSERVE;
    const uint64_t edit = read | FRACTAL_CAP_WORKSPACE_EDIT | FRACTAL_CAP_CONTROL_ACQUIRE |
        FRACTAL_CAP_RENDER_CONFIGURE | FRACTAL_CAP_RENDER_START | FRACTAL_CAP_RENDER_CANCEL |
        FRACTAL_CAP_ARTIFACT_CREATE;
    if (role == FRACTAL_ROLE_OWNER || role == FRACTAL_ROLE_OPERATOR) return edit | FRACTAL_CAP_ADMIN;
    if (role == FRACTAL_ROLE_EDITOR) return edit;
    if (role == FRACTAL_ROLE_OBSERVER) return read;
    return 0u;
}

fractal_workspace_id fractal_workspace_identity(uint64_t tick, fractal_participant_id owner,
                                                 const char *name) {
    fractal_workspace_id result = {0u, 0u};
    uint64_t first = UINT64_C(1469598103934665603);
    uint64_t second = UINT64_C(1099511628211);
    first = hash_u64(hash_u64(hash_u64(first, FRACTAL_WORKSPACE_CONTRACT_VERSION), tick), owner.high);
    first = hash_u64(first, owner.low);
    if (name != NULL) first = hash_bytes(first, name, bounded_length(name, FRACTAL_WORKSPACE_NAME_CAPACITY));
    second = hash_u64(hash_u64(second, first), UINT64_C(0x57534b5350414345));
    result.high = first; result.low = second;
    return result;
}

uint64_t fractal_workspace_generation_identity(const fractal_workspace_generation *generation) {
    char canonical[2048]; size_t length = 0u; uint64_t hash = UINT64_C(1469598103934665603);
    if (generation == NULL || fractal_render_spec_serialize_canonical_json(&generation->render_spec,
        canonical, sizeof(canonical), &length) != FRACTAL_OK) return 0u;
    hash = hash_u64(hash, generation->number); hash = hash_u64(hash, generation->parent_revision);
    hash = hash_bytes(hash, canonical, length);
    hash = hash_bytes(hash, generation->numeric_id, bounded_length(generation->numeric_id, sizeof(generation->numeric_id)));
    hash = hash_bytes(hash, generation->compute_id, bounded_length(generation->compute_id, sizeof(generation->compute_id)));
    hash = hash_bytes(hash, generation->scheduler_id, bounded_length(generation->scheduler_id, sizeof(generation->scheduler_id)));
    return hash;
}

static fractal_workspace_participant *participant_find(fractal_workspace_state *state, fractal_participant_id id) {
    uint32_t index; for (index = 0u; index < state->participant_count; ++index)
        if (fractal_workspace_id_equal(state->participants[index].id, id)) return &state->participants[index];
    return NULL;
}
static const fractal_workspace_participant *participant_find_const(const fractal_workspace_state *state, fractal_participant_id id) {
    uint32_t index; for (index = 0u; index < state->participant_count; ++index)
        if (fractal_workspace_id_equal(state->participants[index].id, id)) return &state->participants[index];
    return NULL;
}

int fractal_workspace_add_participant(fractal_workspace_state *state, fractal_participant_id id,
                                      fractal_participant_role role) {
    fractal_workspace_participant *entry;
    if (state == NULL || id_zero(id) || role > FRACTAL_ROLE_OBSERVER ||
        state->participant_count >= FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY || participant_find(state, id) != NULL) return -1;
    entry = &state->participants[state->participant_count++]; memset(entry, 0, sizeof(*entry));
    entry->id = id; entry->role = role; entry->status = FRACTAL_PARTICIPANT_CONNECTED;
    entry->capabilities = fractal_workspace_role_capabilities(role); return 0;
}

int fractal_workspace_init(fractal_workspace_state *state, uint64_t tick,
                           fractal_participant_id owner, const char *name) {
    if (state == NULL || id_zero(owner) || !bounded_text(name, FRACTAL_WORKSPACE_NAME_CAPACITY)) return -1;
    memset(state, 0, sizeof(*state)); state->contract_version = FRACTAL_WORKSPACE_CONTRACT_VERSION;
    state->id = fractal_workspace_identity(tick, owner, name); memcpy(state->display_name, name, strlen(name) + 1u);
    state->lifecycle = FRACTAL_WORKSPACE_OPEN; state->revision = 1u; state->created_tick = tick; state->modified_tick = tick;
    state->lease.status = FRACTAL_LEASE_RELEASED; state->generation.execution = FRACTAL_GENERATION_PENDING;
    fractal_render_spec_init_default(&state->generation.render_spec);
    strcpy(state->generation.numeric_id, "fractal.numeric.binary64.v1");
    strcpy(state->generation.compute_id, "fractal.compute.scalar.v1");
    strcpy(state->generation.scheduler_id, "fractal.scheduler.serial.v1");
    return fractal_workspace_add_participant(state, owner, FRACTAL_ROLE_OWNER);
}

bool fractal_client_intent_validate(const fractal_client_intent *intent) {
    if (intent == NULL || intent->contract_version != FRACTAL_WORKSPACE_CONTRACT_VERSION ||
        id_zero(intent->workspace_id) || id_zero(intent->participant_id) || id_zero(intent->identity) ||
        intent->command_sequence == 0u || intent->kind > FRACTAL_INTENT_ADMIN_REVOKE_LEASE) return false;
    if (intent->kind == FRACTAL_INTENT_RENAME) return bounded_text(intent->payload.text, sizeof(intent->payload.text));
    if (intent->kind == FRACTAL_INTENT_SET_VIEWPORT) return fractal_render_spec_validate(&intent->payload.render_spec) == FRACTAL_OK;
    if (intent->kind == FRACTAL_INTENT_TRANSFER_LEASE) return !id_zero(intent->payload.participant);
    return true;
}

static bool render_affecting(fractal_client_intent_kind kind) {
    return kind >= FRACTAL_INTENT_PAN_VIEWPORT && kind <= FRACTAL_INTENT_UPDATE_PALETTE;
}
static fractal_workspace_capabilities required_capability(fractal_client_intent_kind kind) {
    if (kind == FRACTAL_INTENT_REQUEST_SNAPSHOT || kind == FRACTAL_INTENT_REQUEST_GENERATION_HISTORY) return FRACTAL_CAP_WORKSPACE_READ;
    if (kind == FRACTAL_INTENT_ACQUIRE_LEASE || kind == FRACTAL_INTENT_RELEASE_LEASE || kind == FRACTAL_INTENT_TRANSFER_LEASE) return FRACTAL_CAP_CONTROL_ACQUIRE;
    if (render_affecting(kind)) return FRACTAL_CAP_RENDER_CONFIGURE;
    if (kind == FRACTAL_INTENT_START || kind == FRACTAL_INTENT_SUPERSEDE || kind == FRACTAL_INTENT_PAUSE || kind == FRACTAL_INTENT_RESUME) return FRACTAL_CAP_RENDER_START;
    if (kind == FRACTAL_INTENT_CANCEL) return FRACTAL_CAP_RENDER_CANCEL;
    if (kind == FRACTAL_INTENT_REQUEST_ARTIFACT || kind == FRACTAL_INTENT_MARK_DISCOVERY) return FRACTAL_CAP_ARTIFACT_CREATE;
    if (kind == FRACTAL_INTENT_REQUEST_ANALYSIS) return FRACTAL_CAP_ANALYSIS_READ;
    if (kind == FRACTAL_INTENT_ADMIN_REVOKE_LEASE) return FRACTAL_CAP_ADMIN;
    return FRACTAL_CAP_WORKSPACE_EDIT;
}
static bool needs_lease(fractal_client_intent_kind kind) {
    return render_affecting(kind) || (kind >= FRACTAL_INTENT_START && kind <= FRACTAL_INTENT_CANCEL);
}

fractal_workspace_transition fractal_workspace_reduce(fractal_workspace_state *state,
                                                       const fractal_client_intent *intent) {
    fractal_workspace_transition out = {false, false, false, 0u, 0u, 0u, FRACTAL_REJECT_STRUCTURE};
    fractal_workspace_participant *actor; bool change = false;
    if (state == NULL) return out;
    out.previous_revision = state->revision; out.resulting_revision = state->revision;
    out.resulting_generation = state->generation.number;
    if (!fractal_client_intent_validate(intent)) return out;
    if (!fractal_workspace_id_equal(state->id, intent->workspace_id)) { out.rejection = FRACTAL_REJECT_WORKSPACE; return out; }
    if (intent->expected_revision != state->revision) { out.rejection = FRACTAL_REJECT_STALE_REVISION; return out; }
    actor = participant_find(state, intent->participant_id);
    if (actor == NULL || actor->status == FRACTAL_PARTICIPANT_REVOKED || (actor->capabilities & required_capability(intent->kind)) == 0u) { out.rejection = FRACTAL_REJECT_UNAUTHORIZED; return out; }
    if (intent->command_sequence <= actor->latest_accepted_command_sequence) { out.rejection = FRACTAL_REJECT_DUPLICATE_COMMAND; return out; }
    if (needs_lease(intent->kind) && (state->lease.status != FRACTAL_LEASE_ACTIVE || !fractal_workspace_id_equal(state->lease.owner, actor->id))) { out.rejection = FRACTAL_REJECT_LEASE; return out; }
    if (intent->kind == FRACTAL_INTENT_ACQUIRE_LEASE) {
        if (state->lease.status == FRACTAL_LEASE_ACTIVE && !fractal_workspace_id_equal(state->lease.owner, actor->id)) { out.rejection = FRACTAL_REJECT_LEASE; return out; }
        state->lease.owner = actor->id; state->lease.epoch++; state->lease.status = FRACTAL_LEASE_ACTIVE; change = true;
    } else if (intent->kind == FRACTAL_INTENT_RELEASE_LEASE) {
        if (state->lease.status != FRACTAL_LEASE_ACTIVE || !fractal_workspace_id_equal(state->lease.owner, actor->id)) { out.rejection = FRACTAL_REJECT_LEASE; return out; }
        state->lease.status = FRACTAL_LEASE_RELEASED; change = true;
    } else if (intent->kind == FRACTAL_INTENT_TRANSFER_LEASE) {
        fractal_workspace_participant *target = participant_find(state, intent->payload.participant);
        if (state->lease.status != FRACTAL_LEASE_ACTIVE || !fractal_workspace_id_equal(state->lease.owner, actor->id) || target == NULL || (target->capabilities & FRACTAL_CAP_CONTROL_ACQUIRE) == 0u) { out.rejection = FRACTAL_REJECT_LEASE; return out; }
        state->lease.owner = target->id; state->lease.epoch++; change = true;
    } else if (intent->kind == FRACTAL_INTENT_ADMIN_REVOKE_LEASE) {
        state->lease.status = FRACTAL_LEASE_REVOKED; state->lease.epoch++; change = true;
    } else if (intent->kind == FRACTAL_INTENT_RENAME) {
        strcpy(state->display_name, intent->payload.text); change = true;
    } else if (intent->kind == FRACTAL_INTENT_CLOSE) { state->lifecycle = FRACTAL_WORKSPACE_CLOSED; change = true;
    } else if (render_affecting(intent->kind)) {
        if (state->generation.execution == FRACTAL_GENERATION_RUNNING || state->generation.execution == FRACTAL_GENERATION_QUEUED) { state->generation.execution = FRACTAL_GENERATION_SUPERSEDED; state->generation.superseded = true; }
        state->generation.number++; state->generation.parent_revision = state->revision;
        if (intent->kind == FRACTAL_INTENT_SET_VIEWPORT) state->generation.render_spec = intent->payload.render_spec;
        state->generation.execution = FRACTAL_GENERATION_PENDING; state->generation.preview = FRACTAL_GENERATION_PENDING;
        state->generation.analysis = FRACTAL_GENERATION_PENDING; state->generation.artifact = FRACTAL_GENERATION_PENDING;
        state->generation.requested_by = actor->id; state->generation.accepted = true; state->generation.superseded = false;
        state->generation.identity = fractal_workspace_generation_identity(&state->generation); out.generation_created = true; change = true;
    } else if (intent->kind == FRACTAL_INTENT_START || intent->kind == FRACTAL_INTENT_RESUME) { state->generation.execution = FRACTAL_GENERATION_QUEUED; change = true;
    } else if (intent->kind == FRACTAL_INTENT_SUPERSEDE) { state->generation.execution = FRACTAL_GENERATION_SUPERSEDED; state->generation.superseded = true; change = true;
    } else if (intent->kind == FRACTAL_INTENT_PAUSE) { state->generation.execution = FRACTAL_GENERATION_PENDING; change = true;
    } else if (intent->kind == FRACTAL_INTENT_CANCEL) { state->generation.execution = FRACTAL_GENERATION_CANCELLED; change = true; }
    actor->latest_accepted_command_sequence = intent->command_sequence; out.accepted = true; out.rejection = FRACTAL_REJECT_NONE;
    out.authoritative_change = change;
    if (change) { state->revision++; state->modified_tick = state->created_tick + state->revision; state->lease.acquisition_revision = state->revision; }
    out.resulting_revision = state->revision; out.resulting_generation = state->generation.number; return out;
}

int fractal_workspace_snapshot_build(const fractal_workspace_state *state, fractal_participant_id actor,
                                     fractal_workspace_snapshot *snapshot) {
    const fractal_workspace_participant *entry;
    if (state == NULL || snapshot == NULL || state->participant_count > FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY || state->artifact_count > FRACTAL_WORKSPACE_ARTIFACT_CAPACITY) return -1;
    entry = participant_find_const(state, actor); if (entry == NULL) return -1;
    memset(snapshot, 0, sizeof(*snapshot)); snapshot->contract_version = state->contract_version;
    snapshot->workspace_id = state->id; snapshot->revision = state->revision; snapshot->generation = state->generation;
    snapshot->preview_reference = state->preview_reference; snapshot->analysis_reference = state->analysis_reference;
    snapshot->artifact_count = state->artifact_count; memcpy(snapshot->artifacts, state->artifacts, sizeof(state->artifacts));
    snapshot->participant_count = state->participant_count; memcpy(snapshot->participants, state->participants, sizeof(state->participants));
    snapshot->lease = state->lease; snapshot->actor_capabilities = entry->capabilities;
    snapshot->latest_accepted_command_sequence = entry->latest_accepted_command_sequence;
    snapshot->runtime_capabilities = UINT64_C(1); snapshot->compatibility_status = 1u; return 0;
}
