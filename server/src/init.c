#include <pthread.h>

#include "../../kernel/memory/memory.h"
#include "guiNetwork.h"
#include "sensorsNetwork.h"
#include "../parse/rules.tab.h"
#include "sensors.h"
#include "actions.h"
#include "actionneurs.h"
#include "initCheckRules.h"
#include "timedCheck.h"
#include "common.h"

#include "init.h"
#include "rules.h"

void init()
{
  printf("Initialisation...\n");
  initMemory();
  printf("MemTotale: %ld\n", getGMemTotal());
  printf("MemFree: %ld\n", getGMemFree());

  pthread_mutex_init(&sensorsMutex, NULL);

  parseAll();
  parseConfig();

  initRules();
  startTimedCheck(60);

  sensorsNetworkStart();
  guiNetworkStart();

}

void destroy()
{
  guiNetworkStop();
  sensorsNetworkStop();

  stopTimedCheck();
  stopRules();
  pthread_mutex_destroy(&sensorsMutex);

  cleanMemory();
  gFree(nameLogRules);

  destroyMemory();
}

void cleanMemory(){
  cleanActions();
  cleanRules();
  cleanActionneurs();
  cleanSensors();
}
