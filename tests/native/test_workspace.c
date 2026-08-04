#include "fractal/workspace.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static fractal_workspace_id id(uint64_t value) { fractal_workspace_id result = {value, value * 17u}; return result; }
static fractal_client_intent intent(const fractal_workspace_state *state, fractal_participant_id actor,
                                    uint64_t sequence, fractal_client_intent_kind kind) {
    fractal_client_intent value; memset(&value, 0, sizeof(value));
    value.contract_version = FRACTAL_WORKSPACE_CONTRACT_VERSION; value.workspace_id = state->id;
    value.participant_id = actor; value.expected_revision = state->revision; value.command_sequence = sequence;
    value.kind = kind; value.identity = id(1000u + sequence); return value;
}

int main(void) {
    fractal_workspace_state state, copy; fractal_workspace_snapshot snapshot;
    fractal_workspace_id owner = id(1u), editor = id(2u), observer = id(3u), other = id(4u);
    fractal_client_intent command; fractal_workspace_transition transition, repeated;
    uint64_t initial_revision, initial_generation;
    assert(fractal_workspace_init(&state, 42u, owner, "Лаборатория") == 0);
    assert(state.revision == 1u && state.participant_count == 1u);
    assert(fractal_workspace_id_equal(state.id, fractal_workspace_identity(42u, owner, "Лаборатория")));
    assert(fractal_workspace_add_participant(&state, editor, FRACTAL_ROLE_EDITOR) == 0);
    assert(fractal_workspace_add_participant(&state, observer, FRACTAL_ROLE_OBSERVER) == 0);

    command = intent(&state, observer, 1u, FRACTAL_INTENT_SET_VIEWPORT);
    command.payload.render_spec = state.generation.render_spec;
    assert(fractal_workspace_reduce(&state, &command).rejection == FRACTAL_REJECT_UNAUTHORIZED);

    command = intent(&state, editor, 1u, FRACTAL_INTENT_ACQUIRE_LEASE);
    transition = fractal_workspace_reduce(&state, &command); assert(transition.accepted && state.revision == 2u);
    command = intent(&state, owner, 1u, FRACTAL_INTENT_ACQUIRE_LEASE);
    assert(fractal_workspace_reduce(&state, &command).rejection == FRACTAL_REJECT_LEASE);

    initial_revision = state.revision; initial_generation = state.generation.number;
    command = intent(&state, editor, 2u, FRACTAL_INTENT_SET_VIEWPORT);
    command.payload.render_spec = state.generation.render_spec; strcpy(command.payload.render_spec.scale, "2");
    copy = state; transition = fractal_workspace_reduce(&state, &command);
    repeated = fractal_workspace_reduce(&copy, &command);
    assert(transition.accepted && transition.generation_created && state.revision == initial_revision + 1u);
    assert(state.generation.number == initial_generation + 1u && state.generation.identity == copy.generation.identity);
    assert(transition.resulting_revision == repeated.resulting_revision);
    assert(fractal_workspace_reduce(&state, &command).rejection == FRACTAL_REJECT_STALE_REVISION);

    command = intent(&state, editor, 3u, FRACTAL_INTENT_REQUEST_SNAPSHOT);
    initial_revision = state.revision; initial_generation = state.generation.number;
    assert(fractal_workspace_reduce(&state, &command).accepted);
    assert(state.revision == initial_revision && state.generation.number == initial_generation);
    command = intent(&state, editor, 3u, FRACTAL_INTENT_REQUEST_SNAPSHOT);
    assert(fractal_workspace_reduce(&state, &command).rejection == FRACTAL_REJECT_DUPLICATE_COMMAND);

    command = intent(&state, editor, 4u, FRACTAL_INTENT_TRANSFER_LEASE); command.payload.participant = owner;
    assert(fractal_workspace_reduce(&state, &command).accepted && fractal_workspace_id_equal(state.lease.owner, owner));
    command = intent(&state, owner, 2u, FRACTAL_INTENT_START); assert(fractal_workspace_reduce(&state, &command).accepted);
    command = intent(&state, owner, 3u, FRACTAL_INTENT_SUPERSEDE); assert(fractal_workspace_reduce(&state, &command).accepted);
    assert(state.generation.execution == FRACTAL_GENERATION_SUPERSEDED);
    command = intent(&state, owner, 4u, FRACTAL_INTENT_CANCEL); assert(fractal_workspace_reduce(&state, &command).accepted);
    assert(state.generation.execution == FRACTAL_GENERATION_CANCELLED);
    command = intent(&state, owner, 5u, FRACTAL_INTENT_ADMIN_REVOKE_LEASE);
    assert(fractal_workspace_reduce(&state, &command).accepted && state.lease.status == FRACTAL_LEASE_REVOKED);

    assert(fractal_workspace_snapshot_build(&state, observer, &snapshot) == 0);
    assert(snapshot.revision == state.revision && snapshot.participant_count == state.participant_count);
    while (state.participant_count < FRACTAL_WORKSPACE_PARTICIPANT_CAPACITY)
        assert(fractal_workspace_add_participant(&state, id(10u + state.participant_count), FRACTAL_ROLE_OBSERVER) == 0);
    assert(fractal_workspace_add_participant(&state, other, FRACTAL_ROLE_EDITOR) != 0);
    state.artifact_count = FRACTAL_WORKSPACE_ARTIFACT_CAPACITY;
    assert(fractal_workspace_snapshot_build(&state, owner, &snapshot) == 0);
    state.artifact_count++; assert(fractal_workspace_snapshot_build(&state, owner, &snapshot) != 0);
    puts("workspace contract tests: ok"); return 0;
}
