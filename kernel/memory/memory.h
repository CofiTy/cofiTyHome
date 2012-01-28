#ifndef _MEMORY_H_
#define _MEMORY_H_

/* TODO: malloc accepts a size request without checking its plausibility; free believes that the block it is asked to free contains a valid size field. Improve these routines so they make more pains with error checking.

   Développer calloc
   Remplacer sbrk par nmap
 */

#define OK 1
#define ERROR -1

#define SIZE_ALLOC 5000000

void initMemory();

void *gMalloc(unsigned nbytes);

void gFree(void *ap);

int getGMemTotal();

int getGMemFree();

#endif
