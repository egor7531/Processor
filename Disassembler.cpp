#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Commons.h"
#include "File.h"

int * WriteInstuction(FILE * fp, int * sizeInstruct);
void FillDASM(const int * instruct, const int sizeInstruct);

int main()
{
    const char * nameFile = "Bytecode.bin";
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
        printf("Can't open file\n");

    int sizeInstruct = 0;

    int * instruct = WriteInstuction(fp, &sizeInstruct);

    FillDASM(instruct, sizeInstruct);

    free(instruct);

    return 0;
}

int * WriteInstuction(FILE * fp, int * sizeInstruct)
{
    assert(fp != NULL);

    int fileSize = GetFileSize(fp);
    *sizeInstruct = GetFileSize(fp) / sizeof(int);

    int * instruct = (int *)calloc(*sizeInstruct, sizeof(int));

    if(instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    if(fread(instruct, sizeof(int), *sizeInstruct, fp) != *sizeInstruct)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    fclose(fp);

    return instruct;
}

void FillDASM(const int * instruct,const int sizeInstruct)
{
    assert(instruct != NULL);
    assert(sizeInstruct > 0);

    const char * nameFile = "DASM.txt";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Can't open file: %s\n", nameFile);

    for(int i = 0; i < sizeInstruct; i++)
    {
        switch(instruct[i] & CMD)
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
                            fprintf(fp, " %s", REGS_NAME[instruct[++i] - 1]);   \
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
