#if ! defined ( SNET_H_ )
#define SNET_H_

#include common.h

void sensorsNetworkStart();
void sensorsNetworkStop();
int sensorsNetworkSend(const char * msg_ptr, size_t msg_len);

#endif /*SNET*/
