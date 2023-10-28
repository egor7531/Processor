DEF_CMD(PUSH, 1,  1,
{
    if(DEF_INSTR[DEF_INDEX] & RAM)
    {
        if(DEF_INSTR[DEF_INDEX] & REG)
            DEF_PUSH(DEF_RAM[DEF_REG[DEF_INSTR[++DEF_INDEX] - 1]]);

        else if(DEF_INSTR[DEF_INDEX] & IMM)
            DEF_PUSH(DEF_RAM[DEF_INSTR[++DEF_INDEX] - 1]);
    }

    else if(DEF_INSTR[DEF_INDEX] & REG)
        DEF_PUSH(DEF_REG[DEF_INSTR[++DEF_INDEX] - 1]);

    else if(DEF_INSTR[DEF_INDEX] & IMM)
        DEF_PUSH(DEF_INSTR[++DEF_INDEX]);
})

DEF_CMD(POP,  11, 1,
{
    int value = 0;

    DEF_POP(value);

    if(DEF_INSTR[DEF_INDEX] & RAM)
    {
        if(DEF_INSTR[DEF_INDEX] & REG)
            DEF_RAM[DEF_REG[DEF_INSTR[++DEF_INDEX] - 1]] = value;

        else if(DEF_INSTR[DEF_INDEX] & IMM)
            DEF_RAM[DEF_INSTR[++DEF_INDEX] - 1] = value;
    }

    else if(DEF_INSTR[DEF_INDEX] & REG)
        DEF_REG[DEF_INSTR[++DEF_INDEX] - 1] = value;
})

DEF_CMD(HLT,  12, 0, )

DEF_CMD(IN,   2,  0,
{
    int value = 0;
    printf("Enter number: ");
    scanf("%d", &value);

    DEF_PUSH(value);
})

DEF_CMD(OUT,  7,  0,
{
    int value = 0;
    DEF_POP(value);

    printf("Answer: %d\n", value);
})

#define BINARY_OPERATION(sign)      \
    int value1 = 0;                 \
    int value2 = 0;                 \
                                    \
    DEF_POP(value1);                \
    DEF_POP(value2);                \
                                    \
    DEF_PUSH(value2 sign value1);   \

DEF_CMD(DIV,  3,  0,
{
    BINARY_OPERATION(/);
})

DEF_CMD(SUB,  4,  0,
{
    BINARY_OPERATION(-);

})

DEF_CMD(ADD,  5,  0,
{
    BINARY_OPERATION(+);
})

DEF_CMD(MUL,  6,  0,
{
    BINARY_OPERATION(*);
})

DEF_CMD(JUMP, 15, 1,
{
    DEF_INDEX = DEF_INSTR[++DEF_INDEX] - 1;
})

#define COND_JUMP(sign)                             \
    int value1 = 0;                                 \
    int value2 = 0;                                 \
                                                    \
    DEF_POP(value1);                                \
    DEF_POP(value2);                                \
                                                    \
    if(value2 sign value1)                          \
        DEF_INDEX = DEF_INSTR[++DEF_INDEX] - 1;     \
                                                    \
    else                                            \
        DEF_INDEX++;                                \

DEF_CMD(JA,  20, 1,
{
    COND_JUMP(>);
})

DEF_CMD(JAE, 21, 1,
{
    COND_JUMP(>=);
})

DEF_CMD(JB,  22, 1,
{
    COND_JUMP(<);
})

DEF_CMD(JBE, 23, 1,
{
    COND_JUMP(<=);
})

DEF_CMD(JE, 24, 1,
{
    COND_JUMP(==);
})

DEF_CMD(JNE, 25, 1,
{
    COND_JUMP(!=);

})

DEF_CMD(CALL, 17, 1,
{
    DEF_PUSH(DEF_INDEX + 1);

    DEF_INDEX = DEF_INSTR[++DEF_INDEX] - 1;
})

DEF_CMD(RET, 18, 0,
{
    int value = 0;

    DEF_POP(value);

    DEF_INDEX = value;
})
