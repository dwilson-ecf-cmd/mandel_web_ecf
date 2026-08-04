#ifndef FRACTAL_PAIRING_H
#define FRACTAL_PAIRING_H
#include <stdbool.h>
#include <stdint.h>
#include "fractal/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FRACTAL_PAIRING_PROTOCOL_VERSION 1u
#define FRACTAL_PAIRING_NAME_CAPACITY 96u
#define FRACTAL_PAIRING_FINGERPRINT_CAPACITY 32u
#define FRACTAL_PAIRING_NONCE_CAPACITY 16u
#define FRACTAL_PAIRING_PENDING_CAPACITY 4u
#define FRACTAL_PAIRING_CREDENTIAL_CAPACITY 8u
#define FRACTAL_PAIRING_ALLOWED_WORKSPACE_CAPACITY 4u

typedef fractal_workspace_id fractal_service_id;
typedef fractal_workspace_id fractal_device_id;
typedef fractal_workspace_id fractal_session_id;
typedef fractal_workspace_id fractal_invitation_id;
typedef fractal_workspace_id fractal_handshake_id;
typedef fractal_workspace_id fractal_credential_id;
typedef fractal_workspace_id fractal_transcript_id;

typedef enum fractal_identity_status { FRACTAL_IDENTITY_CURRENT = 0, FRACTAL_IDENTITY_SUPERSEDED, FRACTAL_IDENTITY_REVOKED } fractal_identity_status;
typedef enum fractal_device_platform { FRACTAL_DEVICE_DESKTOP = 0, FRACTAL_DEVICE_ANDROID, FRACTAL_DEVICE_REMOTE_DESKTOP, FRACTAL_DEVICE_UNKNOWN } fractal_device_platform;
typedef enum fractal_attestation_status { FRACTAL_ATTESTATION_NOT_PRESENT = 0, FRACTAL_ATTESTATION_SOFTWARE, FRACTAL_ATTESTATION_HARDWARE, FRACTAL_ATTESTATION_FAILED } fractal_attestation_status;
typedef enum fractal_enrollment_state { FRACTAL_ENROLLMENT_UNKNOWN = 0, FRACTAL_ENROLLMENT_PENDING, FRACTAL_ENROLLMENT_APPROVED, FRACTAL_ENROLLMENT_REVOKED } fractal_enrollment_state;
typedef enum fractal_invitation_status { FRACTAL_INVITATION_CREATED = 0, FRACTAL_INVITATION_ADVERTISED, FRACTAL_INVITATION_CLAIMED, FRACTAL_INVITATION_APPROVED, FRACTAL_INVITATION_DENIED, FRACTAL_INVITATION_EXPIRED, FRACTAL_INVITATION_CONSUMED, FRACTAL_INVITATION_REVOKED } fractal_invitation_status;
typedef enum fractal_pairing_stage { FRACTAL_PAIRING_UNKNOWN = 0, FRACTAL_PAIRING_DISCOVERED, FRACTAL_PAIRING_INVITATION_PRESENTED, FRACTAL_PAIRING_KEY_PROOF_VERIFIED, FRACTAL_PAIRING_HUMAN_CONFIRMATION_REQUIRED, FRACTAL_PAIRING_APPROVED, FRACTAL_PAIRING_CREDENTIAL_ISSUED, FRACTAL_PAIRING_ACTIVE, FRACTAL_PAIRING_FAILED } fractal_pairing_stage;
typedef enum fractal_pairing_message_kind { FRACTAL_PAIRING_CLIENT_HELLO = 0, FRACTAL_PAIRING_SERVICE_HELLO, FRACTAL_PAIRING_DEVICE_ENROLLMENT_REQUEST, FRACTAL_PAIRING_SERVICE_CHALLENGE, FRACTAL_PAIRING_DEVICE_PROOF, FRACTAL_PAIRING_SERVICE_PROOF, FRACTAL_PAIRING_HUMAN_VERIFICATION_PENDING, FRACTAL_PAIRING_APPROVAL, FRACTAL_PAIRING_CREDENTIAL_ISSUANCE, FRACTAL_PAIRING_COMPLETE } fractal_pairing_message_kind;
typedef enum fractal_human_verification_method { FRACTAL_VERIFY_MATCHING_NUMERIC_CODE = 0, FRACTAL_VERIFY_MATCHING_WORD_SEQUENCE, FRACTAL_VERIFY_QR_INVITATION, FRACTAL_VERIFY_LOCAL_OPERATOR_APPROVAL } fractal_human_verification_method;
typedef enum fractal_credential_status { FRACTAL_CREDENTIAL_ACTIVE = 0, FRACTAL_CREDENTIAL_EXPIRED, FRACTAL_CREDENTIAL_REVOKED, FRACTAL_CREDENTIAL_SUPERSEDED, FRACTAL_CREDENTIAL_SUSPENDED } fractal_credential_status;
typedef enum fractal_pairing_rejection { FRACTAL_PAIRING_REJECT_NONE = 0, FRACTAL_PAIRING_REJECT_STRUCTURE, FRACTAL_PAIRING_REJECT_VERSION, FRACTAL_PAIRING_REJECT_ORDER, FRACTAL_PAIRING_REJECT_INVITATION, FRACTAL_PAIRING_REJECT_IDENTITY, FRACTAL_PAIRING_REJECT_TRANSCRIPT, FRACTAL_PAIRING_REJECT_NONCE_REPLAY, FRACTAL_PAIRING_REJECT_PROOF, FRACTAL_PAIRING_REJECT_HUMAN_VERIFICATION, FRACTAL_PAIRING_REJECT_CAPABILITY, FRACTAL_PAIRING_REJECT_DENIED, FRACTAL_PAIRING_REJECT_CAPACITY, FRACTAL_PAIRING_REJECT_REVOKED } fractal_pairing_rejection;

