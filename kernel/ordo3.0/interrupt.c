#include "interrupt.h"


/*
 * The level of interruption inhibition
 */
int it = 0;

/*
 * Increase the level,
 * and disable interruptions
 */
void disable_it()
{
	it++;
	irq_disable();
}

/*
 * Decrease the level,
 * and if it's null then
 * enable interruption
 */
void enable_it()
{
	if (--it <= 0)
	{
		it = 0;
		irq_enable();
	}
}

/*
 * Reset the interrupt inhibition level to 0
 */
void reset_it()
{
	it = 0;
	irq_enable();
}
