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

    FILE *fp = fopen(nameBinaryFile, "rb");

    if(fp == NULL)
    {
        printf("Can't open file\n");
        fclose(fp);
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

void fill_DASM(const int * instrs,const int sizeInstrs)
{
    assert(instrs != NULL);
    assert(sizeInstrs > 0);

    const char * nameFile = "DASM.txt";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
    {
        printf("Can't open file: %s\n", nameFile);
        fclose(fp);
        abort();
    }

    for(int i = 0; i < sizeInstrs; i++)
    {
        switch(instrs[i] & BIT_FIELD_CMD)
        {
            #define DEF_CMD(name, num, args, code)                                  \
                case num:                                                           \
                    fprintf(fp, #name);                                             \
                                                                                    \
                    if(args)                                                        \
                    {                                                               \
                        if(instrs[i] & BIT_FIELD_RAM)                               \
                        {                                                           \
                            if(instrs[i] & BIT_FIELD_IMM)                           \
                                fprintf(fp, " [%d]", instrs[++i]);                  \
                                                                                    \
                            else if(instrs[i] & BIT_FIELD_REG)                      \
                                fprintf(fp, " [%s]", NAME_REGS[instrs[++i] - 1]);   \
                        }                                                           \
                                                                                    \
                        else if(instrs[i] & BIT_FIELD_IMM)                          \
                            fprintf(fp, " %d", instrs[++i]);                        \
                                                                                    \
                        else if(instrs[i] & BIT_FIELD_REG)                          \
                            fprintf(fp, " %s", NAME_REGS[instrs[++i] - 1]);         \
                                                                                    \
                        else{ i++;                                                  \
                            fprintf(fp, " :func");}                                 \
                    }                                                               \
                    fprintf(fp, "\n");                                              \
                   break;

            #include "Commands.h"
            #undef DEF_CMD
        }
    }

    fclose(fp);
}
