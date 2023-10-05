#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "Command.h"

void Push(myStack * stk, FILE * fp)
{
    assert(stk != NULL);
    assert(fp  != NULL);

    double value = 0;

    fscanf(fp, "%lf", &value);

    StackPush(stk, value);
}

void In(myStack * stk)
{
    assert(stk != NULL);

    double value = 0;

    printf("Enter number: ");
    scanf("%lf", &value);

    StackPush(stk, value);
}

void Out(myStack * stk)
{
    assert(stk != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value = 0;

    StackPop(stk, &value);

    fprintf(fp, "%.2lf", value);

    fclose(fp);
}

void Hlt(myStack * stk)
{
    assert(stk != NULL);

    StackDtor(stk);
}

void Div(myStack * stk)
{
    assert(stk != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value1 = 0;
    double value2 = 0;

    StackPop(stk, &value1);
    StackPop(stk, &value2);

    StackPush(stk, value2 / value1);
}

void Add(myStack * stk)
{
    assert(stk != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value1 = 0;
    double value2 = 0;

    StackPop(stk, &value1);
    StackPop(stk, &value2);

    StackPush(stk, value2 + value1);
}

void Mul(myStack * stk)
{
    assert(stk != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value1 = 0;
    double value2 = 0;

    StackPop(stk, &value1);
    StackPop(stk, &value2);

    StackPush(stk, value2 * value1);
}

void Sub(myStack * stk)
{
    assert(stk != NULL);

    const char * nameFile = "Result.txt";

    FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

    double value1 = 0;
    double value2 = 0;

    StackPop(stk, &value1);
    StackPop(stk, &value2);

    StackPush(stk, value2 - value1);
}

void MakeCommand(FILE * fp, const char * command, myStack * stk)
{
    assert(fp      != NULL);
    assert(command != NULL);
    assert(stk     != NULL);

    if(!strcmp(command, "push"))
        Push(stk, fp);

    else if(!strcmp(command, "in"))
        In(stk);

    else if(!strcmp(command, "out"))
        Out(stk);

    else if(!strcmp(command, "HLT"))
        Hlt(stk);

    else if(!strcmp(command, "div"))
        Div(stk);

    else if(!strcmp(command, "add"))
        Add(stk);

    else if(!strcmp(command, "mul"))
        Mul(stk);

    else if(!strcmp(command, "sub"))
        Sub(stk);

    else
        printf("Unkown command\n");
}
