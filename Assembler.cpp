#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Commons.h"
#include "File.h"

int GetNumReg(const char *);
void WriteBytecode(const char * buffer, const int nLine);                                                                   \

int main()
{
    const char * nameFile  = "ASM.txt";
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
    WriteBytecode(buf, nLine);

    free(buf);

    return 0;
}

void WriteBytecode(const char * buf, const int nLine)
{
    assert(buf != NULL);
    assert(nLine > 0);

    const char * nameFile = "Bytecode.bin";
    FILE * fp = fopen(nameFile, "wb");

    if(fp == NULL)
        printf("Error work of File\n");

    int * instruct = (int *)calloc(2 * nLine, sizeof(int));

    if(instruct == NULL)
        printf("Pointer on instructions is NULL\n");

    const int maxSizeCommand = 10;
    char command[maxSizeCommand] = {};

    int sizeInstruct = 0;

    for(int i = 0; sscanf(buf + i, "%s",  command) != EOF; i += 2)
    {
        i += strlen(command);

        #define DEF_CMD(name, num, args, code)                                      \
            if(!strcmp(command, #name))                                             \
            {                                                                       \
                if(args)                                                            \
                {                                                                   \
                    int value = 0;                                                  \
                    const int maxSizeReg = 5;                                       \
                    char strReg[maxSizeReg] = {};                                   \
                    if(sscanf(buf + i + 1, "%d", &value))                           \
                    {                                                               \
                        instruct[sizeInstruct++] = num | IMM;                       \
                        char str[5] = {};                                           \
                        i += strlen(itoa(value, str, 10)) + 1;                      \
                        instruct[sizeInstruct++] = value;                           \
                    }                                                               \
                    else if(sscanf(buf + i + 1, "%s", strReg))                      \
                    {                                                               \
                        instruct[sizeInstruct++] = num | REG;                       \
                        i += strlen(strReg) + 1;                                    \
                        instruct[sizeInstruct++] = GetNumReg(strReg);               \
                    }                                                               \
                }                                                                   \
                else                                                                \
                    instruct[sizeInstruct++] = num;                                 \
            }                                                                       \

        #include "Command.h"
        #undef DEF_CMD
    }

    fwrite(instruct, sizeof(int), sizeInstruct, fp);

    free(instruct);
    fclose(fp);
}

int GetNumReg(const char * strReg)
{
    for(int i = 0; i < REGISTERS_COUNT; i++)
    {
        if(!strcmp(strReg, REGS_NAME[i]))
            return i + 1;
    }
}
