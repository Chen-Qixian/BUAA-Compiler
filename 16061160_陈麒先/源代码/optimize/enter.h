#ifndef _ENTER_H_
#define _ENTER_H_

#include "parameters.h"
#include "error.h"

namespace compiler{
    void enter_tab(std::string idname , objtyp object , identyp idtype , int length , int adrr);
    void enter_mid_code(std::string z , std::string x , opration op , std::string y);
    void enter_mid_code_only(std::string z , std::string x , opration op , std::string y);
    void enter_mid_code_condition(opration op , std::string z , std::string x , std::string y);
    int lookup_field(std::string name);
    int lookup_global(std::string name);
    int lookup_func(std::string name);
    int lookup_var(std::string name);
    int lookup_cur_field(std::string name , int startidx);
    int lookup_func_idx(std::string name);
    void print_table();
    void print_mid_code();
    void print_const_mid();
    void print_var_mid();
    void print_mid_code_condition();
    void mid_code_output_check();
    void enter_mips_0(std::string labels);
    void enter_mips_1(std::string z , std::string x , std::string y);
    void enter_mips_2(mipsop op , std::string z , int imm);
    void enter_mips_3(mipsop op , std::string z , std::string x);
    void enter_mips_4(mipsop op , std::string z , std::string x , int imm);
    void enter_mips_5(mipsop op , std::string z , std::string x , int imm); // sw $t0 0($sp)
    void enter_mips_6(mipsop op , std::string z);
    void enter_mips_7(mipsop op , std::string z , std::string x , std::string y);  // add $t2 $t0 $t1
    void enter_mips_8(mipsop op , std::string x , std::string y , std::string labels); //beq $t0 $t1 label 类指�?
}
#endif // _ENTER_H_
