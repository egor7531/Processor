#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "File.h"

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
void disassembler_process_errors(Disassembler *dasm, const bool condition, const int err);

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
    assert(dasm != NULL);

    free(dasm->instrs);
    free(dasm->labels);
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

void disassembler_process_errors(Disassembler *dasm, const bool condition, const int err)
{
    assert(dasm != NULL);
    assert(err >= 0);

    if(!condition)
    {
        disassembler_print_errors(err);
        disassembler_dtor(dasm);
        abort();
    }
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
    snprintf(nameLabel + strlen(nameLabel), MAX_LABEL_LENGHT, "%d", dasm->sizeLabels + 1);
    strcpy(dasm->labels[dasm->sizeLabels].labelName, nameLabel);
    dasm->labels[dasm->sizeLabels].labelAddress = address;
    dasm->sizeLabels++;
}

void get_labels(Disassembler *dasm)
{
    assert(dasm != NULL);

    dasm->labels = (Label *)calloc(dasm->sizeInstrs / 2, sizeof(Label));
    disassembler_process_errors(dasm, dasm->labels!= NULL, LABEL_IS_NULL);

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
    disassembler_process_errors(dasm, fp != NULL, FP_IS_NULL);

    int fileSize = get_file_size(fp);
    dasm->sizeInstrs = fileSize / sizeof(int);

    dasm->instrs = (int *)calloc(dasm->sizeInstrs, sizeof(int));
    disassembler_process_errors(dasm, dasm->instrs != NULL, INSTRS_IS_NULL);
    disassembler_process_errors(dasm, fread(dasm->instrs, sizeof(int), dasm->sizeInstrs, fp)
                                        == dasm->sizeInstrs, ERROR_OF_READ_FILE);

    fclose(fp);
}

void write_DASM(Disassembler *dasm)
{
    assert(dasm != NULL);

    const char * nameFile = "DASM.txt";
    FILE * fp = fopen(nameFile, "wb");
    disassembler_process_errors(dasm, fp != NULL, FP_IS_NULL);

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
