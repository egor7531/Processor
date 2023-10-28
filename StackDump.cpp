#include <assert.h>
#include <stdio.h>

#include "StackDump.h"
#include "Stack.h"

#define SPEC_DATA     "%d"
#define SPEC_POSION   "%d"
#define SPEC_CANARY   "%x"
#define SPEC_HASH     "%llu"

void PrintError(int err)
{
    printf("status: ");

    if(err == NO_ERR)
        printf("No error\n");

    if(err & STACK_NULL)
        printf("Pointer on stack NULL\n");

    if(err & DATA_NULL)
        printf("Pointer on data NULL\n");

    if(err & SIZE_LARGE_CAPACITY)
        printf("Size > Capacity\n");

    if(err & CAPACITY_ZERO)
        printf("Capacity is negative or zero\n");

    if(err & SIZE_NEGATIVE)
        printf("Size is negative\n");

    if(err & CANARY_ERR_STK)
        printf("Value of Canary in struct change\n");

    if(err & CANARY_ERR_DATA)
        printf("Value of Canary in array of Data change\n");

    if(err & HASH_ERR)
        printf("Hash change\n");
}

void StackDump(myStack * stk, const char * nameFile, const char * nameFunc, const size_t line, int err)
{
    assert(stk         != NULL);
    assert(stk -> data != NULL);

    printf("\n");
    printf("called from %s %s(%d)\n", nameFile, nameFunc, line);
    printf("stack[%p]\n", stk);
    printf("size = %d\n", stk->sizeStack);
    printf("capacity = %d\n", stk->capacity);
    printf("data[%p]\n", stk->data);

    #ifdef STK_PROTECT
    printf("leftCanary = " SPEC_CANARY "\n", stk->leftCanary);
    printf("rightCanary = " SPEC_CANARY "\n", stk->rightCanary);
    printf("hash = " SPEC_HASH "\n", stk->hash);
    #endif

    PrintError(err);

    #ifdef STK_PROTECT
    printf("%p <" SPEC_CANARY ">\n", ((canary_t *)(stk -> data)) + -1, ((canary_t *)(stk -> data))[-1]);
    #endif

    for(int i = 0; i < stk -> capacity; i++)
    {
        assert(0 <= i && i < stk -> capacity);

       if(stk -> data[i] == POISON)
            printf("%p [%d] = " SPEC_POSION "(POISON)\n", stk -> data + i, i, stk -> data[i]);

        else
            printf("%p *[%d] = " SPEC_DATA "\n", stk -> data + i, i, stk -> data[i]);
    }

    #ifdef STK_PROTECT
    printf("%p <" SPEC_CANARY ">\n", stk -> data + stk -> capacity, stk -> data[stk -> capacity]);
    #endif

    printf("\n");
}
