#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "hw.h"

/*
 * Library for disable interrupt,
 * It is based on hw.h and add multiple 
 * enable and disable in the same time
 */

void disable_it();
/*
 * Increase the level,
 * and disable interruptions
 */

void enable_it();
/*
 * Decrease the level,
 * and if it's null then
 * enable interruption
 */

void reset_it();
/*
 * Reset the interrupt inhibition level to 0
 */

#endif
