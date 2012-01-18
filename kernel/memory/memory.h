#ifndef _MEMORY_H_
#define _MEMORY_H_

/* TODO: malloc accepts a size request without checking its plausibility; free believes that the block it is asked to free contains a valid size field. Improve these routines so they make more pains with error checking.

   Développer calloc
   Remplacer sbrk par nmap
 */

void initMemory();

void *gMalloc(unsigned nbytes);

void gFree(void *ap);

#endif
