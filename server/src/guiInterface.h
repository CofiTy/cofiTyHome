#if ! defined ( GINT_H_ )
#define GINT_H_

#include <mqueue.h>
#include "common.h"

void processCommand(char * command, mqd_t mqSend);

#endif /*GINT*/
