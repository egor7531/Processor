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

void MakeCommand(FILE * fp, const int command, myStack * stk)
{
    assert(fp      != NULL);
    assert(stk     != NULL);

    switch(command)
    {
    case 0: In(stk);
            break;
    case 1: Push(stk, fp);
            break;
    case 4: Out(stk);
            break;
    case -1: Hlt(stk);
            break;
    case 2: Div(stk);
            break;
    case 5: Add(stk);
            break;
    case 6: Mul(stk);
            break;
    case 3: Sub(stk);
            break;
    default: printf("Unkown command\n");
            break;
    }
}
