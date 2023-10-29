#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Common.h"
#include "File.h"

const int MAXLABELLENGHT = 10;
const int MAXSIZEARG = 25;

struct Label
{
    int labelAddress;
    char labelName[MAXLABELLENGHT];
};

int skip_space(const char * buf, int ind);
bool is_label(const char * str, const Label * lbs, const int labelCount);
bool is_number(const char * str);
bool is_ram(char * str, const char * buf, int * ind);
bool is_register(const char * str);
void get_arg(char * str, const char * buf, int *ind);
int get_address_abel(const Label * lbs, const int labelCount, const char * strArg);
int get_num_reg(const char * strReg);
void write_bytecode(const int * insts, const int sizeInstruct);
int * get_instrs(const char * buf, const int nLine, int * sizeInstruct,
                                                const Label * lbs, const int labelCount);
Label * get_labels(const char * buf, int * labelCount, const int nLine);
void add_label(Label * lbs, const char * command, const int countInstrs, int * labelCount);

int main()
{
    int fileSize = 0;
    char * buf = get_file_content(&fileSize);

    int nLine =  get_count_line(buf, fileSize);

    int labelCount = 0;
    Label * lbs = get_labels(buf, &labelCount, nLine);

    //for(int i = 0; i < labelCount; i++)
    //    printf("%s %d\n", lbs[i].labelName, lbs[i].labelAddress);

    int sizeInstrs = 0;
    int * instrs = get_instrs(buf, nLine, &sizeInstrs, lbs, labelCount);

    //for(int i = 0; i < sizeInstrs; i++)
    //    printf("%d\n", instrs[i]);

    write_bytecode(instrs, sizeInstrs);

    free(buf);
    free(instrs);
    free(lbs);

    return 0;
}

int skip_space(const char * buf, int ind)
{
    assert(buf != NULL);
    assert(ind >= 0);

    while(buf[ind] == ' ')
        ind++;

    return ind;
}

int get_num_reg(const char * strReg)
{
    assert(strReg != NULL);

    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(strReg, REGS_NAME[i]))
            return i + 1;
    }

    return -1;
}

void get_arg(char * str, const char * buf, int *ind)
{
    assert(str != NULL);
    assert(buf != NULL);
    assert(ind != NULL);

    int indCopy = *ind;

    if(buf[indCopy] == '[')
    {
        int i = 0;
        for(; buf[indCopy] != ']'; indCopy++, i++)
            str[i] = buf[indCopy];
        str[i++] = buf[indCopy++];
        str[i] = '\0';
    }

    else
    {
        sscanf(buf + indCopy, "%s", str);
        indCopy += strlen(str);
    }

    *ind = indCopy;
}

int get_address_label(const Label * lbs, const int labelCount, const char * strArg)
{
    assert(lbs    != NULL);
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

    for(int i = 0; str[i] != '\0' && str[i] != ']' && str[i] != ' '; i++)
    {
        if(!isdigit(str[i]))
        {
            isnum = false;
            break;
        }
    }

    return isnum;
}

bool is_ram(char * str)
{
    assert(str != NULL);

    char strArgRam[MAXSIZEARG] = {};

    int ind = skip_space(str, 1);

    sscanf(str + ind, "%s", strArgRam);
    if(strArgRam[strlen(strArgRam) - 1] == ']')
        strArgRam[strlen(strArgRam) - 1] = '\0';

    ind += strlen(strArgRam);
    ind = skip_space(str, ind);

    if(str[0] == '[' && str[ind] == ']')
    {
        strcpy(str, strArgRam);
        return true;
    }

    else
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

Label * get_labels(const char * buf, int * labelCount, const int nLine)
{
    assert(buf        != NULL);
    assert(labelCount != NULL);
    assert(nLine > 0);

    const int MAX_LABEL_COUNT = 10;
    Label * lbs = (Label *)calloc(MAX_LABEL_COUNT, sizeof(Label));

    if(lbs == NULL)
    {
        printf("Pointer on lbs in NULL\n");
        abort();
    }

    int countInstrs = 0;
    int ind = 0;

    for(int i = 0; i < nLine; i++)
    {
        ind = skip_space(buf, ind);

        const int maxSizeCommand = 10;
        char command[maxSizeCommand] = {};
        sscanf(buf + ind, "%s", command);

        ind += strlen(command);
        ind = skip_space(buf, ind);

        if(command[0] == ':')
            add_label(lbs, command, countInstrs, labelCount);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                countInstrs++;                                                              \
                if(args)                                                                    \
                {                                                                           \
                    countInstrs++;                                                          \
                                                                                            \
                    char strArg[MAXSIZEARG] = {};                                           \
                    get_arg(strArg, buf, &ind);                                             \
                    ind = skip_space(buf, ind);                                             \
                }                                                                           \
            }                                                                               \

        #include "Commands.h"
        #undef DEF_CMD

        ind += 2;
    }

    return lbs;
}

int * get_instrs(const char * buf, const int nLine, int * sizeInstrs,
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

    int ind = 0;

    for(int i = 0; i < nLine; i++)
    {
        const int maxSizeCommand = 10;
        char command[maxSizeCommand] = {};
        ind = skip_space(buf, ind);
        sscanf(buf + ind, "%s", command);

        ind += strlen(command);
        ind = skip_space(buf, ind);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                int numberCommand = num;                                                    \
                                                                                            \
                if(args)                                                                    \
                {                                                                           \
                    char strArg[MAXSIZEARG] = {};                                           \
                    get_arg(strArg, buf, &ind);                                             \
                    ind = skip_space(buf, ind);                                             \
                                                                                            \
                    if(is_ram(strArg))                                                      \
                        numberCommand |= RAM;                                               \
                                                                                            \
                    if(is_number(strArg))                                                   \
                    {                                                                       \
                        numberCommand |= IMM;                                               \
                        instrs[(*sizeInstrs) + 1] = atoi(strArg);                           \
                    }                                                                       \
                                                                                            \
                    else if(is_register(strArg))                                            \
                    {                                                                       \
                        numberCommand |= REG;                                               \
                        instrs[(*sizeInstrs) + 1] = get_num_reg(strArg);                    \
                    }                                                                       \
                                                                                            \
                    else if(is_label(strArg, lbs, labelCount))                              \
                        instrs[(*sizeInstrs) + 1] = get_address_label(lbs,                  \
                                                                    labelCount, strArg);    \
                                                                                            \
                    else                                                                    \
                    {                                                                       \
                        printf("Unknown arg '%s' in line: %d", strArg, i + 1);              \
                        abort();                                                            \
                    }                                                                       \
                                                                                            \
                    instrs[(*sizeInstrs)++] = numberCommand;                                \
                }                                                                           \
                                                                                            \
                else                                                                        \
                    instrs[*sizeInstrs] = numberCommand;                                    \
                                                                                            \
                (*sizeInstrs)++;                                                            \
            }                                                                               \

        #include "Commands.h"
        #undef DEF_CMD

        ind += 2; //Because file in rb format ends with 2 characters '\r' and '\n'
    }

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
