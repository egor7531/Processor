#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Stack.h"
#include "StackDump.h"

struct myProcessor
{
    int * registers;
    int * instruct;
    myStack stk;
};

const int CHECKCODE = 15;
const int countRegisters = 10;
const int IMM = 1 << 4;
const int REG = 1 << 5;
//const uint8_t OSY = 1 << 6;

enum opcode_t
{
    HLT = 12,
    IN  =  2,
    PUSH = 1,
    DIV =  3,
    SUB =  4,
    OUT =  7,
    ADD =  5,
    MUL =  6,
    POP = 11
};

int GetFileSize(FILE * fp);
void WriteInstuction(myProcessor * prc);
void ProcessorCtor(myProcessor * prc);
void ProcessorDtor(myProcessor * prc);
void ProcessorDump(myProcessor * prc);
void GetResult(myProcessor * prc);
void ExecuteCommand(myProcessor * prc, const int command);
void Sub(myProcessor * prc);
void Mul(myProcessor * prc);
void Add(myProcessor * prc);
void Div(myProcessor * prc);
void Out(myProcessor * prc);
void In(myProcessor * prc);
void Push(myProcessor * prc, const int command);
void Pop(myProcessor * prc);

int main()
{
    myProcessor prc = {};

    ProcessorCtor(&prc);

    GetResult(&prc);

    ProcessorDtor(&prc);

    return 0;
}

void Push(myProcessor * prc, const int command)
{
    assert(prc != NULL);
    assert(command > 0);

    prc->instruct++;

    if(command & IMM)
        StackPush(&prc->stk, *(prc->instruct ));

    else if(command & REG)
        StackPush(&prc->stk, prc->registers[*(prc->instruct) - 1]);

    else
        printf("Syntax error\n");
}

void Pop(myProcessor * prc)
{
    assert(prc != NULL);

    int value = 0;

    StackPop(&prc->stk, &value);

    prc->instruct++;

    prc->registers[*(prc->instruct) - 1] = value;
}

void In(myProcessor * prc)
{
    assert(prc != NULL);

    int value = 0;

    printf("Enter number: ");
    scanf("%d", &value);

    StackPush(&prc->stk, value);
}

void Out(myProcessor * prc)
{
    assert(prc != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    int value = 0;

    StackPop(&prc->stk, &value);

    fprintf(fp, "%d", value);

    fclose(fp);
}

void Div(myProcessor * prc)
{
    assert(prc != NULL);

    int value1 = 0;
    int value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 / value1);
}

void Add(myProcessor * prc)
{
    assert(prc != NULL);

    int value1 = 0;
    int value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 + value1);
}

void Mul(myProcessor * prc)
{
    assert(prc != NULL);

    int value1 = 0;
    int value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 * value1);
}

void Sub(myProcessor * prc)
{
    assert(prc != NULL);

    int value1 = 0;
    int value2 = 0;

    StackPop(&prc->stk, &value1);
    StackPop(&prc->stk, &value2);

    StackPush(&prc->stk, value2 - value1);
}

void GetResult(myProcessor * prc)
{
    for( ; *prc->instruct != HLT; prc->instruct++)
        ExecuteCommand(prc, *prc->instruct);
 }

void ExecuteCommand(myProcessor * prc, const int command)
{
    assert(prc != NULL);
    assert(command > 0);

    switch(command & CHECKCODE)
    {
    case IN: In(prc);
            break;
    case PUSH: Push(prc, command);
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

int GetFileSize(FILE * fp)
{
    struct stat st;

    fstat(fileno(fp), &st);

    return st.st_size;
}

void WriteInstuction(myProcessor * prc)
{
    const char * nameFile = "ByteCode.bin";

    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
        printf("Can't open file\n");

    int countObject = GetFileSize(fp) / sizeof(int);

    prc->instruct = (int *)calloc(GetFileSize(fp), sizeof(char));

    if(prc->instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    if(fread(prc->instruct, sizeof(int), countObject, fp) != countObject)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    //for(int i = 0; i < countObject; i++)
    //    printf("%d\n", prc->instruct[i]);

    fclose(fp);
}

void ProcessorCtor(myProcessor * prc)
{
    assert(prc != NULL);

    prc->registers = (int *)calloc(countRegisters, sizeof(int));

    if(prc->registers == NULL)
        printf("Pointer in registers in NULL\n");

    WriteInstuction(prc);

    StackCtor(&prc->stk);
}

void ProcessorDtor(myProcessor * prc)
{
    assert(prc != NULL);

    free(prc->instruct);
    free(prc->registers);

    prc->instruct = NULL;
    prc->registers = NULL;

    StackDtor(&prc->stk);
}

void ProcessorDump(myProcessor * prc)
{
    assert(prc != NULL);

    assert(&prc->stk != NULL);
    int err = 0;
    STACK_DUMP(&prc->stk, err);

    assert(prc->registers != NULL);

    for(int i = 0; i < countRegisters; i++)
        printf("%d ",  prc->registers[i]);

    printf("\n");
}
