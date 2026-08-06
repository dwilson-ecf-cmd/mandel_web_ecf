#include "fractal/desktop_client_core.h"
#include "fractal/local_workspace_service.h"
#include <assert.h>
#include <string.h>
int main(void){fractal_desktop_client_summary s;uint8_t payload[3]={1,2,3},frame[16],out[16];size_t w=0,ps=0,c=0;assert(fractal_desktop_client_run_reference_scenario(&s)==0);assert(s.semantic_drift==0&&s.replay_explicit);assert(fractal_local_frame_encode(payload,3,frame,sizeof(frame),&w)==0);assert(fractal_local_frame_decode(frame,w,out,sizeof(out),&ps,&c)==0);assert(ps==3&&c==w&&memcmp(payload,out,3)==0);assert(fractal_local_frame_decode(frame,1,out,sizeof(out),&ps,&c)!=0);frame[0]=0x7f;assert(fractal_local_frame_decode(frame,w,out,sizeof(out),&ps,&c)==-2);return 0;}
