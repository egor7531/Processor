#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Commons.h"
#include "File.h"

const int labelLenght = 10;

struct label
{
    int lbsAdress;
    char lbsName[labelLenght];
};

int GetNumReg(const char * strReg);
bool IsLabel(const char * command);
int GetAdressLabel(const label * lbs, const int lableCount, const char * strArg);
void WriteBytecode(const int * instruct, const int sizeInstruct);
int * GetInstuction(const char * buf, const int nLine, int * sizeInstruct,
                                                     const label * lbs, const int lableCount);                                                                \
label * GetLabels(const char * buf, int * lableCount);

int main()
{
    const char * nameFile  = "ASM4.txt";
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

bool IsLabel(const char * command)
{
    assert(command != NULL);

    return command[0] == ':';
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
                    int valueArg = 0;                                                       \
                    const int maxSizeStrArg = 10;                                           \
                    char strArg[maxSizeStrArg] = {};                                        \
                                                                                            \
                    if(sscanf(buf + i + 1, "%d", &valueArg))                                \
                    {                                                                       \
                        char str[5] = {};                                                   \
                        i += strlen(itoa(valueArg, str, 10)) + 1;                           \
                    }                                                                       \
                                                                                            \
                    else if(sscanf(buf + i + 1, "%s", strArg))                              \
                        i += strlen(strArg) + 1;                                            \
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
                    int valueArg = 0;                                                       \
                    const int maxSizeStrArg = 10;                                           \
                    char strArg[maxSizeStrArg] = {};                                        \
                                                                                            \
                    if(sscanf(buf + i + 1, "%d", &valueArg))                                \
                    {                                                                       \
                        instruct[(*sizeInstruct)++] = num | IMM;                            \
                                                                                            \
                        char str[5] = {};                                                   \
                        i += strlen(itoa(valueArg, str, 10)) + 1;                           \
                                                                                            \
                        instruct[(*sizeInstruct)++] = valueArg;                             \
                    }                                                                       \
                                                                                            \
                    else if(sscanf(buf + i + 1, "%s", strArg))                              \
                    {                                                                       \
                        if(IsLabel(strArg))                                                 \
                        {                                                                   \
                            instruct[(*sizeInstruct)++] = num | LAB;                        \
                            instruct[(*sizeInstruct)++] = GetAdressLabel(                   \
                                                            lbs, lableCount, strArg);       \
                        }                                                                   \
                                                                                            \
                        else                                                                \
                        {                                                                   \
                            instruct[(*sizeInstruct)++] = num | REG;                        \
                            instruct[(*sizeInstruct)++] = GetNumReg(strArg);                \
                        }                                                                   \
                                                                                            \
                        i += strlen(strArg) + 1;                                            \
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

    const char * nameFile = "Bytecode.bin";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Error work of File\n");

    fwrite(instruct, sizeof(int), sizeInstruct, fp);

    fclose(fp);
}
