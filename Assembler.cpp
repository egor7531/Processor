#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Common.h"
#include "File.h"

const int MAX_SIZE_COMMAND = 15;
const int MAX_LABEL_LENGHT = 50;
const int MAX_SIZE_ARG = 50;

struct Label
{
    int labelAddress;
    char labelName[MAX_LABEL_LENGHT];
};

void skip_space(char ** buf);

void write_bytecode(const int * insts, const int sizeInstruct);
void add_label(Label * lbs, const char * command, const int countInstrs, int * labelCount);

bool is_label(const char * str, const Label * lbs, const int labelCount);
bool is_number(const char * str);
bool is_ram(char * str, const char * buf, int * ind);
bool is_register(const char * str);

void get_arg(char * str, char ** buf);
int get_label_address(const Label * lbs, const int labelCount, const char * strArg);
int get_reg_num(const char * strReg);
int * get_instrs(char * buf, const int nLine, int * sizeInstruct,
                                                const Label * lbs, const int labelCount);
Label * get_labels(char * buf, int * labelCount, const int nLine);

int main()
{
    const char * nameFile  = "Square.txt";
    int fileSize = 0;
    char * buf = get_file_content(nameFile, &fileSize);

    int nLine =  get_count_line(buf, fileSize);

    int labelCount = 0;
    Label * lbs = get_labels(buf, &labelCount, nLine);

    //for(int i = 0; i < labelCount; i++)
    //    printf("%s %d\n", lbs[i].labelName, lbs[i].labelAddress);

    int sizeInstrs = 0;
    int * instrs = get_instrs(buf, nLine, &sizeInstrs, lbs, labelCount);

    //for(int i = 0; i < sizeInstrs; i++)
    //    printf("%d %d\n", i, instrs[i]);

    write_bytecode(instrs, sizeInstrs);

    free(buf);
    free(instrs);
    free(lbs);

    return 0;
}

void skip_space(char ** buf)
{
    assert(buf != NULL);

    while(**buf == ' ' || **buf == '\r' || **buf == '\n')
        (*buf)++;
}
int get_reg_num(const char * strReg)
{
    assert(strReg != NULL);

    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(strReg, REGS_NAME[i]))
            return i + 1;
    }

    return -1;
}

void get_arg(char * str, char ** buf)
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

int get_label_address(const Label * lbs, const int labelCount, const char * strArg)
{
    assert(lbs        != NULL);
    assert(strArg     != NULL);
    assert(labelCount  >    0);

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

    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(str, REGS_NAME[i]))
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

void add_label(Label * lbs, const char * command, const int countInstrs, int * labelCount)
{
    assert(lbs     != NULL);
    assert(command != NULL);
    assert(countInstrs >= 0);
    assert(*labelCount >= 0);

    strcpy(lbs[*labelCount].labelName, command);
    lbs[*labelCount].labelAddress = countInstrs;
    (*labelCount)++;
}

Label * get_labels(char * buf, int * labelCount, const int nLine)
{
    assert(buf        != NULL);
    assert(labelCount != NULL);
    assert(nLine > 0);

    Label * lbs = (Label *)calloc(nLine, sizeof(Label));

    if(lbs == NULL)
    {
        printf("Pointer on lbs in NULL\n");
        abort();
    }

    int countInstrs = 0;
    char * bufCopy = buf;
    char command[MAX_SIZE_COMMAND] = {};

    for(int i = 0; i < nLine; i++)
    {
        skip_space(&buf);
        sscanf(buf, "%s", command);
        buf += strlen(command);
        if(command[0] == ':')
            add_label(lbs, command, countInstrs, labelCount);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                countInstrs++;                                                              \
                if(args)                                                                    \
                {                                                                           \
                    skip_space(&buf);                                                       \
                    countInstrs++;                                                          \
                    char strArg[MAX_SIZE_ARG] = {};                                         \
                    get_arg(strArg, &buf);                                                  \
                }                                                                           \
            }                                                                               \

        #include "Commands.h"
        #undef DEF_CMD
    }

    buf = bufCopy;
    return lbs;
}

int * get_instrs(char * buf, const int nLine, int * sizeInstrs,
                                        const Label * lbs, const int labelCount)
{
    assert(buf          != NULL);
    assert(lbs          != NULL);
    assert(sizeInstrs   != NULL);
    assert(nLine      >  0);
    assert(labelCount >= 0);

    int * instrs = (int *)calloc(2 * nLine, sizeof(int));

    if(instrs == NULL)
    {
        printf("Pointer on instructions is NULL\n");
        abort();
    }

    char command[MAX_SIZE_COMMAND] = {};
    int countInstrs = 0;

    for(int i = 0; i < nLine; i++)
    {
        skip_space(&buf);
        sscanf(buf, "%s", command);
        buf += strlen(command);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                int numberCommand = num;                                                    \
                                                                                            \
                if(args)                                                                    \
                {                                                                           \
                    skip_space(&buf);                                                        \
                    char strArg[MAX_SIZE_ARG] = {};                                         \
                    get_arg(strArg, &buf);                                                   \
                                                                                            \
                    if(is_ram(strArg))                                                      \
                        numberCommand |= RAM;                                               \
                                                                                            \
                    if(is_number(strArg))                                                   \
                    {                                                                       \
                        numberCommand |= IMM;                                               \
                        instrs[countInstrs + 1] = atoi(strArg);                             \
                    }                                                                       \
                                                                                            \
                    else if(is_register(strArg))                                            \
                    {                                                                       \
                        numberCommand |= REG;                                               \
                        instrs[countInstrs + 1] = get_reg_num(strArg);                      \
                    }                                                                       \
                                                                                            \
                    else if(is_label(strArg, lbs, labelCount))                              \
                        instrs[countInstrs + 1] = get_label_address(lbs,                    \
                                                                    labelCount, strArg);    \
                                                                                            \
                    else                                                                    \
                    {                                                                       \
                        printf("Unknown arg '%s' in line: %d", strArg, i + 1);              \
                        abort();                                                            \
                    }                                                                       \
                                                                                            \
                }                                                                           \
                instrs[countInstrs] = numberCommand;                                        \
                countInstrs += args + 1;                                                    \
            }                                                                               \

        #include "Commands.h"
        #undef DEF_CMD
    }

    *sizeInstrs = countInstrs;
    return instrs;
}

void write_bytecode(const int * instrs, const int sizeInstrs)
{
    assert(instrs != NULL);
    assert(sizeInstrs > 0);

    const char * nameFile = "Bytecode.bin";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
    {
        printf("Error work of File %s: \n", nameFile);
        abort();
    }

    fwrite(instrs, sizeof(int), sizeInstrs, fp);

    fclose(fp);
}
