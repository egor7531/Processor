#ifndef COMMONS_H_INCLUDED
#define COMMONS_H_INCLUDED

const int BIT_FIELD_CMD = 0b11111;
const int BIT_FIELD_IMM = 1 << 5;
const int BIT_FIELD_REG = 1 << 6;
const int BIT_FIELD_RAM = 1 << 7;

static_assert( !(BIT_FIELD_IMM & BIT_FIELD_REG), "IMM and REG overlap");
static_assert( !(BIT_FIELD_IMM & BIT_FIELD_CMD), "IMM and CMD overlap");
static_assert( !(BIT_FIELD_REG & BIT_FIELD_CMD), "REG and CMD overlap");
static_assert( !(BIT_FIELD_RAM & BIT_FIELD_CMD), "RAM and CMD overlap");
static_assert( !(BIT_FIELD_RAM & BIT_FIELD_REG), "RAM and REG overlap");
static_assert( !(BIT_FIELD_RAM & BIT_FIELD_IMM), "RAM and IMM overlap");

const int MAX_LABEL_LENGHT = 50;
struct Label
{
    int labelAddress;
    char labelName[MAX_LABEL_LENGHT];
};

const char *NAME_REGS[] =
{
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
    "r08", "r09", "r10", "r11", "r12", "r13", "r14", "r15"
};

const int COUNT_REGISTERS = sizeof(NAME_REGS) / sizeof(NAME_REGS[0]);
const char *nameBinaryFile = "Bytecode.bin";

#define DEF_CMD(name, num, args, code)    \
    name = num,

enum opcode_t
{
    #include "Commands.h"
};

#undef DEF_CMD

#endif // COMMONS_H_INCLUDED
