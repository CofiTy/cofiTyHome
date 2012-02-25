#if ! defined ( GNET_H_ )
#define GNET_H_

#include <mqueue.h>
#include "common.h"

/**
 * Initialisation of the GUI Network
 **/
void guiNetworkStart();

/**
 * Desactivation of GUI Network and free related memory
 **/
void guiNetworkStop();

/**
 * Send Message toward GUI Client
 **/
int guiNetworkSend(const char * msg_ptr, size_t msg_len, mqd_t mqId);

#endif /*GNET*/
