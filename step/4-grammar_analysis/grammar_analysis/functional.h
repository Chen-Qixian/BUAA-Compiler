#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

#include "lexical.h"

namespace compiler{
    void skip(symbol symset[] , int len);
    bool find_sym(symbol symset[] , int len);
    identyp sym2type(symbol sy);
    identyp find_func(std::string idname);
    std::string int2str(int n);
    int str2int(std::string str);
    bool str_is_num(std::string s);
    std::string type2str(identyp type);  // 类型转字符串，登录中间代码表使用
    opration getNegRelation(symbol sym); // 获取操作类型（关系运算）在条件表达式中使用
    std::string create_if_else_label();   // 产生if语句的else标签
    std::string create_while_beg();   // 产生while语句的beg标签
    std::string create_while_end();   // 产生while语句的end标签
    std::string create_case_end_label();
    std::string create_switch_end_label();
    std::string str_modify(std::string str);  // 对生成mips代码中的字符串进行修饰，转义其中的\n并为每一个字符串的结尾增加换行
}

#endif // _FUNCTIONAL_H_
