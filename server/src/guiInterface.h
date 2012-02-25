#if ! defined ( GINT_H_ )
#define GINT_H_

#include <mqueue.h>
#include "common.h"

/**
 * Process a GUI command and send response if needed
 **/
void processCommand(char * command, mqd_t mqSend);

#endif /*GINT*/
