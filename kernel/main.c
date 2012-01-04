//
//  main.c
//  kernel
//
//  Created by Vincent Bailly on 24/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "myThreads.h"
#include <signal.h>

void child();
void child2();

int main()
{
    init_threading_system();
    createThread(&child);
    createThread(&child2);
    launch_threads();
    return 1;
}

void child()
{
    for (;;)
    {
        printf(" 1");  
        yield();
    }
}

void mess(void)
{
    printf("\t in mess\n");
    yield();
}

void child2()
{
    for (;;)
    {
        printf("before mess");
        mess();
        printf("after mess");
    }
}