typedef struct fractal_key_fingerprint { uint8_t bytes[FRACTAL_PAIRING_FINGERPRINT_CAPACITY]; } fractal_key_fingerprint;
typedef struct fractal_pairing_nonce { uint8_t bytes[FRACTAL_PAIRING_NONCE_CAPACITY]; } fractal_pairing_nonce;

typedef struct fractal_service_identity { fractal_service_id id; char display_name[FRACTAL_PAIRING_NAME_CAPACITY]; fractal_key_fingerprint public_key_fingerprint; uint32_t identity_version; uint32_t key_version; uint64_t created_tick; fractal_identity_status status; } fractal_service_identity;
typedef struct fractal_device_identity { fractal_device_id id; fractal_participant_id participant_id; fractal_key_fingerprint public_key_fingerprint; fractal_device_platform platform; fractal_attestation_status attestation; fractal_enrollment_state enrollment; bool revoked; uint32_t key_version; } fractal_device_identity;

typedef struct fractal_pairing_invitation { fractal_invitation_id id; fractal_service_id service_id; uint32_t service_key_version; fractal_participant_role requested_role; fractal_workspace_capabilities capability_ceiling; uint64_t created_tick; uint64_t expires_tick; bool used; fractal_pairing_nonce nonce; uint32_t human_code; fractal_invitation_status status; fractal_invitation_id identity; uint32_t failed_attempts; uint32_t brute_force_limit; } fractal_pairing_invitation;

typedef struct fractal_pairing_credential { fractal_credential_id id; fractal_participant_id participant_id; fractal_device_id device_id; fractal_service_id service_id; uint32_t service_key_version; uint32_t device_key_version; fractal_workspace_capabilities capabilities; fractal_workspace_id allowed_workspaces[FRACTAL_PAIRING_ALLOWED_WORKSPACE_CAPACITY]; uint32_t allowed_workspace_count; uint64_t issued_tick; uint64_t expires_tick; uint64_t revocation_epoch; fractal_credential_status status; } fractal_pairing_credential;

typedef union fractal_pairing_message_payload { fractal_pairing_nonce nonce; fractal_pairing_invitation invitation; fractal_device_identity device; fractal_workspace_capabilities capabilities; struct { uint32_t code; bool approved; fractal_participant_role role; fractal_workspace_capabilities capabilities; uint64_t lifetime_ticks; } approval; fractal_pairing_credential credential; } fractal_pairing_message_payload;

typedef struct fractal_pairing_message { uint32_t protocol_version; fractal_pairing_message_kind kind; fractal_handshake_id handshake_id; fractal_service_id service_id; fractal_device_id device_id; fractal_invitation_id invitation_id; fractal_transcript_id previous_transcript; fractal_pairing_nonce nonce; uint64_t sequence; uint64_t expires_tick; fractal_pairing_message_payload payload; fractal_transcript_id identity; bool prevalidated_proof; } fractal_pairing_message;

typedef struct fractal_pairing_state { uint32_t protocol_version; fractal_service_identity service; fractal_pairing_invitation invitation; fractal_pairing_stage stage; fractal_handshake_id handshake_id; fractal_device_identity device; fractal_transcript_id transcript; fractal_pairing_nonce seen_nonces[16]; uint32_t seen_nonce_count; uint64_t next_sequence; bool device_proof_verified; bool service_proof_verified; bool human_verified; fractal_pairing_credential credentials[FRACTAL_PAIRING_CREDENTIAL_CAPACITY]; uint32_t credential_count; uint64_t revocation_epoch; } fractal_pairing_state;

typedef struct fractal_pairing_transition { bool accepted; fractal_pairing_rejection rejection; fractal_pairing_stage previous_stage; fractal_pairing_stage resulting_stage; fractal_transcript_id transcript; bool credential_issued; fractal_credential_id credential_id; } fractal_pairing_transition;

bool fractal_pairing_id_equal(fractal_workspace_id left, fractal_workspace_id right);
fractal_pairing_invitation fractal_pairing_invitation_create(fractal_service_id service_id, uint32_t service_key_version, fractal_participant_role role, fractal_workspace_capabilities ceiling, uint64_t created_tick, uint64_t expires_tick, fractal_pairing_nonce nonce, uint32_t human_code);
fractal_transcript_id fractal_pairing_message_identity(const fractal_pairing_message *message);
int fractal_pairing_state_init(fractal_pairing_state *state, fractal_service_identity service, fractal_pairing_invitation invitation);
fractal_pairing_transition fractal_pairing_reduce(fractal_pairing_state *state, const fractal_pairing_message *message, uint64_t now_tick);
int fractal_pairing_revoke_credential(fractal_pairing_state *state, fractal_credential_id credential_id, uint64_t revocation_epoch);
#ifdef __cplusplus
}
#endif
#endif
