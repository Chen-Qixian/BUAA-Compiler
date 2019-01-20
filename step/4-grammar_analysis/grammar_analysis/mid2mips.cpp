#include "mid2mips.h"

namespace compiler{
mid_code cur_mid;   // 当前正在处理的中间代码
int global_var_beg = 0x10010000;  // 全局变量存储在.data中，在字符串生成后加上字符串偏移即为全局变量起始位置
int cur_func_idx;   // 当前正在处理的函数在tab_idx中的下标tabidxidx
int push_num = 0;   // 当前传参的个数，在call中清零

int varlen = 0; // 当前函数中参数，变量个数
int tmplen = 0; // 当前函数中临时变量个数
int shamt = 0;  // (varlen + tmplen + 2) * 4
identyp tmptype = notdef; // 临时变量的类型 char/int 只有在pirntf处理中，getType时用到
std::string lastcall;
int lastidx; // 从mips表中查找上一条指令内容索引，只有在pirntf处理中，getType时用到

midoptyp midop_analyse(std::string op , int& offset){
    // 分析中间代码操作符种类，返回值为种类，参数修改该操作符的地址偏移，或常量数值
    int idx_in_tab;
    int shamt;
    if(op == "RET") return ret_midop;
    else if(str_is_num(op)){        // 操作是数字类型
        offset = str2int(op);       // offset 为该操作数的数值
        return imm_midop;
    }
    else if(op[0] == '\''){  // 操作为字符
        offset = op[1];      // offset 为字符ascii码值
        return imm_midop;
    }
    else if(op[0] == '$'){
        offset = (-4) * (str2int(op.substr(2))); // 临时变量格式$Tx,故从op第2字符开始截取子串，转为数字后*4即为从$sp开始的偏移量
        return tmpvar_midop;
    }
    else{
        if(op.find("[") <= op.length()){   // 该操作为在op串中找到【，即是数组元素
            op = op.substr(0,op.find("["));  // 截取出数组名
        }
        idx_in_tab = lookup_cur_field(op , cur_func_idx);
        if(idx_in_tab == -1){                  // 不在局部，则应从全局进行查找
            idx_in_tab = lookup_global(op);
            if(idx_in_tab == -1){
                std::cout << lastidx << std::endl;
                std::cout << cur_func_idx << std::endl;
                std::cout << op << std::endl;
                std::cout << "###############ERROR in midop_analyse##############\n";
            }
            else{                              // 在全局找到
                tab_entry t = tab[idx_in_tab];
                if(t.obj == constobj){         // 如果是常量
                    offset = t.adr;            // 则offset为该常量值
                    return imm_midop;          // 返回是数字，修改offset为该常量值
                }
                else if(t.obj == varobj){
                    shamt = tab[idx_in_tab].adr * 4; // 读取该变量在全局中的位置*4算偏移
                    offset = global_var_beg + shamt; // 全局从.data中开始计算，返回结果直接.data中的地址
                    return global_var_midop;
                }
                else if(t.obj == arrobj){
                    shamt = tab[idx_in_tab].adr * 4; // 读取该数组在全局中的首位置*4算偏移
                    offset = global_var_beg + shamt; // 全局从.data中开始计算，返回结果直接.data中的地址
                    return global_arr_midop;
                }
            }
        }
        else{            // 该变量在局部
            tab_entry t = tab[idx_in_tab];
            if(t.obj == constobj){         // 如果是常量
                offset = t.adr;
                return imm_midop;          // 返回是数字，修改offset为该常量值
            }
            else if(t.obj == varobj || t.obj == paraobj){  // 局部参数也返回为局部变量类型
                shamt = tab[idx_in_tab].adr * (-4); // 读取该变量在局部中的位置*4算偏移
                offset = shamt;                  // 这个offset需要加上$fp
                return var_midop;
            }
            else if(t.obj == arrobj){
                shamt = tab[idx_in_tab].adr * (-4); // 读取该数组首地址在局部中的位置*4算偏移
                offset = shamt;                  // 这个offset需要加上$fp
                return arr_midop;
            }
        }
    }
    return err_midop; // 返回错误类型
}

void gen_mips_str(){  // 生成目标代码中.data域的字符串
    int slength = 0;  // 字符串累计长度，用于计算全局变量起始位置
    std::string strno;
    enter_mips_0(".globl main");
    enter_mips_0(".data");
    for(int i = 0 ; i < str_idx ; i ++){
        std::string str = str_modify(string_tab[i]);
        strno = "str_" + int2str(i) + ":";
        enter_mips_1(strno , ".asciiz" , str);
        slength += (str.length() - 1);
    }
    enter_mips_1("str_newline:" , ".asciiz" , "\"\\n\"");
    slength ++;
    slength += (4 - slength % 4);    // 地址必须按字对齐
    global_var_beg += slength;
    enter_mips_0(".text");
}

void loadOp(std::string op , std::string reg){       // 根据op加载值到寄存器 【数组未检查】
    int offset;
    midoptyp MOT = midop_analyse(op , offset);
    if(MOT == imm_midop){
        enter_mips_2(mli , reg , offset);  // offset 为该操作数的数值
    }
    else if(MOT == tmpvar_midop){       // 临时变量
        int off = offset + (-4) * varlen;
        enter_mips_5(mlw , reg , "$fp" , off);
    }
    else if(MOT == var_midop){
        enter_mips_5(mlw , reg , "$fp" , offset);
    }
    else if(MOT == global_arr_midop){           // 全局数组【未检查】
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * 4;
            enter_mips_2(mlw , reg , offset);
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t0" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in loadOp global_arr_midop###########\n";
        }
    }
    else if(MOT == arr_midop){     // 局部数组
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * (-4);
            enter_mips_5(mlw , reg , "$fp" , offset);
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t0" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in loadOp arr_midop###########\n";
        }
    }
    else if(MOT == global_var_midop){           // 全局变量
        enter_mips_2(mlw , reg , offset);
    }
}

