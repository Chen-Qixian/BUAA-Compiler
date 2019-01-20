#include "parameters.h"

namespace compiler{
    std::string sym_type[38] = {   "UINT"  ,"IDEN"  ,"PLUS"  ,"MINUS" ,"MULT"  ,"DIV"   ,\
                                   "LESS"  ,"LEQ"   ,"GREAT" ,"GEQ"   ,"EQUAL" ,\
                                   "NEQ"   ,"ASSIGN","CHAR"  ,"STRING","COMMA" ,\
                                   "SEMI"  ,"COLON" ,"LPAR"  ,"RPAR"  ,"LZKH"  ,\
                                   "RZKH"  ,"LDKH"  ,"RDKH"  ,"CONST" ,"INT"   ,\
                                   "CHAR"  ,"VOID"  ,"MAIN"  ,"IF"    ,"SWITCH",\
                                   "CASE"  ,"DEFAUL","SCANF" ,"PRINTF","RETURN","WHILE"};
    std::string obj_type[5] = {"const" , "var" , "func" , "array" , "para"};
    std::string iden_type[3] = {"int" , "char" , "void"};
    std::string opration_type[30] = {"+"     , "-"    , "*"        , "/"    , "const" ,\
                                     "array" , "var"  , "function" , "para" , "push"  ,\
                                     "call"  , "ret"  , ""         , "beq"  , "bne"   ,\
                                     "bgt"   , "bge"  , "blt"      , "ble"  , "j"     ,\
                                     "read"  , "write", "beqz"     , "funcend", ""    ,\
                                     ""      , "exit" };
    std::string mips_op[30] = { "li" ,"syscall", "sw" , "la" , "move" , "lw"  , "subi" ,\
                                "jal" , "jr" , "addi" ,"addu", "sub"  , "mul" , "div"  ,\
                                "beq" , "bne", "bgt"  ,"bge" , "blt"  , "ble" , "j"    ,\
                                "beqz"};
    // ȫ�ֱ�������
    std::string code; //������Դ����
    char id[128];
    std::string token; // �ʷ����������ķ���
    symbol sy;      // ��ǰ���ŵ�����
    int num = 0;    // �ʷ���������������
    int cptr = 0;   // code pointer
    int tptr = 0;   // token pointer  token is id
    char c   = ' '; // Ԥ���ַ�
    int linenum = 1;// �����к�
    int wordnum = 1;// ������Ŀ��˳���ţ�
    bool end_of_file = false;
    int tabidx = 0; // ���ű�����ֵ
    int tabidxidx = 0; // ���ű�ֶα�����
    int global_end = 0; // ȫ�ֱ����������
    int midcodeidx = 0; // �м���������
    int tmp_var = 0; // �м������ʱ������š�
    int if_num = 1;  // if ������
    int while_num = 1; // while������
    int switch_num = 0; // switch������
    int case_num = 1;  // case������ , ��ÿ��switch��������1.
    int str_idx = 0;  // string ������ֵ
    int str_num = 0;  // mips���ɵ�string����
    int mipsidx = 0; // mips������
    bool has_error = false; // ��������Ƿ������ж�
    // ȫ���ļ�����
    std::ofstream lexical_analysis_result;
    std::ofstream mid_code_result;
    std::ofstream mips_code_result;

    //����
    std::string string_tab[SMAX]; // �ַ�����
    tab_entry tab[TABMAX];
    int tab_idx[TABIDXMAX] = {0};
    mid_code mid_code_tab[MIDCODEMAX];
    mips_code mips_code_tab[MIPSCODEMAX];
}
