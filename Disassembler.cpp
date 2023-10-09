#include <assert.h>
#include <stdio.h>

const char * nameFileIn  = "ByteCode.txt";
const char * nameFileOut = "ASMcopy.txt";

void FillASM(FILE * fpout, FILE * fpin, const int command)
{
    assert(fpout   != NULL);
    assert(fpin    != NULL);

    switch(command)
    {
        case -1: fprintf(fpout, "HLT\n");
            break;
        case 0: fprintf(fpout, "in\n");
            break;
        case 1: fprintf(fpout, "push ");
            double value;    // double value = 0; - don't work ???
            fscanf(fpin, "%lf", &value);
            fprintf(fpout, "%.2lf\n", value);
            break;
        case 2: fprintf(fpout, "div\n");
            break;
        case 3: fprintf(fpout, "sub\n");
            break;
        case 4: fprintf(fpout, "out\n");
            break;
        case 5: fprintf(fpout, "add\n");
            break;
        case 6: fprintf(fpout, "mul\n");
            break;
        default: fprintf(fpout, "Unknown command\n");
            break;
    }
}

int main()
{
    FILE * fpin = fopen(nameFileIn, "rb");

    if(fpin == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    int command = 0;

    FILE * fpout = fopen(nameFileOut, "wb");

    while(fscanf(fpin, "%d", &command) != EOF)
        FillASM(fpout, fpin, command);

    fclose(fpin);
    fclose(fpout);

    return 0;
}