void storeOp(std::string op , std::string reg){    // 根据op将寄存器值写入内存相应位置【未完成】
    int offset;
    midoptyp MOT = midop_analyse(op , offset);
    if(MOT == tmpvar_midop){       // 临时变量
        int off = offset + (-4) * varlen;
        enter_mips_5(msw , reg , "$fp" , off);
    }
    else if(MOT == var_midop){      // 局部变量
        enter_mips_5(msw , reg , "$fp" , offset);
    }
    else if(MOT == arr_midop){           // 局部数组【未完成】
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * (-4);
            enter_mips_5(msw , reg ,"$fp" , offset);
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t1" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t1" , "$t1" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else{
            std::cout << "******ERROR in storeOp arr_midop*******\n";
        }
    }
    else if(MOT == global_arr_midop){     // 全局数组【未完成】
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * 4;
            enter_mips_2(msw , reg , offset);
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t1" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(msw , reg , "$t1" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(msw , reg , "$t1" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in storeOp global_arr_midop###########\n";
        }
    }
    else if(MOT == global_var_midop){
        enter_mips_2(msw , reg , offset);
    }
}

identyp getType(std::string op){
    if(str_is_num(op)){
        return intiden;   // 如果直接打印数字 返回int型
    }
    if(op[0] == '\''){
        return chariden;  // 如果直接打印字符 返回char型
    }
    if(op[0] == '$'){          // 如果写的是临时变量，先暂时直接返回int类型（只有char返回值函数才输出char,之后考虑）
        if(tmptype == chariden && mid_code_tab[lastidx].op == RET){
            //tmptype = intiden;     // 输出完后需要回复原状，以免对后续产生干扰。
            return chariden;
        }
        return intiden;
    }
    if(op.find("[") < op.length()){  // 如果存在【，即为数组元素，需要截取出数组名进行查表，否则查表异常
        op = op.substr(0 , op.find("["));
    }
    int idx_in_tab;
    idx_in_tab = lookup_cur_field(op , cur_func_idx);
    if(idx_in_tab == -1){  // 从全局查找
        idx_in_tab = lookup_global(op);
        if(idx_in_tab == -1){  // 都未找到
            std::cout << op << std::endl;
            std::cout << cur_func_idx << std::endl;
            std::cout << "##########error in getType ############\n";
        }
        else{  // 出现在全局
            return tab[idx_in_tab].type;
        }
    }
    else{
        return tab[idx_in_tab].type;
    }
    return notdef;
}

void readOp(){                               // 处理read操作
    std::string name = cur_mid.z;
    int idx_in_tab = lookup_cur_field(name , cur_func_idx);
    if(idx_in_tab != -1){                 // 局部变量的读入已完成
        int off = tab[idx_in_tab].adr;
            identyp typ = tab[idx_in_tab].type;
            if(typ == intiden){
                enter_mips_2(mli , "$v0" , 5);
            }
            else if(typ == chariden){
                enter_mips_2(mli , "$v0" , 12);
            }
            enter_mips_0(mips_op[msyscall]);
            enter_mips_5(msw , "$v0" , "$fp" , off * (-4));
    }
    else{
        idx_in_tab = lookup_global(name);    // 全局变量的读入
        if(idx_in_tab != -1){    // 在全局找到
            int off = tab[idx_in_tab].adr;
            int addr = global_var_beg + 4 * off;
            identyp typ = tab[idx_in_tab].type;
            if(typ == intiden){
                enter_mips_2(mli , "$v0" , 5);
            }
            else if(typ == chariden){
                enter_mips_2(mli , "$v0" , 12);
            }
            enter_mips_0(mips_op[msyscall]);
            enter_mips_2(msw , "$v0" , addr);
            }
        else{
            std::cout << "ERROR in readOp()\n";
        }
    }
}

