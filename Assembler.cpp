#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef int instruct_t;

const int IMM = 1 << 4;
const int REG = 1 << 5;
//const uint8_t OSY = 1 << 6;

enum opcode_t
{
    HLT = 12,
    IN  =  2,
    PUSH = 1,
    DIV =  3,
    SUB =  4,
    OUT =  7,
    ADD =  5,
    MUL =  6,
    POP = 11
};

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

    free(buffer);

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

    if(instructions == NULL)
        printf("Pointer on instructions is NULL\n");

    const int maxSizeCommand = 10;
    char command[maxSizeCommand] = {};

    int countObject = 0;

    for( ; sscanf(buffer, "%s",  command) != EOF; buffer += 2)
    {
        buffer += strlen(command);

        if(!strcmp(command, "push"))
        {
            instruct_t value = 0;

            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            if(sscanf(buffer + 1, "%d", &value))
            {
                instructions[countObject++] = PUSH | IMM;

                char str[5] = {};
                buffer += strlen(itoa(value, str, 10)) + 1;

                instructions[countObject++] = value;
            }

            else if(sscanf(buffer + 1, "%s", strReg))
            {
                instructions[countObject++] = PUSH | REG;

                buffer += strlen(strReg) + 1;

                instructions[countObject++] = strReg[1] - 'a' + 1;
            }
        }

        else if(!strcmp(command, "pop"))
        {
            instructions[countObject++] = POP | REG;

            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            sscanf(buffer + 1, "%s", strReg);

            instructions[countObject++] = strReg[1] - 'a' + 1;

            buffer += strlen(strReg) + 1;
        }

        else if(!strcmp(command, "in"))
            instructions[countObject++] = IN;

        else if(!strcmp(command, "out"))
            instructions[countObject++] = OUT;

        else if(!strcmp(command, "HLT"))
            instructions[countObject++] = HLT;

        else if(!strcmp(command, "div"))
            instructions[countObject++] = DIV;

        else if(!strcmp(command, "add"))
            instructions[countObject++] = ADD;

        else if(!strcmp(command, "mul"))
            instructions[countObject++] = MUL;

        else if(!strcmp(command, "sub"))
            instructions[countObject++] = SUB;

        else
            printf("Unkown command\n");
    }

    fwrite(instructions, sizeof(instruct_t), countObject, fpout);

    free(instructions);
    fclose(fpout);
}
