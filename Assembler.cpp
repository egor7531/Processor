#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Common.h"
#include "File.h"

struct Assembler
{
    int sizeBuf;
    char *buf;
    int sizeLabel;
    Label *labels;
    int sizeInstrs;
    int *instrs;
    int errors;
};

enum ASM_ERRORS
{
    BUF_IS_NULL     = 1 << 1,
    LABEL_IS_NULL   = 1 << 2,
    INSTRS_IS_NULL  = 1 << 3,
    BAD_SYNTAX      = 1 << 4,
    FP_IS_NULL      = 1 << 5
};

const int MAX_SIZE_COMMAND = 15;
const int MAX_SIZE_ARG = 50;

void assembler_ctor(Assembler *asmr);
void assembler_dtor(Assembler *asmr);
//void assembler_dump(const Assembler *asmr);
void assembler_print_errors(const int err, const int nLine);

void skip_space(char **buf);

void write_bytecode(Assembler *asmr);
void add_label(Label *lbs, const char *command, const int numInstr, int *labelCount);

bool is_label(const char *str, const Label *lbs, const int labelCount);
bool is_number(const char *str);
bool is_ram(char *str);
bool is_register(const char *str);

void get_arg(char *str, char **buf);
int get_label_address(const Label *lbs, const int labelCount, const char *strArg);
int get_reg_num(const char *strReg);

void get_instrs(Assembler *asmr, const int nLine);
void get_labels(Assembler *asmr, const int nLine);

int main()
{
    Assembler asmr = {};
    assembler_ctor(&asmr);
    assembler_dtor(&asmr);

    return 0;
}

void assembler_ctor(Assembler *asmr)
{
    assert(asmr != NULL);

    const char *nameFile  = "Square.txt";
    asmr->buf = get_file_content(nameFile, &asmr->sizeBuf);
    if(asmr->buf == NULL)
    {
        asmr->errors |= BUF_IS_NULL;
        assembler_print_errors(asmr->errors, 0);
        assembler_dtor(asmr);
        return;
    }

    int nLine =  get_count_line(asmr->buf, asmr->sizeBuf);
    get_labels(asmr, nLine);
    get_instrs(asmr, nLine);
    write_bytecode(asmr);
}

void assembler_dtor(Assembler *asmr)
{
    free(asmr->buf);
    free(asmr->instrs);
    free(asmr->labels);

    asmr->buf = NULL;
    asmr->instrs = NULL;
    asmr->labels = NULL;
}

/*void assembler_dump(const Assembler *asmr)
{
    assert(asmr != NULL);
    assembler_print_errors(asmr->errors);

    if(asmr->buf == NULL)
        return;
    for(int i = 0; i < asmr->sizeBuf; i++)
        printf("%c", asmr->buf[i]);
    printf("\n");

    if(asmr->labels == NULL)
        return;
    for(int i = 0; i < asmr->sizeLabel; i++)
        printf("%s %d", asmr->labels[i].labelName, asmr->labels[i].labelAddress);
    printf("\n");

    if(asmr->buf == NULL)
        return;
    for(int i = 0; i < asmr->sizeInstrs; i++)
        printf("%d ", asmr->instrs[i]);
    printf("\n");
}*/

void assembler_print_errors(const int err, const int nLine)
{
    assert(err > 0);
    assert(nLine > 0);

    printf("status error: ");
    if(err & BUF_IS_NULL)
        printf("pointer on buf is null\n");
    if(err & LABEL_IS_NULL)
        printf("pointer on label is null\n");
    if(err & INSTRS_IS_NULL)
        printf("pointer on instructions is null\n");
    if(err & BAD_SYNTAX)
        printf("bad syntax in line %d\n", nLine);
    if(err & FP_IS_NULL)
        printf("pointer on file is null\n");
}

void skip_space(char **buf)
{
    assert(buf != NULL);

    while(**buf == ' ' || **buf == '\r' || **buf == '\n')
        (*buf)++;
}

int get_reg_num(const char *strReg)
{
    assert(strReg != NULL);

    for(int i = 0; i < COUNT_REGISTERS; i++)
    {
        if(!strcmp(strReg, NAME_REGS[i]))
            return i + 1;
    }

    return -1;
}

void get_arg(char *str, char **buf)
{
    assert(str != NULL);
    assert(buf != NULL);

    if(**buf == '[')
    {
        int i = 0;
        for(; **buf != ']'; i++, (*buf)++)
            str[i] = **buf;
        str[i++] = **buf;
        str[i] = '\0';
        (*buf)++;
    }
    else
    {
        sscanf(*buf, "%s", str);
        *buf += strlen(str);
    }
}

int get_label_address(const Label *lbs, const int labelCount, const char *strArg)
{
    assert(lbs != NULL);
    assert(strArg != NULL);
    assert(labelCount > 0);

    for(int i = 0; i < labelCount; i++)
    {
        if(!strcmp(lbs[i].labelName, strArg))
            return lbs[i].labelAddress;
    }

    return -1;
}

bool is_register(const char * str)
{
    assert(str != NULL);

    for(int i = 0; i < COUNT_REGISTERS; i++)
    {
        if(!strcmp(str, NAME_REGS[i]))
            return true;
    }

    return false;
}

bool is_label(const char * str, const Label * lbs, const int labelCount)
{
    assert(str != NULL);
    assert(lbs != NULL);
    assert(labelCount >= 0);

    for(int i = 0; i < labelCount; i++)
    {
        if(!strcmp(str, lbs[i].labelName))
            return true;
    }

    return false;
}

bool is_number(const char * str)
{
    assert(str != NULL);

    bool isnum = true;
    for(int i = 1; str[i] != '\0'; i++)
    {
        if(!isdigit(str[i]))
        {
            isnum = false;
            break;
        }
    }

    return isnum && (str[0] == '-' || isdigit(str[0]));
}

