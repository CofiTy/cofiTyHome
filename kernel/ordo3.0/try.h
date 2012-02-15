#ifndef TRY_H
#define TRY_H
#include "context.h"

typedef int(fct)(void);
void throw(ctx *cont, int value);
int try(ctx *cont,fct *f);

#endif
