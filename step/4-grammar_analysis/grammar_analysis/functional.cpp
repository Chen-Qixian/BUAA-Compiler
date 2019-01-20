// 记录一些grammar中调用到的函数

#include "functional.h"

namespace compiler{

// 从symset中寻找符号sy，存在则返回true;
bool find_sym(symbol symset[] , int len){
    for(int i = 0 ; i < len ; i ++){
        if(sy == symset[i]) return true;
    }
    return false;
}

// 跳读函数，直到sy属于symset停止跳读。用于一个程序块的开始阶段寻找开始符号，或错误发生后用于找到下一正确符号
void skip(symbol symset[] , int len){
    while(!find_sym(symset , len) && !end_of_file) getsym();
}

identyp sym2type(symbol sy){
    if(sy == INT){
        return intiden;
    }
    else if(sy == CHARKW){
        return chariden;
    }
    else{
        return voididen;
    }
}

identyp find_func(std::string idname){                // 设计冗余：与enter.cpp : lookup_func重复。
    // 这个函数主要是函数调用作为语句出现时使用的。
    for(int i = 0 ; i <= tabidxidx ; i ++){           // bug fix : i <= tabidxidx 为递归调用查找本身。
        if(tab[tab_idx[i]].name == idname){           //而有返回值函数调用作为因子不用这个函数查表
            return tab[tab_idx[i]].type;
        }
    }
    return notdef;
}

std::string int2str(int n){
    std::string str;
    std::stringstream ss;
    ss << n;
    ss >> str;
    return str;
}

int str2int(std::string str){
    int i = 0;
    std::stringstream ss;
    ss << str;
    ss >> i;
    return i;
}

bool str_is_num(std::string s){
    std::stringstream sin(s);
    double d;
    char c;
    if (!(sin >> d)) {
        return false;
    }
    if (sin >> c)
    {
        return false;
    }
    return true;
}

std::string type2str(identyp type){  // 类型转字符串，登录中间代码表使用
    if(type == intiden){
        return "int";
    }
    else if(type == chariden){
        return "char";
    }
    else if(type == voididen){
        return "void";
    }
    else{
        return "";
    }
}

opration getNegRelation(symbol sym){
    switch (sym){
        case EQUAL: return bne;
        case NEQ  : return beq;
        case GREAT: return ble;
        case GEQ  : return blt;
        case LESS : return bge;
        case LEQ  : return bgt;
        default :break;
    }
    return j;  // 这是随便返回的
}

std::string create_if_else_label(){
    std::string label;
    label = "if_" + int2str(if_num) + "_else";
    if_num ++;
    return label;
}

std::string create_while_beg(){
    std::string label;
    label = "while_" + int2str(while_num) + "_beg";
    return label;
}

std::string create_while_end(){
    std::string label;
    label = "while_" + int2str(while_num) + "_end";
    while_num ++;
    return label;
}

std::string create_case_end_label(){
    std::string label;
    label = "case_" + int2str(switch_num) + "_" + int2str(case_num) + "_end";
    case_num ++;
    return label;
}

std::string create_switch_end_label(){
    std::string label;
    label = "case_" + int2str(switch_num + 1) + "_end";
    case_num = 1;
    switch_num ++;
    return label;
}

std::string str_modify(std::string str){  // 对生成mips代码中的字符串进行修饰，转义其中的\n并为每一个字符串的结尾增加换行
    for(unsigned int i = 0 ; i < str.length() ; i ++){
        if(str[i] == '\\' && i + 1 < str.length() && str[i + 1] == 'n'){
            str = str.replace(i , 1 , "\\\\");
            i ++;
        }
    }
    str = str.insert(str.length() - 1 ,"\\n");
    return str;
}
}














