#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int GetFileSize(FILE * fp);
char * GetFileContent(FILE * fp, const int fileSize);
void WriteByteCode(const char * buffer);

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



    WriteByteCode(buffer);

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

    char * buf = (char *)cafferlloc(fileSize, sizeof(char));

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
void WriteByteCode(const char * buffer)
{
    assert(buffer != NULL);

    const char * nameFileOut = "ByteCode.bin";
    const int maxSizeCommand = 10;

    char command[maxSizeCommand] = {};

    FILE * fpout = fopen(nameFileOut, "wb");  //надо проверять на NULL?

    while(fscanf(fpin, "%s",  command) != EOF)
    {
        if(!strcmp(command, "push"))
        {
            double value = 0;

            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            if(fscanf(fpin, "%lf", &value))
                fprintf(fpout, "1 %.2lf\n", value);

            else if(fscanf(fpin, "%s", strReg))
                fprintf(fpout, "1 %s\n", strReg);
        }

        else if(!strcmp(command, "pop"))
        {
            const int maxSizeReg = 5;
            char strReg[maxSizeReg] = {};

            fscanf(fpin, "%s", strReg);

            fprintf(fpout, "11 %s\n", strReg);
        }

        else if(!strcmp(command, "in"))
            fprintf(fpout, "0\n");

        else if(!strcmp(command, "out"))
            fprintf(fpout, "4\n");

        else if(!strcmp(command, "HLT"))
            fprintf(fpout, "-1\n");

        else if(!strcmp(command, "div"))
            fprintf(fpout, "2\n");

        else if(!strcmp(command, "add"))
            fprintf(fpout, "5\n");

        else if(!strcmp(command, "mul"))
            fprintf(fpout, "6\n");

        else if(!strcmp(command, "sub"))
            fprintf(fpout, "3\n");

        else
            printf("Unkown command\n");
    }

    fclose(fpout);
}
