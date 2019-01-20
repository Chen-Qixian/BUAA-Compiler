#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace compiler{

// ȫ�ֺ궨��
#define TABMAX 16*1024
#define TABIDXMAX 1024
#define MIDCODEMAX 16*1024
#define MIPSCODEMAX 16*1024
#define SMAX 1024
#define PARAMAX 128 // ���������������ֵ
#define FUNCMAX 128 // �����������ֵ
#define VARMAX  128 // ���ʽ�б����������ֵ
// ö�����Ͷ���
// �������Ͷ���
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
// �м���������
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
    ret, //11     ret Ϊ�������� return (x);  move $v0 ...
    RET, //12     RET Ϊ�з���ֵ�ĺ������շ���ֵ move ... $v0
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

// mips�еĲ�����
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
// ��ʶ�����ࣺ���������������� , ���� , ����
enum objtyp{constobj , varobj , funcobj , arrobj , paraobj};
// ��ʶ�����ͣ�int , char , void
enum identyp{intiden , chariden , voididen , notdef}; // notdef Ϊ����ʱ����ű�δ���֣�δ�������
// �м�������������
enum midoptyp{imm_midop , ret_midop , tmpvar_midop , var_midop , arr_midop , err_midop , global_var_midop , global_arr_midop};

// ȫ�ֱ�������
extern std::string sym_type[38]; // ��������
extern std::string obj_type[5]; // ��ʶ������
extern std::string iden_type[3]; // ��ʶ����������
extern std::string opration_type[30]; // �м�������������
extern std::string mips_op[30]; // mips��������
extern std::string code;  // ������Դ����
extern char id[128];    // ��ǰ�Ӵʷ����������ı�ʶ��
extern symbol sy;         // ��ǰ�Ӵʷ����������ķ�������
extern char c;     // ������ַ�
extern int num;    // �ʷ���������������
extern int cptr;   // code pointer
extern int tptr;   // token pointer
extern int linenum; // �����к�
extern int wordnum; // ���ʸ������
extern bool end_of_file;
extern int tabidx; // ���ű�����ֵ
extern int tabidxidx; // ���ű�ֶα�����
extern int global_end; // ȫ�ֱ����������
extern int midcodeidx;
extern int tmp_var; // �м������ʱ������š�
extern int if_num;   // if ������
extern int while_num; // while������
extern int switch_num; // switch������
extern int case_num;  // case������ , ��ÿ��switch��������1.
extern int str_idx;  // string ������ֵ
extern int str_num;  // ������mips���ַ�����ǩ�ı�ţ�ÿ������������ַ�����++
extern int mipsidx;  // mips������
extern bool has_error;


// �ļ�����
extern std::ofstream lexical_analysis_result;
extern std::ofstream mid_code_result;
extern std::ofstream mips_code_result;

// ����
struct tab_entry{ // ���ű���
    std::string name;
    objtyp obj;      // ��������
    identyp type;    // ������������ �� ������Ϊ�䷵��ֵ����
    int len;         // ��Ϊ����������������Ϊ0������Ϊ���鳤��
    int adr;         // ������intΪ����ֵ��charΪ��asciiֵ������Ϊ���ƫ������������Ϊ����������ʱ�����ĸ���
};

struct mid_code{ // �м������
    opration op;     // ��������
    std::string z;   // �м���
    std::string x;   // ��������
    std::string y;   // ��������
};

struct mips_code{  // mips������
    mipsop op;
    std::string z;
    std::string x;
    std::string y;
    int imm;
};

extern std::string string_tab[SMAX]; // �ַ�����
extern tab_entry tab[TABMAX];
extern int tab_idx[TABIDXMAX];  // ���ű�ֶα� ���ڴ���ű����ͷ����ֵ��
extern mid_code mid_code_tab[MIDCODEMAX]; // �м�����
extern mips_code mips_code_tab[MIPSCODEMAX]; // mips�����

}

#endif // _PARAMETERS_H_
