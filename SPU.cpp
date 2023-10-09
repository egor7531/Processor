#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Stack.h"

typedef double register_t;

struct myRegisters
{
    register_t rax;
    register_t rbx;
    register_t rcx;
    register_t rdx;
};

struct myProcessor
{
    myRegisters reg;
    myStack     stk;
};

enum command_t
{
    HLT = -1,
    IN  =  0,
    PUSH = 1,
    DIV =  2,
    SUB =  3,
    OUT =  4,
    ADD =  5,
    MUL =  6,
    POP = 11
};

void GetResult(FILE * fp, myProcessor * prc);
void ExecuteCommand(FILE * fp, const int command, myProcessor * prc);
void Sub(myProcessor * prc);
void Mul(myProcessor * prc);
void Add(myProcessor * prc);
void Div(myProcessor * prc);
void Out(myProcessor * prc);
void In(myProcessor * prc);
void Push(FILE * fp, myProcessor * prc);
void Pop(myProcessor * prc);
void ProcessorCtor(myProcessor * prc);

int main()
{
    const char * nameFile = "ByteCode.txt";

    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    myProcessor prc = {};

    ProcessorCtor(&prc);

    GetResult(fp, &prc);

    fclose(fp);

// ProcessorDtor();

    return 0;
}

void Push(FILE * fp, myProcessor * prc)
{
    assert(prc != NULL);
    assert(fp  != NULL);

    double value = 0;

    fscanf(fp, "%lf", &value);

    StackPush(&prc->stk, value);
}

void Pop(myProcessor * prc)
{
    assert(prc != NULL);


}

void In(myProcessor * prc)
{
    assert(prc != NULL);

    double value = 0;

    printf("Enter number: ");
    scanf("%lf", &value);

    StackPush(&prc->stk, value);
}

void Out(myProcessor * prc)
{
    assert(prc != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value = 0;

    StackPop(&prc->stk, &value);

    fprintf(fp, "%.2lf", value);

    fclose(fp);
}

void Div(myProcessor * prc)
{
    assert(prc != NULL);

    double value1 = 0;
    double value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 / value1);
}

void Add(myProcessor * prc)
{
    assert(prc != NULL);

    double value1 = 0;
    double value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 + value1);
}

void Mul(myProcessor * prc)
{
    assert(prc != NULL);

    double value1 = 0;
    double value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 * value1);
}

void Sub(myProcessor * prc)
{
    assert(prc != NULL);

    double value1 = 0;
    double value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 - value1);
}

void GetResult(FILE * fp, myProcessor * prc)
{
    int command = 0;

    while(fscanf(fp, "%d", &command) != EOF && command != HLT)
        ExecuteCommand(fp, command, prc);
}

void ExecuteCommand(FILE * fp, const int command, myProcessor * prc)
{
    assert(fp  != NULL);
    assert(prc != NULL);

    switch(command)
    {
    case IN: In(prc);
            break;
    case PUSH: Push(fp, prc);
            break;
    case OUT: Out(prc);
            break;
    case DIV: Div(prc);
            break;
    case ADD: Add(prc);
            break;
    case MUL: Mul(prc);
            break;
    case SUB: Sub(prc);
            break;
    case POP: Pop(prc);
            break;
    default: printf("Unkown command\n");
            break;
    }
}

void ProcessorCtor(myProcessor * prc)
{
    assert(prc != NULL);

    /*prc->reg->rax = 0;
    prc->reg->rbx = 0;
    prc->reg->rcx = 0;
    prc->reg->rdx = 0;*/

    StackCtor(&prc->stk);
}
