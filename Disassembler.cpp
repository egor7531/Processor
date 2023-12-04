#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "File.h"

const int WAS_LABEL = -1;
const char *LABEL = ":label";

struct Disassembler
{
    int sizeInstrs;
    int *instrs;
    Label *labels;
    int sizeLabels;
    int errors;
};

enum DASM_ERRORS
{
    LABEL_IS_NULL       = 1 << 1,
    INSTRS_IS_NULL      = 1 << 2,
    FP_IS_NULL          = 1 << 3,
    ERROR_OF_READ_FILE  = 1 << 4,
};

void disassembler_ctor(Disassembler *dasm);
void disassembler_dtor(Disassembler *dasm);
void disassembler_print_errors(const int err);

void get_instuctions(Disassembler *dasm);
void add_label(Disassembler *dasm, const int address);
void get_labels(Disassembler *dasm);
void write_DASM(Disassembler *dasm);

int main()
{
    Disassembler dasm = {};
    disassembler_ctor(&dasm);
    disassembler_dtor(&dasm);

    return 0;
}

void disassembler_ctor(Disassembler *dasm)
{
    assert(dasm != NULL);

    get_instuctions(dasm);
    //for(int i = 0; i < dasm->sizeInstrs; i++)
    //    printf("%d\n", dasm->instrs[i]);

    get_labels(dasm);
    //for(int i = 0; i < dasm->sizeLabels; i++)
    //    printf("%s %d\n", dasm->labels[i].labelName, dasm->labels[i].labelAddress);

    write_DASM(dasm);
}

void disassembler_dtor(Disassembler *dasm)
{
    free(dasm->instrs);
    free(dasm->labels);

    dasm->instrs = NULL;
    dasm->labels = NULL;
}

void disassembler_print_errors(const int err)
{
    assert(err > 0);

    printf("status error: ");
    if(err & LABEL_IS_NULL)
        printf("pointer on label is null\n");
    if(err & INSTRS_IS_NULL)
        printf("pointer on instructions is null\n");
    if(err & FP_IS_NULL)
        printf("pointer on file is null\n");
    if(err & ERROR_OF_READ_FILE)
        printf("error when reading file\n");
}

void add_label(Disassembler *dasm, const int address)
{
    assert(dasm != NULL);
    assert(address >= 0);

    for(int i = 0; i < dasm->sizeLabels; i++)
    {
        if(dasm->labels[i].labelAddress == address)
                return;
    }

    char nameLabel[MAX_LABEL_LENGHT] = ":label";
    snprintf(nameLabel + strlen(LABEL), MAX_LABEL_LENGHT, "%d", dasm->sizeLabels + 1);
    strcpy(dasm->labels[dasm->sizeLabels].labelName, nameLabel);
    dasm->labels[dasm->sizeLabels].labelAddress = address;
    dasm->sizeLabels++;
}

void get_labels(Disassembler *dasm)
{
    assert(dasm != NULL);

    dasm->labels = (Label *)calloc(dasm->sizeInstrs / 2, sizeof(Label));
    if(dasm->labels == NULL)
    {
        dasm->errors |= LABEL_IS_NULL;
        disassembler_print_errors(dasm->errors);
        disassembler_dtor(dasm);
        return;
    }

    for(int i = 0; i < dasm->sizeInstrs; i++)
    {
        switch(dasm->instrs[i] & BIT_FIELD_CMD)
        {
            #define DEF_CMD(name, num, args, code)                                      \
                case num:                                                               \
                    if(args)                                                            \
                    {                                                                   \
                        if(dasm->instrs[i] <= BIT_FIELD_CMD)                            \
                            add_label(dasm, dasm->instrs[i + 1]);                       \
                        i++;                                                            \
                    }                                                                   \
                    break;

            #include "Commands.h"
            #undef DEF_CMD
        }
    }
}

void get_instuctions(Disassembler *dasm)
{
    assert(dasm != NULL);

    FILE *fp = fopen(nameBinaryFile, "rb");
    if(fp == NULL)
    {
        dasm->errors |= FP_IS_NULL;
        disassembler_print_errors(dasm->errors);
        disassembler_dtor(dasm);
        return;
    }

    int fileSize = get_file_size(fp);
    dasm->sizeInstrs = fileSize / sizeof(int);

    dasm->instrs = (int *)calloc(dasm->sizeInstrs, sizeof(int));
    if(dasm->instrs == NULL)
    {
        dasm->errors |= INSTRS_IS_NULL;
        disassembler_print_errors(dasm->errors);
        disassembler_dtor(dasm);
        return;
    }

    if(fread(dasm->instrs, sizeof(int), dasm->sizeInstrs, fp) != dasm->sizeInstrs)
    {
        dasm->errors |= ERROR_OF_READ_FILE;
        disassembler_print_errors(dasm->errors);
        disassembler_dtor(dasm);
        return;
    }

    fclose(fp);
}

void write_DASM(Disassembler *dasm)
{
    assert(dasm != NULL);

    const char * nameFile = "DASM.txt";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
    {
        dasm->errors |= FP_IS_NULL;
        disassembler_print_errors(dasm->errors);
        disassembler_dtor(dasm);
        return;
    }

    for(int i = 0; i < dasm->sizeInstrs; i++)
    {
        for(int j = 0; j < dasm->sizeLabels; j++)
        {
            if(i == dasm->labels[j].labelAddress)
            {
                fprintf(fp, "%s\n", dasm->labels[j].labelName);
                break;
            }
        }

        switch(dasm->instrs[i] & BIT_FIELD_CMD)
        {
            #define DEF_CMD(name, num, args, code)                                      \
                case num:                                                               \
                    fprintf(fp, #name);                                                 \
                                                                                        \
                    if(args)                                                            \
                    {                                                                   \
                        if(dasm->instrs[i] & BIT_FIELD_RAM)                             \
                        {                                                               \
                            if(dasm->instrs[i] & BIT_FIELD_IMM)                         \
                                fprintf(fp, " [%d]", dasm->instrs[++i]);                \
                                                                                        \
                            else if(dasm->instrs[i] & BIT_FIELD_REG)                    \
                                fprintf(fp, " [%s]", NAME_REGS[dasm->instrs[++i] - 1]); \
                        }                                                               \
                                                                                        \
                        else if(dasm->instrs[i] & BIT_FIELD_IMM)                        \
                            fprintf(fp, " %d", dasm->instrs[++i]);                      \
                                                                                        \
                        else if(dasm->instrs[i] & BIT_FIELD_REG)                        \
                            fprintf(fp, " %s", NAME_REGS[dasm->instrs[++i] - 1]);       \
                                                                                        \
                        else                                                            \
                        {                                                               \
                            for(int j = 0; j < dasm->sizeLabels; j++)                   \
                            {                                                           \
                                if(dasm->instrs[i + 1] == dasm->labels[j].labelAddress) \
                                {                                                       \
                                    fprintf(fp, " %s", dasm->labels[j].labelName);      \
                                    break;                                              \
                                }                                                       \
                            }                                                           \
                            i++;                                                        \
                        }                                                               \
                    }                                                                   \
                    fprintf(fp, "\n");                                                  \
                    break;

            #include "Commands.h"
            #undef DEF_CMD
        }
    }

    fclose(fp);
}
