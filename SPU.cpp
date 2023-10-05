#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Stack.h"
#include "Command.h"

const char * nameFile = "ASM.txt";

int main()
{
    FILE *fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("can't open file\n");
        return -1;
    }

    myStack stk = {};

    StackCtor(&stk);

    char command[100] = {};

    while(fscanf(fp, "%s",  command) != EOF)
        MakeCommand(fp, command, &stk);

    fclose(fp);

    return 0;
}
