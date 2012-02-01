#ifndef _MEMORY_H_
#define _MEMORY_H_

/**
 * Size of program memory, in bytes.
 * In unsigned long, do not use more.
 */
#define SIZE_ALLOC 5000000

/**
 * Initialise memory functions (pointers and mutex).
 * Use only once.
 */
void initMemory();

/**
 * Stop all memory management.
 */
void destroyMemory();

/**
 * Allocate a chunk of memory. NULL if too big
 * for remaining memory.
 */
void *gMalloc(unsigned long nbytes);

/**
 * Free a chunk of memory.
 * Please do not gFree not gMalloced pointers...
 */
void gFree(void *ap);

/**
 * Get total memory (currently SIZE_ALLOC).
 */
unsigned long getGMemTotal();

/**
 * Get free memory.
 */
unsigned long getGMemFree();

#endif
