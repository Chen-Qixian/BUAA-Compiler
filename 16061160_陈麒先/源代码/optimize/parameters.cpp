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
    // 全局变量定义
    std::string code; //待编译源代码
    char id[128];
    std::string token; // 词法分析读到的符号
    symbol sy;      // 当前符号的种类
    int num = 0;    // 词法分析读出的数字
    int cptr = 0;   // code pointer
    int tptr = 0;   // token pointer  token is id
    char c   = ' '; // 预读字符
    int linenum = 1;// 代码行号
    int wordnum = 1;// 单词数目（顺序编号）
    bool end_of_file = false;
    int tabidx = 0; // 符号表索引值
    int tabidxidx = 0; // 符号表分段表索引
    int global_end = 0; // 全局变量定义结束
    int midcodeidx = 0; // 中间代码表索引
    int tmp_var = 0; // 中间代码临时变量编号。
    int if_num = 1;  // if 语句计数
    int while_num = 1; // while语句计数
    int switch_num = 0; // switch语句计数
    int case_num = 1;  // case语句计数 , 在每个switch结束后置1.
    int str_idx = 0;  // string 表索引值
    int str_num = 0;  // mips生成的string计数
    int mipsidx = 0; // mips表索引
    bool has_error = false; // 编译过程是否出错的判断
    // 全局文件定义
    std::ofstream lexical_analysis_result;
    std::ofstream mid_code_result;
    std::ofstream mips_code_result;

    //表定义
    std::string string_tab[SMAX]; // 字符串表
    tab_entry tab[TABMAX];
    int tab_idx[TABIDXMAX] = {0};
    mid_code mid_code_tab[MIDCODEMAX];
    mips_code mips_code_tab[MIPSCODEMAX];
}