bool is_ram(char * str)
{
    assert(str != NULL);

    char * strCopy = str;
    str++;
    skip_space(&str);
    char strArgRam[MAX_SIZE_ARG] = {};
    sscanf(str, "%s", strArgRam);

    if(strArgRam[strlen(strArgRam) - 1] == ']')
        strArgRam[strlen(strArgRam) - 1] = '\0';

    str += strlen(strArgRam);
    skip_space(&str);

    if(*strCopy == '[' && *str == ']')
    {
        str = strCopy;
        strcpy(str, strArgRam);
        return true;
    }

    return false;
}

void add_label(Label *lbs, const char *command, const int numInstr, int *labelCount)
{
    assert(lbs != NULL);
    assert(command != NULL);
    assert(numInstr >= 0);
    assert(*labelCount >= 0);

    strcpy(lbs[*labelCount].labelName, command);
    lbs[*labelCount].labelAddress = numInstr;
    (*labelCount)++;
}

void get_labels(Assembler *asmr, const int nLine)
{
    assert(asmr != NULL);
    assert(asmr->buf != NULL);
    assert(nLine > 0);

    asmr->labels = (Label *)calloc(nLine, sizeof(Label));
    if(asmr->labels == NULL)
    {
        asmr->errors |= LABEL_IS_NULL;
        assembler_print_errors(asmr->errors, 0);
        assembler_dtor(asmr);
        return;
    }

    int numInstr = 0;
    char *bufCopy = asmr->buf;
    char command[MAX_SIZE_COMMAND] = {};

    for(int i = 0; i < nLine; i++)
    {
        skip_space(&asmr->buf);
        if(sscanf(asmr->buf, "%s", command) == EOF)
            break;
        asmr->buf += strlen(command);
        if(command[0] == ':')
            add_label(asmr->labels, command, numInstr, &asmr->sizeLabel);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                numInstr++;                                                                 \
                if(args)                                                                    \
                {                                                                           \
                    skip_space(&asmr->buf);                                                 \
                    numInstr++;                                                             \
                    char strArg[MAX_SIZE_ARG] = {};                                         \
                    get_arg(strArg, &asmr->buf);                                            \
                }                                                                           \
                continue;                                                                   \
            }

        #include "Commands.h"
        #undef DEF_CMD
    }

    asmr->buf = bufCopy;
}

void get_instrs(Assembler *asmr, const int nLine)
{
    assert(asmr != NULL);
    assert(nLine > 0);

    asmr->instrs = (int *)calloc(2 * nLine, sizeof(int));
    if(asmr->instrs == NULL)
    {
        asmr->errors |= INSTRS_IS_NULL;
        assembler_print_errors(asmr->errors, 0);
        assembler_dtor(asmr);
        return;
    }

    char command[MAX_SIZE_COMMAND] = {};
    int countInstrs = 0;

    for(int i = 0; i < nLine; i++)
    {
        skip_space(&asmr->buf);
        if(sscanf(asmr->buf, "%s", command) == EOF)
            break;
        asmr->buf += strlen(command);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                int numberCommand = num;                                                    \
                                                                                            \
                if(args)                                                                    \
                {                                                                           \
                    skip_space(&asmr->buf);                                                 \
                    char strArg[MAX_SIZE_ARG] = {};                                         \
                    get_arg(strArg, &asmr->buf);                                            \
                                                                                            \
                    if(is_ram(strArg))                                                      \
                        numberCommand |= BIT_FIELD_RAM;                                     \
                                                                                            \
                    if(is_number(strArg))                                                   \
                    {                                                                       \
                        numberCommand |= BIT_FIELD_IMM;                                     \
                        asmr->instrs[countInstrs + 1] = atoi(strArg);                       \
                    }                                                                       \
                                                                                            \
                    else if(is_register(strArg))                                            \
                    {                                                                       \
                        numberCommand |= BIT_FIELD_REG;                                     \
                        asmr->instrs[countInstrs + 1] = get_reg_num(strArg);                \
                    }                                                                       \
                                                                                            \
                    else if(is_label(strArg, asmr->labels, asmr->sizeLabel))                \
                        asmr->instrs[countInstrs + 1] =                                     \
                    get_label_address(asmr->labels, asmr->sizeLabel, strArg);               \
                                                                                            \
                    else                                                                    \
                    {                                                                       \
                        asmr->errors |= BAD_SYNTAX;                                         \
                        assembler_print_errors(asmr->errors, i + 1);                        \
                        assembler_dtor(asmr);                                               \
                        return;                                                             \
                    }                                                                       \
                                                                                            \
                }                                                                           \
                asmr->instrs[countInstrs] = numberCommand;                                  \
                countInstrs += args + 1;                                                    \
                continue;                                                                   \
            }

        #include "Commands.h"
        #undef DEF_CMD

        if(!is_label(command, asmr->labels, asmr->sizeLabel))
        {
            asmr->errors |= BAD_SYNTAX;
            assembler_print_errors(asmr->errors, i + 1);
            assembler_dtor(asmr);
            return;
        }
    }

    asmr->sizeInstrs = countInstrs;
}

void write_bytecode(Assembler *asmr)
{
    assert(asmr != NULL);

    FILE * fp = fopen(nameBinaryFile, "wb");
    if(fp == NULL)
    {
        fclose(fp);
        asmr->errors |= FP_IS_NULL;
        assembler_print_errors(asmr->errors, 0);
        assembler_dtor(asmr);
        return;
    }

    fwrite(asmr->instrs, sizeof(int), asmr->sizeInstrs, fp);
    fclose(fp);
}
