#if ! defined ( GNET_H_ )
#define GNET_H_

#include "common.h"

void guiNetworkStart();
void guiNetworkStop();
int guiNetworkSend(const char * msg_ptr, size_t msg_len);

#endif /*GNET*/
