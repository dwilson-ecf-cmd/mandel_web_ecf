#include "fractal/validation/transport_validation.h"
#include <stdio.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#define CHECK(x) do{if(!(x)){printf("fail %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
int main(void){uint32_t i,drift=0;for(i=0;i<fractal_validation_scenario_count();i++){const fractal_validation_scenario*s=fractal_validation_scenario_at(i);fractal_validation_semantic_result fake,local;fractal_validation_drift_report r;CHECK(fractal_validation_run_scenario(s,FRACTAL_VALIDATION_TRANSPORT_FAKE,&fake)==0);CHECK(fractal_validation_run_scenario(s,FRACTAL_VALIDATION_TRANSPORT_UNIX_SOCKET,&local)==0);r=fractal_validation_compare(&fake,FRACTAL_VALIDATION_TRANSPORT_FAKE,&local,FRACTAL_VALIDATION_TRANSPORT_UNIX_SOCKET);drift+=r.semantic_drift_count;}
#ifndef _WIN32
{int sv[2];pid_t pid;char b='v';CHECK(socketpair(AF_UNIX,SOCK_STREAM,0,sv)==0);pid=fork();CHECK(pid>=0);if(pid==0){close(sv[0]);CHECK(read(sv[1],&b,1)==1);CHECK(write(sv[1],&b,1)==1);close(sv[1]);_exit(0);}close(sv[1]);CHECK(write(sv[0],&b,1)==1);CHECK(read(sv[0],&b,1)==1);close(sv[0]);CHECK(waitpid(pid,0,0)==pid);} 
#endif
printf("local transport parity: scenarios=%u semantic drift = %u; named pipe status=COMPILE_ONLY\n",fractal_validation_scenario_count(),drift);CHECK(drift==0);return 0;}
