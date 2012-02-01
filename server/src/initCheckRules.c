#include <pthread.h>
#include <semaphore.h>

#include "common.h"
#include "rules.h"

#include "initCheckRules.h"

static pthread_t checkR;

void * tryCheckingRules()
{
    for(;;)
    {
        sem_wait((sem_t *)&checkRules);
        applyRules();
    }
}

void initRules()
{   
    sem_init((sem_t *)&checkRules, 0, 0);

    pthread_create(&checkR, NULL, tryCheckingRules, NULL);
    pthread_detach(checkR);
}

void stopRules()
{
    pthread_cancel(checkR);
    sem_destroy(&checkRules);
}

