#include "fractal/local_workspace_service.h"
#include <assert.h>
int main(void){fractal_local_service a,b;fractal_local_checkpoint c;uint64_t d1,d2;fractal_local_service_init(&a,1);assert(fractal_local_service_open(&a)==0);d1=fractal_local_semantic_digest(&a);assert(fractal_local_checkpoint_create(&a,&c)==0);assert(fractal_local_checkpoint_restore(&b,&c,2)==0);d2=fractal_local_semantic_digest(&b);assert(d1==d2);assert(b.restart_epoch==2);c.checksum++;assert(fractal_local_checkpoint_restore(&b,&c,3)!=0);return 0;}
