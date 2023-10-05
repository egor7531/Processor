#include <assert.h>
#include <stdio.h>
#include <string.h>

const char * nameFileIn  = "ASM.txt";
const char * nameFileOut = "ByteCode.txt";

void FillByteCode(FILE * fpout, FILE * fpin, const char * command)
{
    assert(fpout   != NULL);
    assert(fpin    != NULL);
    assert(command != NULL);

    if(!strcmp(command, "push"))
    {
        double value = 0;
        fscanf(fpin, "%lf", &value);

        fprintf(fpout, "1 %.2lf\n", value);
    }

    else if(!strcmp(command, "in"))
        fprintf(fpout, "7\n");

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

int main()
{
    FILE * fpin = fopen(nameFileIn, "rb");

    if(fpin == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    char command[100] = {};

    FILE * fpout = fopen(nameFileOut, "wb");

    while(fscanf(fpin, "%s",  command) != EOF)
        FillByteCode(fpout, fpin, command);

    fclose(fpin);
    fclose(fpout);

    return 0;
}
