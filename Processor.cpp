#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack.h"
#include "StackDump.h"
#include "Commons.h"
#include "File.h"

struct processor
{
    int * registers;
    int * instruct;
    myStack stk;
};

#define DEF_PUSH(arg) StackPush(&prc->stk, arg)
#define DEF_POP(arg) StackPop(&prc->stk, &arg)
#define DEF_INSTR prc->instruct
#define DEF_REG(arg) prc->registers[arg]
#define DEF_INDEX i

void WriteRegisters(processor * prc);
void WriteInstuction(processor * prc);
void ProcessorCtor(processor * prc);
void ProcessorDtor(processor * prc);
void ProcessorDump(processor * prc);
void GetResult(processor * prc);

int main()
{
    processor prc = {};
    ProcessorCtor(&prc);

    GetResult(&prc);

    ProcessorDtor(&prc);

    return 0;
}

void GetResult(processor * prc)
{
    for(int i = 0; *(prc->instruct + i) != HLT; i++)
    {
        switch(*(prc->instruct + i) & CMD)
        {
            #define DEF_CMD(name, num, args, code)      \
                case num:                               \
                    code;                               \
                    break;                              \

            #include "Command.h"

            #undef DEF_CMD

            #undef DEF_PUSH
            #undef DEF_POP
            #undef DEF_INSTR
            #undef DEF_REG
            #undef DEF_INDEX
        }
    }
}

void WriteInstuction(processor * prc)
{
    assert(prc != NULL);

    const char * nameFile = "Bytecode.bin";
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
        printf("Can't open file\n");

    int fileSize = GetFileSize(fp);
    int sizeInstruct = GetFileSize(fp) / sizeof(int);

    prc->instruct = (int *)calloc(sizeInstruct, sizeof(int));

    if(prc->instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    if(fread(prc->instruct, sizeof(int), sizeInstruct, fp) != sizeInstruct)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    fclose(fp);
}

void WriteRegisters(processor * prc)
{
    prc->registers = (int *)calloc(REGISTERS_COUNT, sizeof(int));

    if(prc->registers == NULL)
        printf("Pointer in registers in NULL\n");
}

void ProcessorCtor(processor * prc)
{
    assert(prc != NULL);

    WriteRegisters(prc);
    WriteInstuction(prc);
    StackCtor(&prc->stk);
}

void ProcessorDtor(processor * prc)
{
    assert(prc != NULL);

    free(prc->instruct);
    free(prc->registers);

    prc->instruct = NULL;
    prc->registers = NULL;

    StackDtor(&prc->stk);
}

void ProcessorDump(processor * prc)
{
    assert(prc != NULL);

    assert(&prc->stk != NULL);
    int err = 0;
    STACK_DUMP(&prc->stk, err);

    assert(prc->registers != NULL);

    for(int i = 0; i < REGISTERS_COUNT; i++)
        printf("%d ",  prc->registers[i]);

    printf("\n");
}
