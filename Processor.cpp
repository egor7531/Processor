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

const int CHECK_CODE  = 0x1111;
const int IMM = 1 << 4;
const int REG = 1 << 5;

static_assert( !(IMM & REG),         "IMM and REG overlap");
static_assert( !(IMM & CHECK_CODE ), "IMM and CHECK_CODE  overlap");
static_assert( !(REG & CHECK_CODE ), "REG and CHECK_CODE  overlap");

//const uint8_t OSY = 1 << 6;

#define DEF_CMD(name, num, args, code)    \
    name = num,                           \

#define DEF_PUSH(arg) StackPush(&prc->stk, arg)
#define DEF_POP(arg) StackPop(&prc->stk, &arg)
#define DEF_ARG *(++prc->instruct)
#define DEF_COMMAND *prc->instruct
#define DEF_REG(arg) prc->registers[arg]

enum opcode_t
{
    #include "Command.h"
};

#undef DEF_CMD

int GetFileSize(FILE * fp);
void WriteInstuction(myProcessor * prc);
void ProcessorCtor(myProcessor * prc);
void ProcessorDtor(myProcessor * prc);
void ProcessorDump(myProcessor * prc);
void GetResult(myProcessor * prc);
void ExecuteCommand(myProcessor * prc, const int command);

int main()
{
    myProcessor prc = {};

    ProcessorCtor(&prc);

    GetResult(&prc);

    ProcessorDtor(&prc);

    return 0;
}

void GetResult(myProcessor * prc)
{
    for( ; *prc->instruct != HLT; prc->instruct++)
    {
        switch(*prc->instruct & CHECK_CODE)
        {
            #define DEF_CMD(name, num, args, code)      \
                case num:                               \
                    code;                               \
                    break;                              \

            #include "Command.h"

            #undef DEF_CMD

            #undef DEF_PUSH
            #undef DEF_POP
            #undef DEF_COMMAND
            #undef DEF_ARG
            #undef DEF_REG
        }
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

    fclose(fp);
}

void ProcessorCtor(myProcessor * prc)
{
    assert(prc != NULL);

    prc->registers = (int *)calloc(REGISTERS_COUNT, sizeof(int));

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

    //for(int i = 0; i < REGISTERS_COUNT; i++)
    //    printf("%d ",  prc->registers[i]);

    printf("\n");
}
