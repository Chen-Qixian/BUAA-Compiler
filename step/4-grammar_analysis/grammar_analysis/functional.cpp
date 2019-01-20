// ��¼һЩgrammar�е��õ��ĺ���

#include "functional.h"

namespace compiler{

// ��symset��Ѱ�ҷ���sy�������򷵻�true;
bool find_sym(symbol symset[] , int len){
    for(int i = 0 ; i < len ; i ++){
        if(sy == symset[i]) return true;
    }
    return false;
}

// ����������ֱ��sy����symsetֹͣ����������һ�������Ŀ�ʼ�׶�Ѱ�ҿ�ʼ���ţ���������������ҵ���һ��ȷ����
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

identyp find_func(std::string idname){                // ������ࣺ��enter.cpp : lookup_func�ظ���
    // ���������Ҫ�Ǻ���������Ϊ������ʱʹ�õġ�
    for(int i = 0 ; i <= tabidxidx ; i ++){           // bug fix : i <= tabidxidx Ϊ�ݹ���ò��ұ���
        if(tab[tab_idx[i]].name == idname){           //���з���ֵ����������Ϊ���Ӳ�������������
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

std::string type2str(identyp type){  // ����ת�ַ�������¼�м�����ʹ��
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
    return j;  // ������㷵�ص�
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

std::string str_modify(std::string str){  // ������mips�����е��ַ����������Σ�ת�����е�\n��Ϊÿһ���ַ����Ľ�β���ӻ���
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














