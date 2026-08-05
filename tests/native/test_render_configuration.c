#include "fractal/workspace_controller.h"
#include <assert.h>
#include <string.h>
int main(void){fractal_render_configuration a,b;char name[96];assert(fractal_render_configuration_default(&a)==0);assert(fractal_render_configuration_validate(&a)==0);assert(fractal_render_configuration_save_local(&a,"local_config_test",name,sizeof(name))==0);assert(strstr(name,"..")==0);assert(fractal_render_configuration_load_local(&b,"local_config_test",a.identity)==0);assert(memcmp(&a,&b,sizeof(a))==0);b.checksum++;assert(fractal_render_configuration_validate(&b)!=0);return 0;}
