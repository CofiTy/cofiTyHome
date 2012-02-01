#include <unistd.h>

#include "init.h"

int main(int argc, char ** argv){


	init();
	sleep(120);
	destroy();
	return 0;
}

