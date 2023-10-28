DEF_CMD(push, 1,  1,
{
    if(INSTR[INDEX] & RAM)
    {
        if(INSTR[INDEX] & REG)
            PUSH(RAMS[REGS[INSTR[++INDEX] - 1]]);

        else if(INSTR[INDEX] & IMM)
            PUSH(RAMS[INSTR[++INDEX] - 1]);
    }

    else if(INSTR[INDEX] & REG)
        PUSH(REGS[INSTR[++INDEX] - 1]);

    else if(INSTR[INDEX] & IMM)
        PUSH(INSTR[++INDEX]);
})

DEF_CMD(pop,  11, 1,
{
    int value = 0;

    POP(value);

    if(INSTR[INDEX] & RAM)
    {
        if(INSTR[INDEX] & REG)
            RAMS[REGS[INSTR[++INDEX] - 1]] = value;

        else if(INSTR[INDEX] & IMM)
            RAMS[INSTR[++INDEX] - 1] = value;
    }

    else if(INSTR[INDEX] & REG)
        REGS[INSTR[++INDEX] - 1] = value;
})

DEF_CMD(hlt,  12, 0, )

DEF_CMD(in,   2,  0,
{
    int value = 0;
    printf("Enter number: ");
    scanf("%d", &value);

    PUSH(value);
})

DEF_CMD(out,  7,  0,
{
    int value = 0;
    POP(value);

    printf("Answer: %d\n", value);
})

#define BINARY_OPERATION(sign)      \
    int value1 = 0;                 \
    int value2 = 0;                 \
                                    \
    POP(value1);                    \
    POP(value2);                    \
                                    \
    PUSH(value2 sign value1);       \

DEF_CMD(dlv,  3,  0,
{
    BINARY_OPERATION(/);
})

DEF_CMD(sub,  4,  0,
{
    BINARY_OPERATION(-);

})

DEF_CMD(add,  5,  0,
{
    BINARY_OPERATION(+);
})

DEF_CMD(mul,  6,  0,
{
    BINARY_OPERATION(*);
})

#undef BINARY_OPERATION

DEF_CMD(jump, 15, 1,
{
    INDEX = INSTR[++INDEX] - 1;
})

#define COND_JUMP(sign)                             \
    int value1 = 0;                                 \
    int value2 = 0;                                 \
                                                    \
    POP(value1);                                    \
    POP(value2);                                    \
                                                    \
    if(value2 sign value1)                          \
        INDEX = INSTR[++INDEX] - 1;                 \
                                                    \
    else                                            \
        INDEX++;                                    \

DEF_CMD(ja,  20, 1,
{
    COND_JUMP(>);
})

DEF_CMD(jae, 21, 1,
{
    COND_JUMP(>=);
})

DEF_CMD(jb,  22, 1,
{
    COND_JUMP(<);
})

DEF_CMD(jbe, 23, 1,
{
    COND_JUMP(<=);
})

DEF_CMD(je, 24, 1,
{
    COND_JUMP(==);
})

DEF_CMD(jne, 25, 1,
{
    COND_JUMP(!=);

})

#undef COND_JUMP

DEF_CMD(call, 17, 1,
{
    PUSH(INDEX + 1);

    INDEX = INSTR[++INDEX] - 1;
})

DEF_CMD(ret, 18, 0,
{
    int value = 0;

    POP(value);

    INDEX = value;
})
