DEF_CMD(PUSH, 1,  1,
                    {
                        assert(prc != NULL);
                        assert(command > 0);

                        prc->instruct++;

                        if(command & IMM)
                            StackPush(&prc->stk, *(prc->instruct ));

                        else if(command & REG)
                            StackPush(&prc->stk, prc->registers[*(prc->instruct) - 1]);

                        else
                            printf("Syntax error\n");
                    })

DEF_CMD(HLT,  12, 0, )

DEF_CMD(IN,   2,  0,
                    {
                        assert(prc != NULL);

                        int value = 0;

                        printf("Enter number: ");
                        scanf("%d", &value);

                        StackPush(&prc->stk, value);
                    })

DEF_CMD(DIV,  3,  0,
                    {
                        assert(prc != NULL);

                        int value1 = 0;
                        int value2 = 0;

                        StackPop(&prc->stk, &value1);
                        StackPop(&prc->stk, &value2);

                        StackPush(&prc->stk, value2 / value1);
                    })

DEF_CMD(SUB,  4,  0,
                    {
                        assert(prc != NULL);

                        int value1 = 0;
                        int value2 = 0;

                        StackPop(&prc->stk, &value1);
                        StackPop(&prc->stk, &value2);

                        StackPush(&prc->stk, value2 - value1);
                    })

DEF_CMD(OUT,  7,  0,
                    {
                        assert(prc != NULL);

                        const char * nameFile = "Result.txt";
                        FILE * fp = fopen(nameFile, "wb"); //Нужно делать проферку файла, если мы в него записываем, а не читаем?

                        int value = 0;

                        StackPop(&prc->stk, &value);

                        fprintf(fp, "%d", value);

                        fclose(fp);
                    })

DEF_CMD(ADD,  5,  0,
                    {
                        assert(prc != NULL);

                        int value1 = 0;
                        int value2 = 0;

                        StackPop(&prc->stk, &value1);
                        StackPop(&prc->stk, &value2);

                        StackPush(&prc->stk, value2 + value1);
                    })

DEF_CMD(MUL,  6,  0,
                    {
                        assert(prc != NULL);

                        int value1 = 0;
                        int value2 = 0;

                        StackPop(&prc->stk, &value1);
                        StackPop(&prc->stk, &value2);

                        StackPush(&prc->stk, value2 * value1);
                    })

DEF_CMD(POP,  11, 1,
                    {
                        assert(prc != NULL);

                        int value = 0;

                        StackPop(&prc->stk, &value);

                        prc->instruct++;

                        prc->registers[*(prc->instruct) - 1] = value;
                    })
