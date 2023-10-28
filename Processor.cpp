#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack.h"
#include "StackDump.h"
#include "File.h"
#include "Common.h"

const int RAM_SIZE = 100;

struct Processor
{
    int ram[RAM_SIZE];
    int registers[REGISTERS_COUNT];
    int * instrs;
    myStack stk;
};

void read_instuctions(Processor * prc);
void processor_ctor(Processor * prc);
void processor_dtor(Processor * prc);
void processor_dump(Processor * prc);
void run_processor(Processor * prc);

int main()
{
    Processor prc = {};
    processor_ctor(&prc);

    run_processor(&prc);

    processor_dtor(&prc);

    return 0;
}

void run_processor(Processor * prc)
{
    assert(prc != NULL);

    for(int i = 0; prc->instrs[i] != hlt; i++)
    {
        switch(prc->instrs[i] & CMD)
        {
            #define PUSH(arg) StackPush(&prc->stk, arg)
            #define POP(arg) StackPop(&prc->stk, &arg)
            #define INDEX i
            #define INSTR prc->instrs
            #define RAMS prc->ram
            #define REGS prc->registers

            #define DEF_CMD(name, num, args, code)      \
                case num:                               \
                    code;                               \
                    break;                              \

            #include "Commands.h"

            #undef DEF_CMD

            #undef DEF_PUSH
            #undef DEF_POP
            #undef DEF_INSTR
            #undef DEF_REG
            #undef DEF_INDEX
            #undef DEF_RAM
        }
        //processor_dump(prc);
    }
}

void read_instuctions(Processor * prc)
{
    assert(prc != NULL);

    const char * nameFile = "Bytecode.bin";
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("Can't open file\n");
        abort();
    }

    int fileSize = get_file_size(fp);
    int sizeInstrs = fileSize / sizeof(int);

    prc->instrs = (int *)calloc(sizeInstrs, sizeof(int));

    if(prc->instrs == NULL)
    {
        printf("Pointer on instructions is NULL\n");
        abort();
    }

    if(fread(prc->instrs, sizeof(int), sizeInstrs, fp) != sizeInstrs)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    //for(int i = 0; i < sizeInstrs; i++)
    //    printf("%d\n", prc->instrs[i]);

    fclose(fp);
}

void processor_ctor(Processor * prc)
{
    assert(prc != NULL);

    read_instuctions(prc);
    StackCtor(&prc->stk);
}

void processor_dtor(Processor * prc)
{
    assert(prc != NULL);

    free(prc->instrs);
    prc->instrs = NULL;

    StackDtor(&prc->stk);
}

void processor_dump(Processor * prc)
{
    assert(prc != NULL);
    assert(&prc->stk != NULL);
    assert(prc->registers != NULL);
    assert(prc->ram != NULL);

    int err = 0;
    STACK_DUMP(&prc->stk, err);

    printf("Registers:\n");
    for(int i = 0; i < REGISTERS_COUNT; i++)
        printf("%d ",  prc->registers[i]);

    printf("\nRAM:\n");
    for(int i = 0; i < RAM_SIZE; i++)
        printf("%d ",  prc->ram[i]);

    printf("\n");
}
