#include "fake_tls.h"
void fractal_fake_tls_init(fractal_fake_tls*tls){if(tls){tls->state=FRACTAL_FAKE_TLS_NEW;tls->channel_binding=0;tls->rejection=FRACTAL_SECURITY_ACCEPTED;}}
fractal_transport_security_rejection fractal_fake_tls_handshake(fractal_fake_tls*tls,fractal_security_policy p){fractal_transport_security_rejection r=fractal_security_policy_validate(p);if(!tls)return r;tls->state=FRACTAL_FAKE_TLS_HANDSHAKING;tls->rejection=r;if(r==FRACTAL_SECURITY_ACCEPTED){tls->state=FRACTAL_FAKE_TLS_AUTHENTICATED;tls->channel_binding=UINT64_C(0x6672616374616c31);}else tls->state=FRACTAL_FAKE_TLS_REJECTED;return r;}
bool fractal_fake_tls_can_write_application_data(const fractal_fake_tls*tls){return tls&&tls->state==FRACTAL_FAKE_TLS_AUTHENTICATED;}
