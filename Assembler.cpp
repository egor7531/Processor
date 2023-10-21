#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef int instruct_t;

int GetFileSize(FILE * fp);
int GetCountLine(char * buf, int fileSize);
char * GetFileContent(FILE * fp, const int fileSize);
void WriteByteCode(const char * buffer, const int nLine);

int main()
{
    const char * nameFileIn  = "ASM.txt";

    FILE * fpin = fopen(nameFileIn, "rb"); //в каком режиме открывать? проблема в символе \r

    if(fpin == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    int fpinSize = GetFileSize(fpin);

    char * buffer = GetFileContent(fpin, fpinSize);

    fclose(fpin);

    int nLine =  GetCountLine(buffer, fpinSize);

    WriteByteCode(buffer, nLine);

    return 0;
}

int GetFileSize(FILE * fp)
{
    struct stat st;

    fstat(fileno(fp), &st);

    return st.st_size;
}

char * GetFileContent(FILE * fp, const int fileSize)
{
    assert(fp != NULL);
    assert(fileSize > 0);

    char * buf = (char *)calloc(fileSize, sizeof(char));

    if(buf == NULL)
        printf("Pointer on buffer is NULL\n");

    if(fread(buf, sizeof(char), fileSize, fp) != fileSize)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }

    return buf;
}

int GetCountLine(char * buf, int fileSize)
{
    assert(buf != NULL);
    assert(fileSize > 0);

    int nLine = 0;

    for(size_t i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
            nLine++;
    }

    return nLine;
}

void WriteByteCode(const char * buffer, const int nLine)
{
    assert(buffer != NULL);
    assert(nLine > 0);

    const char * nameFileOut = "ByteCode.bin";
    FILE * fpout = fopen(nameFileOut, "wb");  //надо проверять на NULL?

    if(fpout == NULL)
        printf("Error work of File\n");

    instruct_t * instructions = (instruct_t *)calloc(2 * nLine, sizeof(instruct_t));

    if(instructions == nullptr)
        printf("Pointer on instructions is NULL\n");

    const int maxSizeCommand = 10;
    char command[maxSizeCommand] = {};

    int i = 0;

    while(sscanf(buffer, "%s",  command) != EOF)
    {
        printf("%s\n", command);
        if(!strcmp(command, "push"))
        {
            buffer += 5;

            instructions[i++] = 1;

            instruct_t value = 0;

            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            if(sscanf(buffer, "%d", &value))
            {
                char str[5] = {};
                buffer += strlen(itoa(value, str, 10)) + 2;

                instructions[i++] = value;
            }

            else if(sscanf(buffer, "%s", strReg))
            {
                buffer += strlen(strReg) + 2;

                instructions[i++] = strReg[1] - 'a' + 1;
            }
        }

        else if(!strcmp(command, "pop"))
        {
            buffer += 4;

            instructions[i++] = 11;

            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            sscanf(buffer, "%s", strReg);

            instructions[i++] = strReg[1] - 'a' + 1;

            buffer += strlen(strReg) + 2;
        }

        else if(!strcmp(command, "in"))
        {
            buffer += 4;

            instructions[i++] = 2;
        }
        else if(!strcmp(command, "out"))
        {
            buffer += 5;

            instructions[i++] = 7;
        }

        else if(!strcmp(command, "HLT"))
        {
            buffer += 5;

            instructions[i++] = 12;
        }

        else if(!strcmp(command, "div"))
        {
            buffer += 5;

            instructions[i++] = 3;
        }

        else if(!strcmp(command, "add"))
        {
            buffer += 5;

            instructions[i++] = 5;
        }

        else if(!strcmp(command, "mul"))
        {
            buffer += 5;

            instructions[i++] = 6;
        }

        else if(!strcmp(command, "sub"))
        {
            buffer += 5;

            instructions[i++] = 4;
        }

        else
            printf("Unkown command\n");
    }

    fwrite(instructions,sizeof(instruct_t), i, fpout);

    fclose(fpout);
}
