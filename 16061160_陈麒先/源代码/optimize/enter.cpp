
// 设计一些登录符号表 以及不同的查表方式 以及表信息的输出
#include "enter.h"

namespace compiler {
    // 登录符号表
    void enter_tab(std::string idname , objtyp object , identyp idtype , int length , int adrr){
        if(tabidx == TABMAX){
            fatal(0); // 符号表溢出
        }
        if(object != funcobj && lookup_field(idname) != -1){
            error(40); // 常量变量重定义错误
        }
        else if(object == funcobj && (lookup_global(idname) != -1 || lookup_func(idname) != -1)){
            error(45); //函数重定义错误
        }
        else{ // 如果当前作用域内未找到重名定义，填表
            tab[tabidx].name = idname;
            tab[tabidx].obj = object;
            tab[tabidx].type = idtype;
            tab[tabidx].len = length;
            tab[tabidx].adr = adrr;
            tabidx ++;    // 表索引后移
            if(tabidxidx == 0){
                global_end ++; // 如果是最外层（全局定义）      // 是不是可以用tab_idx[1]的值来代替呢？
            }
        }
    }
    //登录中间代码表并输出
    void enter_mid_code(std::string z , std::string x , opration op , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // 中间代码表溢出
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        print_mid_code();
        midcodeidx ++;
    }
    // 仅登录中间代码表不输出（针对特殊格式中间代码）
    void enter_mid_code_only(std::string z , std::string x , opration op , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // 中间代码表溢出
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        midcodeidx ++;
    }
    // 登录中间代码的条件指令并输出
    void enter_mid_code_condition(opration op , std::string z , std::string x , std::string y){
        if(midcodeidx == MIDCODEMAX){
            fatal(1); // 中间代码表溢出
        }
        mid_code_tab[midcodeidx].z = z;
        mid_code_tab[midcodeidx].x = x;
        mid_code_tab[midcodeidx].op = op;
        mid_code_tab[midcodeidx].y = y;
        print_mid_code_condition();
        midcodeidx ++;
    }
    // 以下查表返回找到的变量在符号表中的索引值tabidx,未找到返回-1
    // 查符号表中当前域                                                 ////////////////注意这个只能用于中间代码生成阶段
    int lookup_field(std::string name){
        for(int i = tab_idx[tabidxidx] ; i < tabidx ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // 查找符号表中全局量
    int lookup_global(std::string name){
        for(int i = 0 ; i < global_end ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // 查找符号表中所有函数
    int lookup_func(std::string name){
        for(int i = 1 ; i <= tabidxidx ; i ++){    // 注意查表的终止条件为tabidxidx本身
            if(tab[tab_idx[i]].name == name){
                return tab_idx[i];
            }
        }
        return -1;
    }
    // 查找变量/数组在符号表中的存在性
    int lookup_var(std::string name){
        int field_idx = lookup_field(name);
        if(field_idx == -1){
            return lookup_global(name);
        }
        else{
            return field_idx;
        }
    }
    // 查找变量在当前域中的存在性               ////// 与之前的函数区分！这里实在目标代码生成阶段使用
    int lookup_cur_field(std::string name , int startidx){
        for(int i = tab_idx[startidx] ; i < tab_idx[startidx + 1] ; i ++){
            if(tab[i].name == name){
                return i;
            }
        }
        return -1;
    }
    // 查找函数在函数索引表中的下标
    int lookup_func_idx(std::string name){
        for(int i = 1 ; i < tabidxidx ; i ++){    // 必须从tabidxidx = 1开始查（不会查不到，因为必须有main）
            if(tab[tab_idx[i]].name == name){     // 因为tabidxidx = 0 是global区
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
        for(int i = 0 ; i < tabidxidx + 1; i ++){        // 输出函数索引表信息，第一项为global头，最后一项为符号表结尾
            tab_result << tab_idx[i] << ' ';
        }
        tab_result << std::endl;
        for(int i = 0 ; i < str_idx ; i ++){
            tab_result << string_tab[i] << ' ';
        }
    }

    void print_mid_code(){  // 表达式中间代码输出
//            std::cout << mid_code_tab[midcodeidx].z << " = "
//            << mid_code_tab[midcodeidx].x << ' '
//            << opration_type[mid_code_tab[midcodeidx].op] << ' '
//            << mid_code_tab[midcodeidx].y << std::endl ;
            mid_code_result << mid_code_tab[midcodeidx].z << " = "
            << mid_code_tab[midcodeidx].x << ' '
            << opration_type[mid_code_tab[midcodeidx].op] << ' '
            << mid_code_tab[midcodeidx].y << std::endl ;

    }

    void print_const_mid(){ // 常数中间代码输出
//        std::cout << opration_type[mid_code_tab[midcodeidx - 1].op] <<' ' << mid_code_tab[midcodeidx - 1].z
//             << ' ' << mid_code_tab[midcodeidx - 1].x << " = " << mid_code_tab[midcodeidx - 1].y << std::endl;
        mid_code_result << opration_type[mid_code_tab[midcodeidx - 1].op] <<' ' << mid_code_tab[midcodeidx - 1].z <<\
         ' ' << mid_code_tab[midcodeidx - 1].x << " = " << mid_code_tab[midcodeidx - 1].y << std::endl;
    }

    void print_var_mid(){ //变量中间代码输出
//        std::cout << opration_type[mid_code_tab[midcodeidx - 1].op] << ' ' << mid_code_tab[midcodeidx - 1].z <<
//           ' ' <<  mid_code_tab[midcodeidx - 1].x << std::endl;
        mid_code_result << opration_type[mid_code_tab[midcodeidx - 1].op] << ' ' << mid_code_tab[midcodeidx - 1].z <<\
            ' ' <<  mid_code_tab[midcodeidx - 1].x << std::endl;
    }

    void print_mid_code_condition(){ //跳转条件中间代码输出
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
    // 以下开始是中间代码向目标代码转化过程
    /////////////////////////////////////////////////////////////////////////////////////////////

    void enter_mips_0(std::string labels){  // 没有操作数 .data /.text /label: /syscall
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].z = labels; // 标签被记录到z 中
        mipsidx ++;
        mips_code_result << labels << std::endl;
    }

    void enter_mips_1(std::string z , std::string x , std::string y){  // 三个字符串 str_0: .asciiz "haha"
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].z = z;
        mips_code_tab[mipsidx].x = x;
        mips_code_tab[mipsidx].y = y;
        mipsidx ++;
        mips_code_result << z << ' ' << x << ' ' << y << std::endl;
    }

    void enter_mips_2(mipsop op , std::string z , int imm){ // li $v0 4 这类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].imm= imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << imm << std::endl;
    }

    void enter_mips_3(mipsop op , std::string z , std::string x){ // la $a0 str_0  / move $fp $sp 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << std::endl;
    }

    void enter_mips_4(mipsop op , std::string z , std::string x , int imm){  // subi $sp $sp 4 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].imm  = imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << ' ' << imm << std::endl;
    }

    void enter_mips_5(mipsop op , std::string z , std::string x , int imm){ // sw $t0 0($sp) 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].imm  = imm;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << imm << '(' << x << ")\n";
    }

    void enter_mips_6(mipsop op , std::string z){  // jal function 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << std::endl;
    }

    void enter_mips_7(mipsop op , std::string z , std::string x , std::string y){  // add $t2 $t0 $t1 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = z;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].y  = y;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << z << ' ' << x << ' ' << y << std::endl;
    }

    void enter_mips_8(mipsop op , std::string x , std::string y , std::string labels){ //beq $t0 $t1 label 类指令
        if(mipsidx == MIPSCODEMAX){
            fatal(2); // 目标代码表溢出
        }
        mips_code_tab[mipsidx].op = op;
        mips_code_tab[mipsidx].z  = labels;
        mips_code_tab[mipsidx].x  = x;
        mips_code_tab[mipsidx].y  = y;
        mipsidx ++;
        mips_code_result << mips_op[op] << ' ' << x << ' ' << y << ' ' << labels << std::endl;
    }
}

