#ifndef TRY_H
#define TRY_H
#include "context.h"


/*
 * An implementation of try and trow function
 */
typedef int(fct)(void);
/*
 * Restore previous saved context
 * This function never returns
 * parameters:
 * 	cont: The context to restore
 * 	value: the value to be returned by the function try
 */
void throw(ctx *cont, int value);
/*
 * Save the current context and call the function f
 * parameters:
 * 	cont: where to save the context
 * 	fct: the function to call
 * return value:
 * 	- the f return value
 * 	- the second parameter of throw if f doesn't return (throw is called)
 */
int try(ctx *cont,fct *f);

#endif
