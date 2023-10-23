#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const int CHECK_CODE = 0b1111;
const int IMM = 1 << 4;
const int REG = 1 << 5;

static_assert( !(IMM & REG),         "IMM and REG overlap");
static_assert( !(IMM & CHECK_CODE ), "IMM and CHECK_CODE  overlap");
static_assert( !(REG & CHECK_CODE ), "REG and CHECK_CODE  overlap");

const char *regs_name[] = { "rax",
                            "rbx",
                            "rcx",
                            "rdx",
                            "rsi",
                            "rdi",
                            "rbp",
                            "rsp",
                            "r08",
                            "r09",
                            "r10",
                            "r11",
                            "r12",
                            "r13",
                            "r14",
                            "r15"};

const int REGISTERS_COUNT = sizeof(regs_name) / sizeof(regs_name[0]);

int GetFileSize(FILE * fp);
int * GetFileContent(int * sizeInstruct);
void FillASM(int * instruct, int sizeInstruct);

int main()
{
    int sizeInstruct = 0;

    int * instruct = GetFileContent(&sizeInstruct);

    FillASM(instruct, sizeInstruct);

    return 0;
}

int GetFileSize(FILE * fp)
{
    struct stat st;

    fstat(fileno(fp), &st);

    return st.st_size;
}

int * GetFileContent(int * sizeInstruct)
{
    const char * nameFile = "ByteCode.bin";
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
        printf("Can't open file\n");

    *sizeInstruct = GetFileSize(fp) / sizeof(int);

    int * instruct = (int *)calloc(GetFileSize(fp), sizeof(char));

    if(instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    if(fread(instruct, sizeof(int), *sizeInstruct, fp) != *sizeInstruct)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    //for(int i = 0; i < countObject; i++)
    //    printf("%d\n", instruct[i]);
    fclose(fp);

    return instruct;
}

void FillASM(int * instruct, int sizeInstruct)
{
    assert(instruct != NULL);
    assert(sizeInstruct > 0);

    const char * nameFile = "ASMcopy.txt";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Can't open file: %s\n", nameFile);

    for(int i = 0; i < sizeInstruct; i++)
    {
        switch(instruct[i] & CHECK_CODE)
        {
            #define DEF_CMD(name, num, args, code)                              \
                case num:                                                       \
                    fprintf(fp, #name);                                         \
                                                                                \
                    if(args)                                                    \
                    {                                                           \
                        if(instruct[i] & IMM)                                   \
                            fprintf(fp, " %d", instruct[++i]);                  \
                                                                                \
                        else if(instruct[i] & REG)                              \
                            fprintf(fp, " %s", regs_name[instruct[++i] - 1]);   \
                    }                                                           \
                                                                                \
                    fprintf(fp, "\n");                                          \
                    break;                                                      \

            #include "Command.h"
            #undef DEF_CMD
        }
    }

    fclose(fp);
}
