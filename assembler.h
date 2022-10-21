#define DEF_CMD(name, num, arg, ...)\
        cmd_##name = num,  
enum Commands
{
    #include "cmd.h"
};
#undef DEF_CMD

enum shift
{
    imm_const = 5,
    reg       = 6,
    memory    = 7
};

const int LABEL_NAME_LEN = 25;
const int LABEL_POISON = -1;
struct label
{
    int line = LABEL_POISON;
    char name[LABEL_NAME_LEN] = {};
};


const char* OUTPUT_FILE = "code.txt";
const int COMMAND_LEN = 20;
const char* SIGNATURE = "ASS.300";
const int LABELS_NUM = 20;

struct Commands_Info
{
    char * buffer;
    char ** lines;
    size_t size;
    size_t lines_amount;
};


enum Errors
{
    POP_TO_IMM_CONST = 1,
    NO_SOURCE_FILE,
    REGISTER_ERROR,
    UNKNOWN_ARGUMENT,
    ZERO_POINTER,
    MANY_LABELS,
    JMP_ERROR,
    ARGV_ERROR,
};

const char* Errors_Names[] =   {"It's not an error, just let it be here",
                                "Trying to pop to immediate constant",
                                "No source file found",
                                "Cringe register name",
                                "Unknow argument",
                                "Zero_pointer",
                                "Too many labels already used",
                                "Don't understand where to jump",
                                "Write 2 arguments after the assembler.exe shows the input and output files names"
                                };

void Read_From_File(struct Commands_Info* text, char* inp_file_name);
void Lines_Connecter(struct Commands_Info* text);
void Get_arg(char* Line, char* code, size_t* pointer, int name, int line);
void add_comm_name(char* code, size_t* pointer, int name, int have_mem, int have_reg, int have_imm);
void add_imm_const(char* code, size_t* pointer, int value);
void add_reg(char* code, size_t* pointer, char* registr, int line);
void Return_Code(struct Commands_Info* text, struct label* labels, char* output_file_name);
void Reach_Value(char** ptr);
void Errors_Processing(int Err, int lline = 0);