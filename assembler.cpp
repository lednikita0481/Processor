#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys\stat.h>
#include <ctype.h>

//#include "config.h"
//#include "stack.h"
#include "assembler.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Errors_Processing(ARGV_ERROR);
    }

    char* inp_file_name = argv[1];
    char* output_file_name = argv[2];

    struct Commands_Info text = {};

    Read_From_File(&text, inp_file_name);

    Lines_Connecter(&text);

    struct label labels[LABELS_NUM];

    Return_Code(&text, labels, output_file_name);

    Read_From_File(&text, inp_file_name);
    Lines_Connecter(&text);
    Return_Code(&text, labels, output_file_name);
}

void Read_From_File(struct Commands_Info* text, char* inp_file_name)
{
    printf("ass1\n");
    FILE* source = NULL;

    if ((source = fopen(inp_file_name, "r")) == NULL) {
        Errors_Processing(NO_SOURCE_FILE);
    }

    struct stat file;
    stat(inp_file_name, &file);
    text->buffer = (char* )calloc(file.st_size, sizeof(char));
    text->size = fread(text->buffer, sizeof(char), file.st_size, source);

    text->lines_amount = 0;
    for (size_t i = 0; i < text->size; i++)
    {
        if (text->buffer[i] == '\n') text->lines_amount++;
    }
    text->lines_amount++;
    printf("ass2\n");
}

void Lines_Connecter(struct Commands_Info* text)
{
    printf("ass3\n");
    text->lines = (char** )calloc(text->lines_amount, sizeof(char*));
    *(text->lines) = text->buffer;

    size_t j = 1;
    for (size_t i = 1; i < text->size-1; i++)
    {
        if (text->buffer[i] == '\n')
        {
            text->buffer[i] = '\0';

            size_t p = 1;
            while (text->buffer[i+p] == ' ' || text->buffer[i+p] == '\t') p++;
            text->lines[j] = &(text->buffer[i+p]);
            j++;
        }
    }
    printf("ass4\n");
}

void Get_arg(char* Line, char* code, size_t* pointer, int name, int line)
{
    printf("Get arg scuco\n");
    Reach_Value(&Line);

    int value = 0;
    char registr[4] = {};

    if (sscanf(Line, " [%d + %s", &value, registr) == 2)     //[10 + ax]
    {
        printf("Get arg type [10 + ax]\n");
        add_comm_name(code, pointer, name, 1, 1, 1);
        add_imm_const(code, pointer, value);
        add_reg(code, pointer, registr, line);

        return;
    }

    else if (sscanf(Line, " [%d", &value) == 1)   //[10]
    {
        printf("Get arg type [10], value = %d\n", value);
        add_comm_name(code, pointer, name, 1, 0, 1);
        add_imm_const(code, pointer, value);
        return;
    }

    else if (sscanf(Line, " [%s", registr) == 1)   //[ax]
    {
        printf("Get arg type [ax]\n");
        add_comm_name(code, pointer, name, 1, 1, 0);
        add_reg(code, pointer, registr, line);

        return;
    }

    else if (sscanf(Line, "%d + %s", &value, registr) == 2)     //10 + ax
    {
        printf("Get arg type 10 + ax\n");

        if (name == cmd_pop)
        {
            Errors_Processing(POP_TO_IMM_CONST, line);
        }

        add_comm_name(code, pointer, name, 0, 1, 1);
        add_imm_const(code, pointer, value);
        add_reg(code, pointer, registr, line);

        return;
    }

    else if (sscanf(Line, "%d", &value) == 1)       //10
    {
        printf("Get arg type 10\n");

        if (name == cmd_pop)
        {
            Errors_Processing(POP_TO_IMM_CONST, line);
        }

        add_comm_name(code, pointer, name, 0, 0, 1);
        add_imm_const(code, pointer, value);

        return;
    }

    else if (sscanf(Line, "%s", registr) == 1)      //ax
    {
        printf("Get arg type ax\n");
        add_comm_name(code, pointer, name, 0, 1, 0);
        add_reg(code, pointer, registr, line);

        return;
    }

    else
    {
        Errors_Processing(UNKNOWN_ARGUMENT, line);
    }
}

void add_comm_name(char* code, size_t* pointer, int name, int have_mem, int have_reg, int have_imm)
{
    *(code + (*(pointer))) = char(name) + ((1*have_mem) << memory) + ((1 * have_reg) << reg) + ((1 * have_imm) << imm_const);
    *(pointer) += sizeof(char);
}

void add_imm_const(char* code, size_t* pointer, int value)
{
    *((int*)(code + (*(pointer)))) = value;
    *(pointer) += sizeof(int);
}

void add_reg(char* code, size_t* pointer, char* registr, int line)
{
    int reg_num = int(registr[0]-'a');

    if (reg_num > 10)
    {
        Errors_Processing(REGISTER_ERROR, line);
    }

    *((int*)(code+ (*(pointer)))) = reg_num;
    *(pointer) += sizeof(int);
}


