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
    std::string type2str(identyp type);  // ����ת�ַ�������¼�м�����ʹ��
    opration getNegRelation(symbol sym); // ��ȡ�������ͣ���ϵ���㣩���������ʽ��ʹ��
    std::string create_if_else_label();   // ����if����else��ǩ
    std::string create_while_beg();   // ����while����beg��ǩ
    std::string create_while_end();   // ����while����end��ǩ
    std::string create_case_end_label();
    std::string create_switch_end_label();
    std::string str_modify(std::string str);  // ������mips�����е��ַ����������Σ�ת�����е�\n��Ϊÿһ���ַ����Ľ�β���ӻ���
}

#endif // _FUNCTIONAL_H_
