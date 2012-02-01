#include <unistd.h>

#include "guiNetwork.h"
#include "sensorsNetwork.h"
#include "../parse/rules.tab.h"
#include "sensors.h"

#include "../../kernel/memory/memory.h"
#include "actionneurs.h"

int main(int argc, char ** argv){
	
	initMemory();
  parseAll();


	printf("MemTotale: %ld\n", getGMemTotal());
	printf("MemFree: %ld\n", getGMemFree());

    sensorsNetworkStart();
    guiNetworkStart();
    sleep(120);
    guiNetworkStop();
    sensorsNetworkStop();



  return 0;
}

