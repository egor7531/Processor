#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Stack.h"
#include "StackDump.h"
#include "File.h"
#include "Common.h"

const int RAM_SIZE = 11 * 11;

enum PRC_ERRORS
{
    INSTRS_IS_NULL      = 1 << 1,
    FP_IS_NULL          = 1 << 2,
    ERROR_OF_READ_FILE  = 1 << 3,
};

struct Processor
{
    int ram[RAM_SIZE];
    int registers[COUNT_REGISTERS];
    int * instrs;
    Stack stk;
    int errors;
};

void processor_print_errors(const int err);
void prcocessor_process_errors(Processor *prc, const bool condition, const int err);
void get_instuctions(Processor * prc);
void processor_ctor(Processor * prc);
void processor_dtor(Processor * prc);
//void processor_dump(Processor * prc);
void run_processor(Processor * prc);
void print_ram(Processor * prc);

int main()
{
    Processor prc = {};
    processor_ctor(&prc);

    run_processor(&prc);

    print_ram(&prc);

    processor_dtor(&prc);
    return 0;
}

void run_processor(Processor * prc)
{
    assert(prc != NULL);

    for(int i = 0; prc->instrs[i] != hlt; i++)
    {
        //printf("\ncommand = %d\n", prc->instrs[i]);
        switch(prc->instrs[i] & BIT_FIELD_CMD)
        {
            #define PUSH(arg) stack_push(&prc->stk, arg)
            #define POP(arg) stack_pop(&prc->stk, &arg)
            #define INDEX i
            #define INSTR prc->instrs
            #define RAMS prc->ram
            #define REGS prc->registers
            #define SQRT(arg) sqrt(arg)
            #define SIN(arg) sin(arg)
            #define COS(arg) cos(arg)

            #define DEF_CMD(name, num, args, code)      \
                case num:                               \
                    code;                               \
                    break;

            #include "Commands.h"

            #undef DEF_CMD

            #undef PUSH
            #undef POP
            #undef INSTR
            #undef REG
            #undef INDEX
            #undef RAM
            #undef SQRT
            #undef SIN
            #undef COS
        }
        //processor_dump(prc);
    }
}

void processor_print_errors(const int err)
{
    assert(err > 0);

    printf("status error: ");
    if(err & INSTRS_IS_NULL)
        printf("pointer on instructions is null\n");
    if(err & FP_IS_NULL)
        printf("pointer on file is null\n");
    if(err & ERROR_OF_READ_FILE)
        printf("error when reading file\n");
}

void prcocessor_process_errors(Processor *prc, const bool condition, const int err)
{
    assert(prc != NULL);
    assert(err >= 0);

    if(!condition)
    {
        processor_print_errors(err);
        processor_dtor(prc);
        abort();
    }
}

void get_instuctions(Processor * prc)
{
    assert(prc != NULL);

    FILE *fp = fopen(nameBinaryFile, "rb");
    prcocessor_process_errors(prc, fp != NULL, FP_IS_NULL);

    int fileSize = get_file_size(fp);
    int sizeInstrs = fileSize / sizeof(int);

    prc->instrs = (int *)calloc(sizeInstrs, sizeof(int));
    prcocessor_process_errors(prc, prc->instrs != NULL, INSTRS_IS_NULL);
    prcocessor_process_errors(prc,
    fread(prc->instrs, sizeof(int), sizeInstrs, fp) == sizeInstrs, ERROR_OF_READ_FILE);

    //for(int i = 0; i < sizeInstrs; i++)
    //    printf("%d\n", prc->instrs[i]);

    fclose(fp);
}

void processor_ctor(Processor * prc)
{
    assert(prc != NULL);

    get_instuctions(prc);
    stack_ctor(&prc->stk);
}

void processor_dtor(Processor * prc)
{
    assert(prc != NULL);

    free(prc->instrs);
    stack_dtor(&prc->stk);
}

/*void processor_dump(Processor * prc)
{
    assert(prc != NULL);
    assert(&prc->stk != NULL);
    assert(prc->registers != NULL);
    assert(prc->ram != NULL);

    STACK_DUMP(&prc->stk);

    printf("Registers:\n");
    for(int i = 0; i < COUNT_REGISTERS; i++)
        printf("%d ",  prc->registers[i]);

    printf("\nRAM:\n");
    for(int i = 0; i < RAM_SIZE; i++)
        printf("%d ",  prc->ram[i]);

    printf("\n");
}*/

void print_ram(Processor * prc)
{
    assert(prc != NULL);

    const char * nameFile = "Result.txt";
    FILE * fp = fopen("Result.txt", "wb");
    prcocessor_process_errors(prc, fp != NULL, FP_IS_NULL);

    int N = sqrt(RAM_SIZE);

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(prc->ram[i * N + j])
                fprintf(fp, " .");
            else
                fprintf(fp, "  ");
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}
