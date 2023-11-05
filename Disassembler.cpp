#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "File.h"

int * read_instuction(int * sizeInstrs);
void fill_DASM(const int * instrs, const int sizeInstrs);

int main()
{
    int sizeInstrs = 0;
    int * instrs = read_instuction(&sizeInstrs);

    fill_DASM(instrs, sizeInstrs);

    free(instrs);

    return 0;
}

int * read_instuction(int * sizeInstrs)
{
    assert(sizeInstrs != NULL);

    const char * nameFile = "Bytecode.bin";
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("Can't open file\n");
        abort();
    }

    int fileSize = get_file_size(fp);
    *sizeInstrs = fileSize / sizeof(int);

    int * instrs = (int *)calloc(*sizeInstrs, sizeof(int));

    if(instrs == NULL)
        printf("Pointer on instructions is NULL\n");

    if(fread(instrs, sizeof(int), *sizeInstrs, fp) != *sizeInstrs)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    fclose(fp);

    return instrs;
}

void fill_DASM(const int * instruct,const int sizeInstruct)
{
    assert(instruct != NULL);
    assert(sizeInstruct > 0);

    const char * nameFile = "DASM.txt";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
    {
        printf("Can't open file: %s\n", nameFile);
        abort();
    }

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

            #include "Commands.h"
            #undef DEF_CMD
        }
    }

    fclose(fp);
}
