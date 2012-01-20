#include <unistd.h>
#include "guiNetwork.h"

int main(int argc, char ** argv){
  guiNetworkStart();
  sleep(120);
  guiNetworkStop();
  return 0;
}

