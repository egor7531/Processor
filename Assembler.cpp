#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Commons.h"
#include "File.h"

const int labelLenght = 10;

struct label
{
    int lbsAdress;
    char lbsName[labelLenght];
};

bool IsLabel(const char * str);
bool IsNumber(const char * str);
bool IsRam(const char * str);
bool IsRegister(const char * str);
int GetAdressLabel(const label * lbs, const int lableCount, const char * strArg);
int GetNumReg(const char * strReg);
void GetArgRam(char * strArgRam, const char * strArg);
void WriteBytecode(const int * instruct, const int sizeInstruct);
int * GetInstuction(const char * buf, const int nLine, int * sizeInstruct,
                                    const label * lbs, const int lableCount);
label * GetLabels(const char * buf, int * lableCount);

int main()
{
    const char * nameFile  = "ASM1.txt";
    FILE * fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    int fileSize = GetFileSize(fp);
    char * buf = GetFileContent(fp, fileSize);

    fclose(fp);

    int nLine =  GetCountLine(buf, fileSize);

    int lableCount = 0;
    label * lbs = GetLabels(buf, &lableCount);

    int sizeInstruct = 0;
    int * instruct = GetInstuction(buf, nLine, &sizeInstruct, lbs, lableCount);

    WriteBytecode(instruct, sizeInstruct);

    free(buf);
    free(instruct);
    free(lbs);

    return 0;
}

int GetNumReg(const char * strReg)
{
    assert(strReg != NULL);

    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(strReg, REGS_NAME[i]))
            return i + 1;
    }
}

void GetArgRam(char * strArgRam, const char * strArg)
{
    assert(strArg    != NULL);
    assert(strArgRam != NULL);

    int j = 0;

    for(int i = 1; strArg[i] != ']'; i++)
        strArgRam[j++] = strArg[i];

    strArgRam[j] = '\0';
}

int GetAdressLabel(const label * lbs, const int lableCount, const char * strArg)
{
    assert(lbs    != NULL);
    assert(strArg != NULL);
    assert(lableCount > 0);

    for(int i = 0; i < lableCount; i++)
    {
        if(!strcmp(lbs[i].lbsName, strArg))
            return lbs[i].lbsAdress;
    }
}

bool IsRegister(const char * str)
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

bool IsLabel(const char * str)
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

bool IsNumber(const char * str)
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

bool IsRam(const char * str)
{
    assert(str != NULL);

    return str[0] == '[' && str[strlen(str) - 1] == ']';
}

label * GetLabels(const char * buf, int * lableCount)
{
    assert(buf        != NULL);
    assert(lableCount != NULL);

    const int MAX_LABEL_COUNT = 10;
    label * lbs = (label *)calloc(MAX_LABEL_COUNT, sizeof(label));

    const int maxSizeCommand = 10;
    char command[maxSizeCommand] = {};

    int countInstruct = 0;

    for(int i = 0; sscanf(buf + i, "%s",  command) != EOF; i += 2)
    {
        i += strlen(command);

        if(IsLabel(command))
        {
            strcpy(lbs[*lableCount].lbsName, command);
            lbs[*lableCount].lbsAdress = countInstruct;
            (*lableCount)++;
        }

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                countInstruct++;                                                            \
                if(args)                                                                    \
                {                                                                           \
                    countInstruct++;                                                        \
                                                                                            \
                    const int maxSizeStrArg = 10;                                           \
                    char strArg[maxSizeStrArg] = {};                                        \
                    sscanf(buf + i + 1, "%s", strArg);                                      \
                                                                                            \
                    i += strlen(strArg) + 1;                                                \
                }                                                                           \
            }                                                                               \

        #include "Command.h"
        #undef DEF_CMD
    }

    return lbs;
}

int * GetInstuction(const char * buf, const int nLine, int * sizeInstruct,
                                    const label * lbs, const int lableCount)
{
    assert(buf          != NULL);
    assert(lbs          != NULL);
    assert(sizeInstruct != NULL);
    assert(nLine      >  0);
    assert(lableCount >= 0);

    int * instruct = (int *)calloc(2 * nLine, sizeof(int));

    if(instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    const int maxSizeCommand = 10;
    char command[maxSizeCommand] = {};

    int sizeInstruc = 0;

    for(int i = 0; sscanf(buf + i, "%s",  command) != EOF; i += 2)
    {
        i += strlen(command);

        #define DEF_CMD(name, num, args, code)                                              \
            if(!strcmp(command, #name))                                                     \
            {                                                                               \
                if(args)                                                                    \
                {                                                                           \
                    const int maxSizeStrArg = 10;                                           \
                    char strArg[maxSizeStrArg] = {};                                        \
                    sscanf(buf + i + 1, "%s", strArg);                                      \
                                                                                            \
                    i += strlen(strArg) + 1;                                                \
                                                                                            \
                    if(IsRam(strArg))                                                       \
                    {                                                                       \
                        char strArgRam[maxSizeStrArg] = {};                                 \
                        GetArgRam(strArgRam, strArg);                                       \
                                                                                            \
                        if(IsNumber(strArgRam))                                             \
                        {                                                                   \
                            instruct[(*sizeInstruct)++] = num | RAM | IMM;                  \
                            instruct[(*sizeInstruct)++] = atoi(strArgRam);                  \
                        }                                                                   \
                                                                                            \
                        else if(IsRegister(strArgRam))                                      \
                        {                                                                   \
                            instruct[(*sizeInstruct)++] = num | RAM | REG;                  \
                            instruct[(*sizeInstruct)++] = GetNumReg(strArgRam);             \
                        }                                                                   \
                    }                                                                       \
                                                                                            \
                    else if(IsNumber(strArg))                                               \
                    {                                                                       \
                        instruct[(*sizeInstruct)++] = num | IMM;                            \
                        instruct[(*sizeInstruct)++] = atoi(strArg);                         \
                    }                                                                       \
                                                                                            \
                    else if(IsLabel(strArg))                                                \
                    {                                                                       \
                        instruct[(*sizeInstruct)++] = num | LAB;                            \
                        instruct[(*sizeInstruct)++] = GetAdressLabel(                       \
                                                            lbs, lableCount, strArg);       \
                    }                                                                       \
                                                                                            \
                    else if(IsRegister(strArg))                                             \
                    {                                                                       \
                        instruct[(*sizeInstruct)++] = num | REG;                            \
                        instruct[(*sizeInstruct)++] = GetNumReg(strArg);                    \
                    }                                                                       \
                }                                                                           \
                                                                                            \
                else                                                                        \
                    instruct[(*sizeInstruct)++] = num;                                      \
            }                                                                               \

        #include "Command.h"
        #undef DEF_CMD
    }

    return instruct;
}

void WriteBytecode(const int * instruct, const int sizeInstruct)
{
    assert(instruct != NULL);
    assert(sizeInstruct > 0);

    //for(int i = 0; i < sizeInstruct; i++)
    //    printf("%d\n",instruct[i]);

    const char * nameFile = "Bytecode.bin";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Error work of File\n");

    fwrite(instruct, sizeof(int), sizeInstruct, fp);

    fclose(fp);
}
