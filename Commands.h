DEF_CMD(push, 1,  1,
{
    if(INSTR[INDEX] & BIT_FIELD_RAM)
    {
        if(INSTR[INDEX] & BIT_FIELD_REG)
            PUSH(RAMS[REGS[INSTR[++INDEX] - 1]]);

        else if(INSTR[INDEX] & BIT_FIELD_IMM)
            PUSH(RAMS[INSTR[++INDEX] - 1]);
    }

    else if(INSTR[INDEX] & BIT_FIELD_REG)
        PUSH(REGS[INSTR[++INDEX] - 1]);

    else if(INSTR[INDEX] & BIT_FIELD_IMM)
        PUSH(INSTR[++INDEX]);
})

DEF_CMD(pop,  11, 1,
{
    int value = 0;

    POP(value);

    if(INSTR[INDEX] & BIT_FIELD_RAM)
    {
        if(INSTR[INDEX] & BIT_FIELD_REG)
            RAMS[REGS[INSTR[++INDEX] - 1]] = value;

        else if(INSTR[INDEX] & BIT_FIELD_IMM)
            RAMS[INSTR[++INDEX] - 1] = value;
    }

    else if(INSTR[INDEX] & BIT_FIELD_REG)
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

DEF_CMD(sqrt_, 30, 0,
{
    int value = 0;
    POP(value);

    PUSH(SQRT(value));
})

DEF_CMD(sin_, 27, 0,
{
    int value = 0;
    POP(value);

    value = -5 * SIN(M_PI / 180 * value);
    PUSH(value);
})

DEF_CMD(cos_, 28, 0,
{
    int value = 0;
    POP(value);

    value = 5 * COS(M_PI / 180 * value);
    PUSH(value);
})

#define BINARY_OPERATION(sign)      \
    int value1 = 0;                 \
    int value2 = 0;                 \
                                    \
    POP(value1);                    \
    POP(value2);                    \
                                    \
    PUSH(value2 sign value1);       \

DEF_CMD(div_,  3,  0,
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
    INDEX = INSTR[INDEX + 1] - 1;
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

DEF_CMD(call, 18, 1,
{
    PUSH(INDEX + 1);

    INDEX = INSTR[INDEX + 1] - 1;
})

DEF_CMD(ret, 19, 0,
{
    int value = 0;

    POP(value);

    INDEX = value;
})
