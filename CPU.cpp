#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys\stat.h>
#include <ctype.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

//#include "TXLib.h"
#include "config.h"
#include "stack.h"
#include "CPU.h"


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Errors_Processing(ARGV_ERROR);
    }

    char* inp_file_name = argv[1];

    struct CPU cpu = {};
    Stack stk = {};
    Stack call_adress_stk = {};
    StackCtor(&stk, 10);
    StackCtor(&call_adress_stk, 5);

    Get_Code(&cpu, inp_file_name);

    Do_What_Said(&cpu, &stk, &call_adress_stk);

    return 0;
}

void Get_Code(struct CPU* cpu, char* inp_file_name)
{
    FILE* code_file = fopen(inp_file_name, "rb");
    if (code_file == NULL)
    {
        Errors_Processing(NO_CODE_FILE);
    }

    char signature[SIGNATURE_LEN] = { 0 };
   
    Check_Signature(code_file);

    char c = 0;
    fscanf(code_file, "%c", &c); //leave \n
    fscanf(code_file, "%d", &cpu->pointer_max);
    fscanf(code_file, "%c", &c); //leave \n

    CPU_Ctor(cpu);
    
    int n = fread(cpu->code, sizeof(char), cpu->pointer_max, code_file);

    fclose(code_file);
}

void Check_Signature(FILE* code_file)
{
    char signature[SIGNATURE_LEN] = {0};
    fread(signature, sizeof(char), SIGNATURE_LEN-1, code_file);
    if (strcmp(NEEDED_SIGNATURE, signature) != 0)
    {
        Errors_Processing(WRONG_SIGNATURE);
    }
}

void CPU_Ctor(struct CPU* cpu)
{
    cpu->code = (char*)calloc(cpu->pointer_max, sizeof(char));
    assert(cpu->code != NULL);

    cpu->regs =  (int*)calloc(REG_SIZE, sizeof(int));
    assert(cpu->regs != NULL);

    cpu->RAM  =  (int*)calloc(MEMORY_SIZE, sizeof(int));
    assert(cpu->RAM != NULL);

    cpu->ip = 0;
}

void Do_What_Said(struct CPU* cpu, Stack* stk, Stack* call_adress_stk)
{
    int iii = 0;
    int n;
    #define DEF_CMD(name, num, arg, ...)\
        case  cmd_##name:               \
            n = cmd_##name;             \
            /*printf("I'm in a command %d pointer is %d\n\n", *(cpu->code + cpu->ip), cpu->ip);*/\
            __VA_ARGS__                 \
            break;                      

    while (cpu->ip < cpu->pointer_max)
    {
        switch (CMD_MASK & *(cpu->code + cpu->ip))
        {
            #include "cmd.h"
            
            default:
                Errors_Processing(UNKNOWN_COMMAND);
                break;
        }
    }

    #undef DEF_CMD
}

int* Get_arg(struct CPU* cpu)
{
    int current_ip = sizeof(char);
    int* arg = 0;

    if ((*(cpu->code + cpu->ip) & (1 << memory)) != 0)
    {
        int memory_cell = 0;

        if ((*(cpu->code + cpu->ip) & (1 << imm_const)) != 0)
        {
            memory_cell += *((int*)(cpu->code + cpu->ip + current_ip));
            current_ip += sizeof(int);
        }

        if ((*(cpu->code + cpu->ip) & (1 << reg)) != 0)
        {
            memory_cell += cpu->regs[*(int*)(cpu->code + cpu->ip + current_ip)]/ACCURACY;
            current_ip += sizeof(int);
        }

        if (memory_cell >= MEMORY_SIZE)
        {
            Errors_Processing(MEM_SEG_FAULT);
        }

        
        arg = &cpu->RAM[memory_cell];
    }


    else
    {
        if ((*(cpu->code + cpu->ip) & (1 << imm_const)) != 0)
        {
            arg = (int*)(*((int*)(cpu->code + cpu->ip + current_ip)));
            current_ip += sizeof(int);
        }

        if ((*(cpu->code + cpu->ip) & (1 << reg)) != 0)
        {
            if ((*(cpu->code + cpu->ip) & (1 << imm_const)) != 0)
            {
                arg += cpu->regs[*(int*)(cpu->code + cpu->ip + current_ip)];
                current_ip += sizeof(int);
            }

            else
            {
                arg = &cpu->regs[*(int*)(cpu->code + cpu->ip + current_ip)];
                current_ip += sizeof(int);
            }
        }
    }

    cpu->ip += current_ip;
    return arg;
}

void Print_Memory(struct CPU* cpu)
{
    ClearScreen();
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (*(cpu->RAM + i) == 0) printf(". ");
        else printf("# ");
        
        if ((i+1)%100 == 0) printf("\n");
    }
    //sleep(1);
}

void ClearScreen()
  {
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
  }

void Errors_Processing(int Err)
{
    printf("%s", Errors_Names[Err]);
    exit(Err);
}