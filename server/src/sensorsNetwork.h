#if ! defined ( SNET_H_ )
#define SNET_H_

#include "common.h"

/**
 * Initialisation of the Network
 **/
void sensorsNetworkStart();

/**
 * Desactivation of Network and free related memory
 **/
void sensorsNetworkStop();

/**
 * Send Message toward Sensors
 **/
int sensorsNetworkSend(const char * msg_ptr, size_t msg_len);

#endif /*SNET*/
