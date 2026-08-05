#include "fractal/local_workspace_service.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
int fractal_local_transport_posix_contract_symbol(void){struct sockaddr_un addr; (void)addr; return (int)FRACTAL_LOCAL_SERVICE_FRAME_LIMIT;}
