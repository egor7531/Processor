#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#define DEBUG
//#define STK_PROTECT

typedef int elem_t;

#ifdef STK_PROTECT
typedef unsigned long long hash_t;
typedef unsigned long long canary_t;
#endif

const elem_t POISON = -777;

struct myStack
{
    #ifdef STK_PROTECT
    canary_t leftCanary;
    #endif

    elem_t * data;
    int sizeStack;
    int capacity;

    #ifdef STK_PROTECT
    hash_t hash;
    canary_t rightCanary;
    #endif

};

enum stack_error_t
{
    NO_ERR              = 0 << 0,
    STACK_NULL          = 1 << 0,
    DATA_NULL           = 1 << 1,
    SIZE_LARGE_CAPACITY = 1 << 2,
    CAPACITY_ZERO       = 1 << 3,
    SIZE_NEGATIVE       = 1 << 4,
    CANARY_ERR_STK      = 1 << 5,
    CANARY_ERR_DATA     = 1 << 6,
    HASH_ERR            = 1 << 7
};

int StackCtor(myStack * stk);
int StackDtor(myStack * stk);
int StackPush(myStack * stk, elem_t value);
int StackPop(myStack * stk, elem_t * RetValue);

#endif // STACK_H_INCLUDED
