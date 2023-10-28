#include <assert.h>
#include <stdlib.h>

#include "Stack.h"
#include "StackDump.h"

const int coeffIncrease = 2;
const int coeffDecrease = 2;

#ifdef STK_PROTECT
const canary_t NUM_CANARY_STACK = 0xDEDABABA;
const canary_t NUM_CANARY_DATA  = 0xBADCAFE;
#endif

const int initialCapacity = 4;

#ifdef STK_PROTECT
hash_t GetValueHash(const myStack * stk)
{
    assert(stk       != NULL);
    assert(stk->data != NULL);

    const int initialValue = 5831;
    const int coeffChange = 33;

    hash_t hash = initialValue;

    for (int i = 0; i < sizeof(myStack); i++)
    {
        assert(0 <= i && i < sizeof(myStack));

        hash = coeffChange * hash + *((char *)(stk) + i);
    }

    for (int i = 0; i < stk->capacity; i++)
    {
        assert(0 <= i && i < sizeof(myStack));

        hash = coeffChange * hash + *((char *)(stk->data) + i);
    }

    return hash;
}

bool HashCheck(myStack * stk)
{
    assert(stk != NULL);

    hash_t hashRef = stk->hash;

    stk->hash = 0;

    return hashRef != GetValueHash(stk);
}
#endif

int StackCheck(myStack * stk)
{
    int err = NO_ERR;

    if(stk == NULL)                                      return STACK_NULL;
    if(stk->data == NULL)                                return DATA_NULL;

    if(stk->sizeStack > stk->capacity)                   err |= SIZE_LARGE_CAPACITY;
    if(stk->capacity <= 0)                               err |= CAPACITY_ZERO;
    if(stk->sizeStack < 0)                               err |= SIZE_NEGATIVE;

    #ifdef STK_PROTECT
    if(stk->leftCanary != NUM_CANARY_STACK
    || stk->rightCanary != NUM_CANARY_STACK)
                                                         err |= CANARY_ERR_STK;

    if(((canary_t *)stk->data)[-1] != NUM_CANARY_DATA
    || *(canary_t *)(((elem_t *)(stk->data)) + stk->capacity) != NUM_CANARY_DATA)
                                                         err |= CANARY_ERR_DATA;
    if(HashCheck(stk))                                   err |= HASH_ERR;
    #endif

    return err;
}

#ifdef STK_PROTECT
void SetCanary(myStack * stk)
{
    assert(stk         != NULL);
    assert(stk -> data != NULL);

    ((canary_t *)(stk->data))[-1] = NUM_CANARY_DATA;
    *(canary_t *)(((elem_t *)(stk->data)) + stk -> capacity) = NUM_CANARY_DATA;
}
#endif

size_t GetDataSize(myStack * stk)
{
    assert(stk != NULL);

    size_t sizeCanary = 0;

    #ifdef STK_PROTECT
    sizeCanary += 2 * sizeof(canary_t);
    #endif

    return stk->capacity * sizeof(elem_t) + sizeCanary;
}

void FillPoison(myStack * stk)
{
    assert(stk       != NULL);
    assert(stk->data != NULL);

    for(int i = stk->sizeStack; i < stk->capacity; i++)
    {
        assert(stk->sizeStack <= i && i < stk->capacity);

        stk->data[i] = POISON;
    }

}

int StackCtor(myStack * stk)
{
    int err = NO_ERR;

    if(stk == NULL)
    {
        err = STACK_NULL;
        STACK_DUMP(stk, err);
        return err;
    }

    #ifdef STK_PROTECT
    stk->leftCanary  = NUM_CANARY_STACK;
    stk->rightCanary = NUM_CANARY_STACK;
    #endif

    stk->sizeStack = 0;
    stk->capacity  = initialCapacity;

    stk->data = (elem_t *)(calloc(GetDataSize(stk), sizeof(char)));

    if(stk->data == NULL)
    {
        err = DATA_NULL;
        STACK_DUMP(stk, err);
        return err;
    }

    #ifdef STK_PROTECT
    stk->data = (elem_t *)((char *)stk->data + sizeof(canary_t));
    #endif

    FillPoison(stk);

    #ifdef STK_PROTECT
    SetCanary(stk);
    stk->hash = GetValueHash(stk);
    #endif

    return NO_ERR;
}

int StackDtor(myStack * stk)
{
    int err = NO_ERR;

    if(stk == NULL)
    {
        err = STACK_NULL;
        STACK_DUMP(stk, err);
        return err;
    }

    if(stk->data == NULL)
    {
        err = DATA_NULL;
        STACK_DUMP(stk, err);
        return err;
    }

    #ifdef STK_PROTECT
    stk->leftCanary = -1;
    stk->rightCanary = -1;
    stk->hash = -1;
    #endif

    stk->sizeStack = -1;
    stk->capacity = -1;

    #ifdef STK_PROTECT
    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));
    #endif

    free(stk->data);

    return NO_ERR;
}


int StackReallocIfNeed(myStack * stk)
{
    assert(stk       != NULL);
    assert(stk->data != NULL);

    if(stk->sizeStack == stk->capacity)
        stk->capacity *= coeffIncrease;

    else if(coeffDecrease * stk->sizeStack  < stk->capacity && stk->sizeStack > initialCapacity)
        stk->capacity /= coeffDecrease;

    else
        return NO_ERR;

    #ifdef STK_PROTECT
    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));
    #endif

    stk->data = (elem_t *)(realloc(stk->data, GetDataSize(stk)));

    if(stk->data == NULL)
    {
        int err = DATA_NULL;
        STACK_DUMP(stk, err);
        return err;
    }

    #ifdef STK_PROTECT
    stk->data = (elem_t *)((char *)stk->data + sizeof(canary_t));
    #endif

    FillPoison(stk);

    #ifdef STK_PROTECT
    SetCanary(stk);
    #endif

    return NO_ERR;
}

int StackPush(myStack * stk, elem_t value)
{
    int err = StackCheck(stk);

    if(err != NO_ERR)
    {
        STACK_DUMP(stk, err);
        return err;
    }

    err = StackReallocIfNeed(stk);

    if(err != NO_ERR)
    {
        STACK_DUMP(stk, err);
        return err;
    }

    stk->data[stk->sizeStack] = value;

    stk->sizeStack++;

    #ifdef STK_PROTECT
    stk->hash = GetValueHash(stk);
    #endif

    return NO_ERR;
}

int StackPop(myStack * stk, elem_t * retValue)
{
    assert(stk         != NULL);
    assert(stk->data   != NULL);
    assert(retValue    != NULL);    // ???

    int err = StackCheck(stk);

    if(err != NO_ERR)
    {
        STACK_DUMP(stk, err);
        return err;
    }

    --stk->sizeStack;

    if(stk->sizeStack < 0)
    {
        err = SIZE_NEGATIVE;
        STACK_DUMP(stk, err);
        return err;
    }

    *retValue = stk->data[stk->sizeStack];

    stk->data[stk->sizeStack] = POISON;

    err = StackReallocIfNeed(stk);

    if(err != NO_ERR)
    {
        STACK_DUMP(stk, err);
        return err;
    }

    #ifdef STK_PROTECT
    stk->hash = GetValueHash(stk);
    #endif

    return NO_ERR;
}

size_t GetSizeStack()
{
    return sizeof(myStack);
}
