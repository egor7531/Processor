DEF_CMD(PUSH, 1,  1,
                    {
                        if(DEF_COMMAND & IMM)
                            DEF_PUSH(DEF_ARG);

                        else if(DEF_COMMAND & REG)
                            DEF_PUSH(DEF_REG(DEF_ARG));

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
                        const char * nameFile = "Result.txt";
                        FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

                        int value = 0;

                        DEF_POP(value);

                        fprintf(fp, "%d", value);

                        fclose(fp);
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

                        DEF_REG(DEF_ARG) = value;
                    })

