#include "interrupt.h"
int it = 0;

void disable_it()
{
	it++;
	irq_disable();
}
void enable_it()
{
	if (--it <= 0)
	{
		it = 0;
		irq_enable();
	}
}
void reset_it()
{
	it = 0;
	irq_enable();
}
