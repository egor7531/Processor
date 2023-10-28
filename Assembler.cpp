#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Common.h"
#include "File.h"

const int MAXLABELLENGHT = 10;
const int MAXSIZEARG = 10;

struct Label
{
    int labelAddress;
    char labelName[MAXLABELLENGHT];
};

bool is_label(const char * str);
bool is_number(const char * str);
bool is_ram(const char * str);
bool is_register(const char * str);
int get_address_abel(const Label * lbs, const int labelCount, const char * strArg);
int get_num_reg(const char * strReg);
void get_arg_ram(char * strArgRam, const char * strArg);
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

void get_arg_ram(char * strArgRam, const char * strArg)
{
    assert(strArg    != NULL);
    assert(strArgRam != NULL);

    int i = 0;

    for(; strArg[i + 1] != ']'; i++)
        strArgRam[i] = strArg[i + 1];

    strArgRam[i] = '\0';
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

    bool isletter = true;

    for(int i = 0; str[i] != '\0'; i++)
    {
        if(!isalpha(str[i]))
        {
            isletter = false;
            break;
        }
    }

    return isletter;
}

bool is_label(const char * str)
{
    assert(str != NULL);

    bool isletter = true;

    for(int i = 1;  str[i] != '\0'; i++)
    {
        if(!isalpha(str[i]))
        {
            isletter = false;
            break;
        }
    }

    return str[0] == ':' && isletter;
}

bool is_number(const char * str)
{
    assert(str != NULL);

    bool isnum = true;

    for(int i = 0; str[i] != '\0'; i++)
    {
        if(!isdigit(str[i]))
        {
            isnum = false;
            break;
        }
    }

    return isnum;
}

bool is_ram(const char * str)
{
    assert(str != NULL);

    return str[0] == '[' && str[strlen(str) - 1] == ']';
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
        const int maxSizeCommand = 10;
        char command[maxSizeCommand] = {};
        sscanf(buf + ind, "%s", command);

        ind += strlen(command);

        if(is_label(command))
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
                    sscanf(buf + ind + 1, "%s", strArg);                                    \
                                                                                            \
                    ind += strlen(strArg) + 1;                                              \
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

    for(int i = 0; i < nLine; i++)
    {
        const int maxSizeCommand = 10;
        char command[maxSizeCommand] = {};
        sscanf(buf, "%s", command);

        buf += strlen(command);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                int numberCommand = num;                                                    \
                                                                                            \
                if(args)                                                                    \
                {                                                                           \
                    char strArg[MAXSIZEARG] = {};                                           \
                    sscanf(buf + 1, "%s", strArg);                                          \
                                                                                            \
                    buf += strlen(strArg) + 1;                                              \
                                                                                            \
                    if(is_ram(strArg))                                                      \
                    {                                                                       \
                        char strArgRam[MAXSIZEARG] = {};                                    \
                        get_arg_ram(strArgRam, strArg);                                     \
                        strcpy(strArgRam, strArg);                                          \
                        numberCommand |= RAM;                                               \
                    }                                                                       \
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
                    else if(is_label(strArg)){                                              \
                        instrs[(*sizeInstrs) + 1] = get_address_label(lbs,                  \
                                                                    labelCount, strArg);}   \
                                                                                            \
                    instrs[(*sizeInstrs)++] = numberCommand;                                \
                }                                                                           \
                                                                                            \
                else                                                                        \
                    instrs[*sizeInstrs] = numberCommand;                                    \
                                                                                            \
                (*sizeInstrs)++;                                                            \
                }                                                                           \

        #include "Commands.h"
        #undef DEF_CMD

        buf += 2; //Because file in rb format ends with 2 characters '\r' and '\n'
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
