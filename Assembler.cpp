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
int GetAdressLabel(const label * lbs, const int MAX_LABEL_COUNT, const char * strArg);
void WriteBytecode(const int * instruct, const int sizeInstruct);
int * GetInstuction(const char * buf, const int nLine, int * sizeInstruct);                                                                \
label * GetLabels(const char * buf);

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

    label * lbs = GetLabels(buf);

    int sizeInstruct = 0;
    int * instruct = GetInstuction(buf, nLine, &sizeInstruct);

    WriteBytecode(instruct, sizeInstruct);

    free(buf);
    free(instruct);

    return 0;
}

int GetNumReg(const char * strReg)
{
    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(strReg, REGS_NAME[i]))
            return i + 1;
    }
}

int GetAdressLabel(const label * lbs, const int MAX_LABEL_COUNT, const char * strArg)
{
    for(int i = 0; i < MAX_LABEL_COUNT; i++)
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

label * GetLabels(const char * buf)
{
    const int MAX_LABEL_COUNT = 10;
    label * lbs = (label *)calloc(MAX_LABEL_COUNT, sizeof(label));

    for(int i = 0; sscanf(buf + i, "%s",  command) != EOF; i += 2)
    {
        printf("%s\n", command);
    }
}

int * GetInstuction(const char * buf, const int nLine, int * sizeInstruct)
{
    assert(buf != NULL);
    assert(nLine > 0);
    assert(sizeInstruct != NULL);

    int * instruct = (int *)calloc(2 * nLine, sizeof(int));

    if(instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    int LABEL_COUNT = 0;

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
                                                            lbs, MAX_LABEL_COUNT, strArg);  \
                        }                                                                   \
                                                                                            \
                        else                                                                \
                        {                                                                   \
                            instruct[(*sizeInstruct)++] = num | REG;                        \
                            instruct[(*sizeInstruct)++] = GetNumReg(strArg);                \
                        }                                                                   \
                                                                                            \
                        i += strlen(strArg) + 1;                                            \
                                                                                            \
                    }                                                                       \
                }                                                                           \
                                                                                            \
                else                                                                        \
                    instruct[(*sizeInstruct)++] = num;                                      \
            }                                                                               \

            if(IsLabel(command))
            {
                strcpy(lbs[LABEL_COUNT].lbsName, command);
                lbs[LABEL_COUNT].lbsAdress = *sizeInstruct;
                LABEL_COUNT++;
            }

        #include "Command.h"
        #undef DEF_CMD
    }

    return instruct;
}

void WriteBytecode(const int * instruct, const int sizeInstruct)
{
    const char * nameFile = "Bytecode.bin";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Error work of File\n");

    fwrite(instruct, sizeof(int), sizeInstruct, fp);

    fclose(fp);
}
