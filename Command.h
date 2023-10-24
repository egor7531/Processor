DEF_CMD(PUSH, 1,  1,
{
    if(*(DEF_INSTR + DEF_INDEX) & IMM)
        DEF_PUSH(*(DEF_INSTR + (++DEF_INDEX)));

    else if(*(DEF_INSTR + DEF_INDEX) & REG)
        DEF_PUSH(DEF_REG(*(DEF_INSTR + (++DEF_INDEX)) - 1));

    else
        printf("Syntax error\n");
})

DEF_CMD(HLT,  12, 0, )

DEF_CMD(IN,   2,  0,
{
    int value = 0;
    printf("Enter number: ");
    scanf("%d", &value);

    DEF_PUSH(value);
})

DEF_CMD(DIV,  3,  0,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    DEF_PUSH(value2 / value1);
})

DEF_CMD(SUB,  4,  0,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    DEF_PUSH(value2 - value1);
})

DEF_CMD(OUT,  7,  0,
{
    int value = 0;
    DEF_POP(value);

    printf("Answer: %d\n", value);
})

DEF_CMD(ADD,  5,  0,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    DEF_PUSH(value2 + value1);
})

DEF_CMD(MUL,  6,  0,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    DEF_PUSH(value2 * value1);
})

DEF_CMD(POP,  11, 1,
{
    int value = 0;

    DEF_POP(value);

    DEF_REG(*(DEF_INSTR + (++DEF_INDEX)) - 1) = value;
})

DEF_CMD(JUMP, 15, 1,
{
    DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;
})

DEF_CMD(JA,  20, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 > value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

DEF_CMD(JAE, 21, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 >= value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

DEF_CMD(JB,  22, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 < value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

DEF_CMD(JBE, 23, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 <= value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

DEF_CMD(JE, 24, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 == value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

DEF_CMD(JNE, 25, 1,
{
    int value1 = 0;
    int value2 = 0;

    DEF_POP(value1);
    DEF_POP(value2);

    if(value2 != value1)
        DEF_INDEX = *(DEF_INSTR + (++DEF_INDEX)) - 1;

    else
        DEF_INDEX++;
})

