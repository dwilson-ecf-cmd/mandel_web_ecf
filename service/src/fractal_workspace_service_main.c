#include "fractal/local_workspace_service.h"
#include <stdio.h>
int main(void){fractal_local_service s;fractal_local_service_status st;fractal_local_service_init(&s,1u);if(fractal_local_service_open(&s)!=0)return 1;st=fractal_local_service_get_status(&s);printf("{\"service\":\"%s\",\"workspace\":\"%s\",\"restart_epoch\":%llu,\"tcp_listener\":false}\n",fractal_service_lifecycle_name(st.lifecycle),fractal_workspace_lifetime_state_name(st.workspace_lifecycle),(unsigned long long)st.restart_epoch);return 0;}
