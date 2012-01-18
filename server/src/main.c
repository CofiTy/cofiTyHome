#include <unistd.h>
#include "sensorsNetwork.h"

int main(int argc, char ** argv){
  sensorsNetworkStart();
  sleep(120);
  sensorsNetworkStop();
  return 0;
}

