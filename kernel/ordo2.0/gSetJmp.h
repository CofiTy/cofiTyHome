#ifndef GSETJMPT_H_
#define GSETJMPT_H_
#include "gSetJmp.h"

typedef int gBuffer[6];
int gSetJmp(gBuffer buffer)__attribute__ ((returns_twice));
void gLongJmp(gBuffer buffer,int value)__attribute__ ((noreturn));

#endif /* GSETJMP_H_ */