void writeOp(){ // 处理write操作
    std::string op = cur_mid.z;
    if(op[0] == '\"'){              // 该输出为一个字符串
        std::string strx;
        strx = "str_" + int2str(str_num);
        str_num ++;
        enter_mips_3(mla , "$a0" , strx);
        enter_mips_2(mli , "$v0" , 4);
        enter_mips_0(mips_op[msyscall]);
    }
    else{
        loadOp(op , "$a0");
        identyp printtype = getType(op);
        if(printtype == chariden){
            enter_mips_2(mli , "$v0" , 11);
        }
        else if(printtype == intiden){
            enter_mips_2(mli , "$v0" , 1);
        }
        enter_mips_0("syscall");
        enter_mips_3(mla , "$a0" , "str_newline");
        enter_mips_2(mli , "$v0" , 4);
        enter_mips_0(mips_op[msyscall]);
    }
}

void funcOp(){
    std::string fname = cur_mid.x;  // z存了类型。。。
    if(fname == "main"){
        enter_mips_0(fname + ":");
    }
    else{
        enter_mips_0("func_" + fname + ":");
    }

    enter_mips_3(mmove , "$fp" , "$sp");            // 存当前函数位置到$fp
    varlen = tab[lookup_func(fname)].len;   // 查找函数位置，查表，获得函数中变量长度
    tmplen = tab[lookup_func(fname)].adr;   // 查找函数位置，查表，获得函数中临时变量长度
    shamt = (varlen + tmplen + 2) * 4;          // 预留出局部变量，临时变量，和$ra,$fp的位置
    if(shamt > 0){
        enter_mips_4(msubi , "$sp" , "$sp" , shamt);
    }
    cur_func_idx = lookup_func_idx(fname);
}

void funcendOp(){
    enter_mips_4(maddi , "$sp" ,"$sp" , shamt);
    enter_mips_6(mjr , "$ra");
}

void pushOp(){  // 【未完成】
    std::string op = cur_mid.z;        // z 中不可能出现RET
    int offset;
    if(midop_analyse(op , offset) == imm_midop){
        enter_mips_2(mli , "$t0" , offset);  // offset 为该操作数的数值
        enter_mips_5(msw , "$t0" , "$sp" , -4 * push_num);
        push_num ++;
    }
    else if(midop_analyse(op , offset) == tmpvar_midop){       //临时变量的压入
        int off = offset + (-4) * varlen;
        enter_mips_5(mlw , "$t0" , "$fp" , off);
        enter_mips_5(msw , "$t0" , "$sp" , -4 * push_num);
        push_num ++;
    }
    else if(midop_analyse(op , offset) == var_midop){
        enter_mips_5(mlw , "$t0" , "$fp" , offset);
        enter_mips_5(msw , "$t0" , "$sp" , -4 * push_num);
        push_num ++;
    }
    else if(midop_analyse(op , offset) == arr_midop){           //////// 局部数组【未完成】
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * (-4);
            enter_mips_5(mlw , "$t1" , "$fp" , offset);      // 取数组值到$t1
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t0" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  找到该元素位置
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in pushOp arr_midop###########\n";
        }
        enter_mips_5(msw , "$t1" , "$sp" , -4 * push_num);  // 这里取数组的值到了$t1中
        push_num ++;
    }
    else if(midop_analyse(op , offset) == global_arr_midop){     //////// 全局数组【未完成】
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // 获取数组下标索引
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // 数组下标是常量
            offset += off * 4;
            enter_mips_2(mlw , "$t1" , offset);
        }
        else if(mot == var_midop){      // 数组下标是局部变量
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // 数组下标是全局变量
            enter_mips_2(mlw , "$t0" , off);           // 不同于上文，直接从地址中取数
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // 数组下标是临时变量
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off 为下标索引在函数中的偏移量 lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset 为数组的首地址  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in pushOp global_arr_midop###########\n";
        }
        enter_mips_5(msw , "$t1" , "$sp" , -4 * push_num); // 这里取数组的值到了$t1中
        push_num ++;
    }
    else if(midop_analyse(op , offset) == global_var_midop){
        enter_mips_2(mlw , "$t0" , offset);
        enter_mips_5(msw , "$t0" , "$sp" , -4 * push_num);
        push_num ++;
    }
}