void Return_Code(struct Commands_Info* text, struct label* labels, char* output_file_name)
{
    printf("ass 5\n");

    if (text == NULL)
    {
        Errors_Processing(ZERO_POINTER);
    }


    printf("%d", text->lines_amount);
    int code_size = (text->lines_amount)*(2*sizeof(int) + sizeof(char));
    printf("\n%d\n", code_size);

    char code[code_size] = {0};
    for (int i = 0; i < code_size; i++) code[i] = '\0';

    printf("ass 51\n");

    size_t line = 0;
    size_t pointer = 0;

    #define DEF_CMD(name, num, arg, ...)                            \
            else if (stricmp(command, #name) == 0)                  \
            {                                                       \
                if (arg)                                            \
                {                                                   \
                    char* Line = text->lines[line];                 \
                    printf("Line in push pop is %s\n", Line);       \
                    int Com_Name = cmd_##name;                      \
                    Get_arg(Line, code, &pointer, Com_Name, line);  \
                    line++;                                         \
                }                                                   \
                else                                                \
                {                                                   \
                    int Com_Name = cmd_##name;                      \
                    *(code + pointer) = char(Com_Name);             \
                    pointer += sizeof(char);                        \
                    line++;                                         \
                }                                                   \
            }

    #define DEF_JMP(c_name)                                         \
        else if (stricmp(command, #c_name) == 0)                    \
        {                                                           \
            printf("Trying to add label\n");                        \
            char* Line = text->lines[line];                         \
            Reach_Value(&Line);                                     \
                                                                    \
            char name[LABEL_NAME_LEN] = {};                         \
                                                                    \
            if (sscanf(Line, "%s", name) == 1)                      \
            {                                                       \
                printf("label has a name\n");                       \
                                                                    \
                int Com_Name = cmd_##c_name;                        \
                *(code + pointer) = char(Com_Name);                 \
                                                                    \
                pointer += sizeof(char);                            \
                                                                    \
                bool found = false;                                 \
                for (int i = 0; i < LABELS_NUM; i++)                \
                {                                                   \
                    if (stricmp(name, labels[i].name) == 0)         \
                    {                                               \
                        found = true;                               \
                        *((int*)(code+pointer)) = labels[i].line;   \
                        printf("\nI found label number %d, pointer it has %d\n\n", i, labels[i].line);\
                        break;                                      \
                    }                                               \
                }                                                   \
                                                                    \
                if (!found)                                         \
                {                                                   \
                    *((int*)(code+pointer)) = LABEL_POISON;         \
                }                                                   \
                                                                    \
                pointer += sizeof(int);                             \
            }                                                       \
                                                                    \
            else                                                    \
            {                                                       \
                Errors_Processing(JMP_ERROR, line);                 \
            }                                                       \
                                                                    \
            line++;                                                 \
        }                                                           


    while (line < text->lines_amount)
    {
        printf("%d, pointer = %d\n", line, pointer);
        char command[COMMAND_LEN] = {};
        sscanf(text->lines[line], "%s", command);

        printf("%s\n", command);

        if (command[strlen(command)-1] == ':')
        {
            int i = 0;
            while (((strcmp(labels[i].name, "") != 0) && (i < LABELS_NUM)) && strncmp(labels[i].name, command, strlen(command) - 1) != 0)
            {
                i++;
            }

            if (i >= LABELS_NUM)
            {
                Errors_Processing(MANY_LABELS, line);
            }

            else
            {
                printf("i = %d\n", i);
                int com_len = strlen(command)-1;
                for (int j = 0; j < com_len; j++)
                {
                    labels[i].name[j] = command[j];
                }

                labels[i].line = pointer;
                printf("������ ������ %d ������� %s �������� ����� %s, �������� ��������� ��� ����: %d, ������ ����: %d ����\n", i, command, labels[i].name, labels[i].line, pointer);
            }

            line++;
        }

        DEF_JMP(jmp)
        DEF_JMP(jb)
        DEF_JMP(jbe)
        DEF_JMP(ja)
        DEF_JMP(jae)
        DEF_JMP(je)
        DEF_JMP(jne)
        DEF_JMP(call)
        #include "cmd.h"

        else
        {
            printf("AIAIAIIAIAIA\n");
            line++;
        }
    }

    #undef DEF_CMD


    FILE* out_file = NULL;
    if ((out_file = fopen(output_file_name, "wb")) == NULL)
    {
        printf("can't open code file\n");
    }

    fprintf(out_file, "%s\n", SIGNATURE);
    fprintf(out_file, "%zd\n", pointer);

    for (int i = 0; i < pointer; i++)
    {
        fprintf(out_file, "%c", code[i]);
    }
    fclose(out_file);

    printf("ass6\n");

    free(text->buffer);
    free(text->lines);

}


void Reach_Value(char** ptr){
    while (((**ptr) != '\0') && ((**ptr) != ' '))
    {
        (*ptr)++;
    }
}


void Errors_Processing(int Err, int lline)
{
    if (lline != 0) printf("%s in line %d", Errors_Names[Err], lline);
    else printf("%s", Errors_Names[Err]);
    exit(Err);
}