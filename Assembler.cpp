#include <assert.h>
#include <stdio.h>
#include <string.h>

void WriteByteCode(FILE * fpin)
{
    assert(fpin != NULL);

    const char * nameFileOut = "ByteCode.txt";
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

int main()
{
    const char * nameFileIn  = "ASM.txt";

    FILE * fpin = fopen(nameFileIn, "rb");

    if(fpin == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    WriteByteCode(fpin);

    fclose(fpin);

    return 0;
}