void callOp(){
    std::string fname = cur_mid.z;
    enter_mips_5(msw , "$ra" , "$sp" , 4);      // sw $ra 4($sp)
    enter_mips_5(msw , "$fp" , "$sp" , 8);      // sw $fp 8($sp)
    enter_mips_6(mjal , "func_" + fname);                 // jal func
    enter_mips_5(mlw , "$ra" , "$sp" , 4);      // lw $ra 4($sp)
    enter_mips_5(mlw , "$fp" , "$sp" , 8);      // lw $fp 8($sp)
    push_num = 0;
    lastcall = fname;        // 最后一次调用的函数名，用于判断该函数的返回值类型，用以控制输出
}

void calculationOp(){
    std::string z = cur_mid.z;
    std::string x = cur_mid.x;
    std::string y = cur_mid.y;
    loadOp(x , "$t1");
    loadOp(y , "$t2");
    if(cur_mid.op == add){
        enter_mips_7(madd , "$t0" , "$t1" , "$t2");
    }
    else if(cur_mid.op == sub){
        enter_mips_7(msub , "$t0" , "$t1" , "$t2");
    }
    else if(cur_mid.op == mult){
        enter_mips_7(mmul , "$t0" , "$t1" , "$t2");
    }
    else if(cur_mid.op == div){
        enter_mips_7(mdiv , "$t0" , "$t1" , "$t2");
    }
    storeOp(z , "$t0");
}

void assignOp(){
    std::string z = cur_mid.z;
    std::string x = cur_mid.x;
    loadOp(x , "$t0");
    storeOp(z , "$t0");

}

void branchOp(){
    std::string labels = cur_mid.z;
    if(cur_mid.op == j){
        enter_mips_6(mj , labels);
        return;
    }

    std::string x = cur_mid.x;
    loadOp(x , "$t0");
    if(cur_mid.op == beqz){
        enter_mips_3(mbeqz , "$t0" , labels);
        return;
    }

    std::string y = cur_mid.y;
    loadOp(y , "$t1");
    switch(cur_mid.op){
        case beq:
            enter_mips_8(mbeq , "$t0" ,"$t1" , labels);
            break;
        case bne:
            enter_mips_8(mbne , "$t0" ,"$t1" , labels);
            break;
        case bgt:
            enter_mips_8(mbgt , "$t0" ,"$t1" , labels);
            break;
        case bge:
            enter_mips_8(mbge , "$t0" ,"$t1" , labels);
            break;
        case blt:
            enter_mips_8(mblt , "$t0" ,"$t1" , labels);
            break;
        case ble:
            enter_mips_8(mble , "$t0" ,"$t1" , labels);
            break;
        default:
            break;
    }
}

void retOp(){
    std::string z = cur_mid.z;
    if(cur_mid.op == ret){
        loadOp(z , "$v0");
    }
    else if(cur_mid.op == RET){
        storeOp(z , "$v0");
        if(cur_mid.x == "RET"){
            tmptype = tab[tab_idx[lookup_func_idx(lastcall)]].type; // 如果是返回值的赋值，需要记录该返回值类型，用于打印char（）函数时确定类型
            //std::cout << "####" << lastcall << std::endl;
            //std::cout << "####" << tab[tab_idx[lookup_func_idx(lastcall)]].name << std::endl;
            //std::cout << "####" << iden_type[tmptype] << std::endl;
            // 只有当前函数返回为char的时候，输出类型才为char，否则为int
        }
    }

}

void mid2mips(){
    mips_code_result.open("16061160_mips_code.txt");
    gen_mips_str();
    for(int i = 0 ; i < midcodeidx ; i ++){
        lastidx = i - 1;
        cur_mid = mid_code_tab[i];
        switch(cur_mid.op){
            case read:{
                readOp();
                break;
            }
            case write:{
                writeOp();
                break;
            }
            case func:{
                funcOp();
                break;
            }
            case funcend:{
                // addi $sp
                funcendOp();
                break;
            }
            case exitprog:{
                enter_mips_2(mli , "$v0" , 10);
                enter_mips_0("syscall");
                break;
            }
            case label:{
                enter_mips_0(cur_mid.z);
                break;
            }
            case push:{
                pushOp();
                break;
            }
            case call:{
                callOp();
                break;
            }
            case add:
            case sub:
            case mult:
            case div:{
                calculationOp();
                break;
            }
            case assignop:{
                assignOp();
                break;
            }
            case j:
            case beqz:
            case beq:
            case bne:
            case bgt:
            case bge:
            case blt:
            case ble:{
                branchOp();
                break;
            }
            case RET:
            case ret:{
                retOp();
                break;
            }
            default: break;
        }
    }
}

}
