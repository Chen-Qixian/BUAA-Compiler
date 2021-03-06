#include "error.h"

namespace compiler{
std::ofstream grammar_err_file;
std::string errormsg[70] = {
    "缺少类型定义 int 或 char.", // 0
    "语句结尾缺少分号", // 1  在变量声明时直接赋值也会报告此错误
    "标识符缺失", //2
    "缺少赋值符号", //3
    "缺少常数数值", //4
    "声明类型与变量类型不匹配", //5
    "常量定义不完善", //6
    "常量定义必须位于变量定义与函数定义之前", //7
    "变量定义识别到非法符号", //8
    "变量类型不能为空", //9
    "数组长度定义必须为整数", //10
    "缺少右中括号", //11
    "缺少逗号或分号", //12
    "变量定义必须位于函数定义之前", //13
    "主函数返回值类型必须为void", // 14
    "缺少右小括号", //15
    "缺少左大括号", //16
    "未识别到正确的语句列", //17
    "未识别到正确的语句结尾，分号或右大括号缺失", //18
    "缺少左小括号", //19
    "条件缺失", //20
    "未识别到正确的语句头", //21
    "未识别到正确的表达式头符号", // 22
    "表达式结构错误", //23
    "项结构错误", //24
    "缺少右中括号", //25  凉了跟11重了！！！！！
    "缺少整数值", //26
    "因子结构错误", //27
    "写语句内容错误", //28
    "返回值表达式错误", //29
    "赋值语句缺少赋值符号", // 30
    "常量值错误", //31
    "缺少冒号", //32
    "缺少右大括号", //33
    "缺少case关键字", //34
    "词法分析读到非法字符", //35
    "字符串不以“结尾", //36
    "整数以前导零开始", //37
    "字符类型出现非法字符", //38
    "字符类型不以'结尾", //39
    "变量常量重定义错误", //40
    "函数未定义", //41
    "函数返回值类型错误", // 42
    "数组下标不能为字符", //43
    "变量未定义", //44
    "函数重定义错误", //45
    "数组越界", //46
    "传入参数过多", //47
    "参数类型不匹配", //48
    "传入参数过少", //49
    "该变量不是数组类型", // 50
    "单独出现的数组名错误", //51
    "不能为常量赋值", //52
    "赋值类型不正确", //53
    "条件表达式参数类型错误", //54
    "常量类型错误", //55
    "不正确的文件结尾", //56
    "!后缺少=", //57
};

std::string fatalmsg[10] = {
    "符号表溢出", //0
    "中间代码表溢出", //1
    "目标代码表溢出", //2
    "读取源代码文件异常" //3
};

void error(int errnum){
    if(errnum == 1 || errnum == 12){
        std::cout << "Error: " << errormsg[errnum] << " at line: " << linenum - 1 << std::endl;
        grammar_err_file << "Error: " << errormsg[errnum] << " at line: " << linenum - 1 << std::endl;
    }
    else{
        std::cout << "Error: " << errormsg[errnum] << " at line: " << linenum << std::endl;
        grammar_err_file << "Error: " << errormsg[errnum] << " at line: " << linenum  << std::endl;
    }
    has_error = true;
}

void fatal(int fatalno){
    std::cout << "Fatal: " << fatalmsg[fatalno] << std::endl;
    std::exit(1);
}

void err_open(){
    grammar_err_file.open("16061160_errors.txt");
}
}
