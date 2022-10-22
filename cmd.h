DEF_CMD(hlt, 0, 0,
    {
        return;
    })

DEF_CMD(push, 1, 1,
    {
        int* arg_shit = Get_arg(cpu);
        int arg = 0;
        if (((*(cpu->code + cpu->ip - sizeof(char) - sizeof(int)) & (1 << imm_const)) != 0) && 
            ((*(cpu->code + cpu->ip - sizeof(char) - sizeof(int)) & (1 << memory)) == 0))
        {
            arg = ((int)arg_shit)*ACCURACY;
        }
        else
        {
            arg = *arg_shit;
        }
        StackPush(stk, arg);
    })

DEF_CMD(pop, 2, 1,
    {
        int* arg = Get_arg(cpu);
        int element = StackPop(stk);
        *(arg) = element;
    })

DEF_CMD(add, 3, 0,
    {
        StackPush(stk, StackPop(stk) + StackPop(stk));
        cpu->ip += sizeof(char);
    })

DEF_CMD(sub, 4, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        StackPush(stk, b - a);
        cpu->ip += sizeof(char);
    })

DEF_CMD(mul, 5, 0, 
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        StackPush(stk, (((a/ACCURACY) * (b))));
        cpu->ip += sizeof(char);
    })

DEF_CMD(div, 6, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (a == 0)
        {
            printf("impossible to divide by zero");
            exit(DIV_BY_ZERO);
        }
        StackPush(stk, (int)(((double)b / (double)a)*ACCURACY));
        cpu->ip += sizeof(char);
    })

DEF_CMD(jmp, 7, 0,
    {
        cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
    })

DEF_CMD(jb, 8, 0,
    {
        printf("Iteration number %d\n", iii);
        iii++;
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b < a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(jbe, 9, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b <= a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(ja, 10, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b > a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(jae, 11, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b >= a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(je, 12, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b == a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(jne, 13, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (b != a) cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
        else cpu->ip += sizeof(char) + sizeof(int);
        StackPush(stk, b);
        StackPush(stk, a);
    })

DEF_CMD(call, 14, 0,
    {
        StackPush(call_adress_stk, (int)(cpu->ip + sizeof(char) + sizeof(int)));
        cpu->ip = *((int*)(cpu->code + cpu->ip + sizeof(char)));
    })

DEF_CMD(ret, 15, 0,
    {
        cpu->ip = (int)StackPop(call_adress_stk);
    })

DEF_CMD(in, 16, 0,
    {
        int arg = 0;
        printf("Enter a number: ");
        scanf("%d", &arg);
        StackPush(stk, arg * ACCURACY);
        cpu->ip += sizeof(char);
    })

DEF_CMD(out, 17, 0,
    {
        double a = StackPop(stk);
        printf("%.3lf\n", (a / ACCURACY));
        cpu->ip += sizeof(char);
    })

DEF_CMD(sqrt, 18, 0, 
    {
        int a = StackPop(stk);
        int b = (int)(ACCURACY * sqrt(a / ACCURACY));
        StackPush(stk, b);
        cpu->ip += sizeof(char);
    })

DEF_CMD(ram, 19, 0, 
    {
        Print_Memory(cpu);
        cpu->ip += sizeof(char);
    })

DEF_CMD(mod, 20, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        int c = (b/ACCURACY) % (a/ACCURACY);
        if (a == 0)
        {
            printf("impossible to divide by zero");
            exit(DIV_BY_ZERO);
        }
        StackPush(stk, c*ACCURACY);
        cpu->ip += sizeof(char);
    })

DEF_CMD(divv, 21, 0,
    {
        int a = StackPop(stk);
        int b = StackPop(stk);
        if (a == 0)
        {
            printf("impossible to divide by zero");
            exit(DIV_BY_ZERO);
        }
        StackPush(stk, (b / a)*ACCURACY);
        cpu->ip += sizeof(char);
    })

DEF_CMD(copy, 22, 0, 
    {
        int a = StackPop(stk);
        StackPush(stk, a);
        StackPush(stk, a);
        cpu->ip += sizeof(char);
    })
