#ifndef STACKDUMP_H_INCLUDED
#define STACKDUMP_H_INCLUDED

#include "Stack.h"

#define STACK_DUMP(stk, err) StackDump(stk, __FILE__, __func__, __LINE__, err)

void StackDump(myStack * stk, const char * nameFile, const char * nameFunc, const size_t line, int err);

#endif // STACKDUMP_H_INCLUDED
