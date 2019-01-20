
// ���һЩ��¼���ű� �Լ���ͬ�Ĳ��ʽ �Լ�����Ϣ�����
#include "enter.h"

namespace compiler {
    // ��¼���ű�
    void enter_tab(std::string idname , objtyp object , identyp idtype , int length , int adrr){
        if(tabidx == TABMAX){
            fatal(0); // ���ű����
        }
        if(object != funcobj && lookup_field(idname) != -1){
            error(40); // ���������ض������
        }
        else if(object == funcobj && (lookup_global(idname) != -1 || lookup_func(idname) != -1)){
            error(45); //�����ض������
        }
        else{ // �����ǰ��������δ�ҵ��������壬���
            tab[tabidx].name = idname;
            tab[tabidx].obj = object;
            tab[tabidx].type = idtype;
            tab[tabidx].len = length;
            tab[tabidx].adr = adrr;
            tabidx ++;    // ����������
            if(tabidxidx == 0){
                global_end ++; // ���������㣨ȫ�ֶ��壩      // �ǲ��ǿ�����tab_idx[1]��ֵ�������أ�
            }
        }
    }
    //��¼�м��������
    void enter_mid_code(std::string z , std::string x , opration op , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // �м��������
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        print_mid_code();
        midcodeidx ++;
    }
    // ����¼�м������������������ʽ�м���룩
    void enter_mid_code_only(std::string z , std::string x , opration op , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // �м��������
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        midcodeidx ++;
    }
    // ��¼�м���������ָ����
    void enter_mid_code_condition(opration op , std::string z , std::string x , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // �м��������
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        print_mid_code_condition();
        midcodeidx ++;
    }
    // ���²�����ҵ��ı����ڷ��ű��е�����ֵtabidx,δ�ҵ�����-1
    // ����ű��е�ǰ��                                                 ////////////////ע�����ֻ�������м�������ɽ׶�
    int lookup_field(std::string name){
        for(int i = tab_idx[tabidxidx] ; i < tabidx ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // ���ҷ��ű���ȫ����
    int lookup_global(std::string name){
        for(int i = 0 ; i < global_end ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // ���ҷ��ű������к���
    int lookup_func(std::string name){
        for(int i = 1 ; i <= tabidxidx ; i ++){    // ע�������ֹ����Ϊtabidxidx����
            if(tab[tab_idx[i]].name == name){
                return tab_idx[i];
            }
        }
        return -1;
    }
    // ���ұ���/�����ڷ��ű��еĴ�����
    int lookup_var(std::string name){
        int field_idx = lookup_field(name);
        if(field_idx == -1){
            return lookup_global(name);
        }
        else{
            return field_idx;
        }
    }
    // ���ұ����ڵ�ǰ���еĴ�����               ////// ��֮ǰ�ĺ������֣�����ʵ��Ŀ��������ɽ׶�ʹ��
    int lookup_cur_field(std::string name , int startidx){
        for(int i = tab_idx[startidx] ; i < tab_idx[startidx + 1] ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // ���Һ����ں����������е��±�
    int lookup_func_idx(std::string name){
        for(int i = 1 ; i < tabidxidx ; i ++){    // �����tabidxidx = 1��ʼ�飨����鲻������Ϊ������main��
            if(tab[tab_idx[i]].name == name){     // ��Ϊtabidxidx = 0 ��global��
                return i;
            }
        }
        return -1;
    }

    std::ofstream tab_result;
    void print_table(){
        tab_result.open("16061160_check_tab.txt");
        tab_result << "name  obj  type  len  adr \n";
        for(int i = 0 ; i < tabidx ; i ++){
            tab_result  << tab[i].name << '\t'
                    << tab[i].obj  << '\t'
                    << tab[i].type << '\t'
                    << tab[i].len  << '\t'
                    << tab[i].adr  << '\t' << std::endl;
        }
        for(int i = 0 ; i < tabidxidx + 1; i ++){        // ���������������Ϣ����һ��Ϊglobalͷ�����һ��Ϊ���ű��β
            tab_result << tab_idx[i] << ' ';
        }
        tab_result << std::endl;
        for(int i = 0 ; i < str_idx ; i ++){
            tab_result << string_tab[i] << ' ';
        }
    }

    void print_mid_code(){  // ���ʽ�м�������
//            std::cout << mid_code_tab[midcodeidx].z << " = "
//            << mid_code_tab[midcodeidx].x << ' '
//            << opration_type[mid_code_tab[midcodeidx].op] << ' '
//            << mid_code_tab[midcodeidx].y << std::endl ;
            mid_code_result << mid_code_tab[midcodeidx].z << " = "
            << mid_code_tab[midcodeidx].x << ' '
            << opration_type[mid_code_tab[midcodeidx].op] << ' '
            << mid_code_tab[midcodeidx].y << std::endl ;

    }

    void print_const_mid(){ // �����м�������
//        std::cout << opration_type[mid_code_tab[midcodeidx - 1].op] <<' ' << mid_code_tab[midcodeidx - 1].z
//             << ' ' << mid_code_tab[midcodeidx - 1].x << " = " << mid_code_tab[midcodeidx - 1].y << std::endl;
        mid_code_result << opration_type[mid_code_tab[midcodeidx - 1].op] <<' ' << mid_code_tab[midcodeidx - 1].z <<\
         ' ' << mid_code_tab[midcodeidx - 1].x << " = " << mid_code_tab[midcodeidx - 1].y << std::endl;
    }

    void print_var_mid(){ //�����м�������
//        std::cout << opration_type[mid_code_tab[midcodeidx - 1].op] << ' ' << mid_code_tab[midcodeidx - 1].z <<
//           ' ' <<  mid_code_tab[midcodeidx - 1].x << std::endl;
        mid_code_result << opration_type[mid_code_tab[midcodeidx - 1].op] << ' ' << mid_code_tab[midcodeidx - 1].z <<\
            ' ' <<  mid_code_tab[midcodeidx - 1].x << std::endl;
    }

    void print_mid_code_condition(){ //��ת�����м�������
//        std::cout << opration_type[mid_code_tab[midcodeidx].op] << ' '
//            << mid_code_tab[midcodeidx].x << ' '
//            << mid_code_tab[midcodeidx].y << ' '
//            << mid_code_tab[midcodeidx].z << std::endl ;
        mid_code_result << opration_type[mid_code_tab[midcodeidx].op] << ' '
            << mid_code_tab[midcodeidx].x << ' '
            << mid_code_tab[midcodeidx].y << ' '
            << mid_code_tab[midcodeidx].z << std::endl ;
    }

    void mid_code_output_check(){
        std::ofstream mid_code_check;
        mid_code_check.open("16061160_check_mid_code.txt");
        for(int i = 0 ; i < midcodeidx ; i ++){
            mid_code_check << mid_code_tab[i].z << " = "
            << mid_code_tab[i].x << ' '
            << opration_type[mid_code_tab[i].op] << ' '
            << mid_code_tab[i].y << std::endl ;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // ���¿�ʼ���м������Ŀ�����ת������
    /////////////////////////////////////////////////////////////////////////////////////////////

    void enter_mips_0(std::string labels){  // û�в����� .data /.text /label: /syscall
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].z = labels; // ��ǩ����¼��z ��
        mipsidx ++;
        mips_code_result << labels << std::endl;
    }

    void enter_mips_1(std::string z , std::string x , std::string y){  // �����ַ��� str_0: .asciiz "haha"
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].z = z;
        mips_code_tab[mipsidx].x = x;
        mips_code_tab[mipsidx].y = y;
        mipsidx ++;
        mips_code_result << z << ' ' << x << ' ' << y << std::endl;
    }

    void enter_mips_2(mipsop op , std::string z , int imm){ // li $v0 4 ����ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].imm= imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << imm << std::endl;
    }

    void enter_mips_3(mipsop op , std::string z , std::string x){ // la $a0 str_0  / move $fp $sp ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << std::endl;
    }

    void enter_mips_4(mipsop op , std::string z , std::string x , int imm){  // subi $sp $sp 4 ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].imm  = imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << ' ' << imm << std::endl;
    }

    void enter_mips_5(mipsop op , std::string z , std::string x , int imm){ // sw $t0 0($sp) ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].imm  = imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << imm << '(' << x << ")\n";
    }

    void enter_mips_6(mipsop op , std::string z){  // jal function ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << std::endl;
    }

    void enter_mips_7(mipsop op , std::string z , std::string x , std::string y){  // add $t2 $t0 $t1 ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].y  = y;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << ' ' << y << std::endl;
    }

    void enter_mips_8(mipsop op , std::string x , std::string y , std::string labels){ //beq $t0 $t1 label ��ָ��
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // Ŀ���������
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = labels;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].y  = y;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << x << ' ' << y << ' ' << labels << std::endl;
    }
}

