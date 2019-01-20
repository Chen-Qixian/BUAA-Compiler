#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace compiler{

// 全局宏定义
#define TABMAX 16*1024
#define TABIDXMAX 1024
#define MIDCODEMAX 16*1024
#define MIPSCODEMAX 16*1024
#define SMAX 1024
#define PARAMAX 128 // 函数参数传递最大值
#define FUNCMAX 128 // 函数个数最大值
#define VARMAX  128 // 表达式中变量个数最大值
// 枚举类型定义
// 符号类型定义
enum symbol{
    UINT,
    IDEN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    LESS,
    LEQ,
    GREAT,
    GEQ,
    EQUAL,
    NEQ,
    ASSIGN,
    CHAR,   // a character value
    STRING,
    COMMA,
    SEMI,
    COLON,
    LPAR,
    RPAR,
    LZKH,
    RZKH,
    LDKH,
    RDKH,
    CONST,
    INT,
    CHARKW,  // keyword char
    VOID,
    MAIN,
    IF,
    SWITCH,
    CASE,
    DEFAUL,
    SCANF,
    PRINTF,
    RETURN,
    WHILE
};
// 中间代码操作符
enum opration{
    add, //0
    sub, //1
    mult, //2
    div, //3
    con, //4
    arr, //5
    var, //6
    func,//7
    para,//8
    push,//9
    call,//10
    ret, //11     ret 为函数返回 return (x);  move $v0 ...
    RET, //12     RET 为有返回值的函数接收返回值 move ... $v0
    beq, //13
    bne, //14
    bgt, //15
    bge, //16
    blt, //17
    ble, //18
    j  , //19
    read,//20
    write,//21
    beqz, //22
    funcend, //23
    assignop,//24
    label, //25
    exitprog //26
};

// mips中的操作符
enum mipsop{
    mli, //0
    msyscall, //1
    msw, //2
    mla, //3
    mmove, //4
    mlw, //5
    msubi, //6
    mjal, //7
    mjr , //8
    maddi,//9
    madd, //10
    msub, //11
    mmul, //12
    mdiv, //13
    mbeq, //14
    mbne, //15
    mbgt, //16
    mbge, //17
    mblt, //18
    mble, //19
    mj,   //20
    mbeqz,//21
};
// 标识符种类：常量，变量，函数 , 数组 , 参数
enum objtyp{constobj , varobj , funcobj , arrobj , paraobj};
// 标识符类型：int , char , void
enum identyp{intiden , chariden , voididen , notdef}; // notdef 为引用时查符号表未发现（未定义错误）
// 中间代码操作符种类
enum midoptyp{imm_midop , ret_midop , tmpvar_midop , var_midop , arr_midop , err_midop , global_var_midop , global_arr_midop};

// 全局变量声明
extern std::string sym_type[38]; // 符号类型
extern std::string obj_type[5]; // 标识符种类
extern std::string iden_type[3]; // 标识符数据类型
extern std::string opration_type[30]; // 中间代码操作符类型
extern std::string mips_op[30]; // mips操作类型
extern std::string code;  // 待编译源代码
extern char id[128];    // 当前从词法分析读出的标识符
extern symbol sy;         // 当前从词法分析读出的符号种类
extern char c;     // 读入的字符
extern int num;    // 词法分析读出的数字
extern int cptr;   // code pointer
extern int tptr;   // token pointer
extern int linenum; // 代码行号
extern int wordnum; // 单词个数编号
extern bool end_of_file;
extern int tabidx; // 符号表索引值
extern int tabidxidx; // 符号表分段表索引
extern int global_end; // 全局变量定义结束
extern int midcodeidx;
extern int tmp_var; // 中间代码临时变量编号。
extern int if_num;   // if 语句计数
extern int while_num; // while语句计数
extern int switch_num; // switch语句计数
extern int case_num;  // case语句计数 , 在每个switch结束后置1.
extern int str_idx;  // string 表索引值
extern int str_num;  // 在生成mips中字符串标签的编号，每次输出遇到该字符串后++
extern int mipsidx;  // mips表索引
extern bool has_error;


// 文件定义
extern std::ofstream lexical_analysis_result;
extern std::ofstream mid_code_result;
extern std::ofstream mips_code_result;

// 表定义
struct tab_entry{ // 符号表项
    std::string name;
    objtyp obj;      // 符号种类
    identyp type;    // 符号数据类型 ， 函数则为其返回值类型
    int len;         // 若为常量，变量，函数为0；数组为数组长度
    int adr;         // 常量：int为其数值，char为其ascii值，变量为相对偏移量，函数则为本函数内临时变量的个数
};

struct mid_code{ // 中间代码项
    opration op;     // 操作类型
    std::string z;   // 中间结果
    std::string x;   // 操作数左
    std::string y;   // 操作数右
};

struct mips_code{  // mips代码项
    mipsop op;
    std::string z;
    std::string x;
    std::string y;
    int imm;
};

extern std::string string_tab[SMAX]; // 字符串表
extern tab_entry tab[TABMAX];
extern int tab_idx[TABIDXMAX];  // 符号表分段表： 用于存符号表各段头索引值。
extern mid_code mid_code_tab[MIDCODEMAX]; // 中间代码表
extern mips_code mips_code_tab[MIPSCODEMAX]; // mips代码表

}

#endif // _PARAMETERS_H_
