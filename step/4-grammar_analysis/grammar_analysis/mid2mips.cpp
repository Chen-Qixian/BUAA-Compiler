#include "mid2mips.h"

namespace compiler{
mid_code cur_mid;   // ��ǰ���ڴ�����м����
int global_var_beg = 0x10010000;  // ȫ�ֱ����洢��.data�У����ַ������ɺ�����ַ���ƫ�Ƽ�Ϊȫ�ֱ�����ʼλ��
int cur_func_idx;   // ��ǰ���ڴ���ĺ�����tab_idx�е��±�tabidxidx
int push_num = 0;   // ��ǰ���εĸ�������call������

int varlen = 0; // ��ǰ�����в�������������
int tmplen = 0; // ��ǰ��������ʱ��������
int shamt = 0;  // (varlen + tmplen + 2) * 4
identyp tmptype = notdef; // ��ʱ���������� char/int ֻ����pirntf�����У�getTypeʱ�õ�
std::string lastcall;
int lastidx; // ��mips���в�����һ��ָ������������ֻ����pirntf�����У�getTypeʱ�õ�

midoptyp midop_analyse(std::string op , int& offset){
    // �����м������������࣬����ֵΪ���࣬�����޸ĸò������ĵ�ַƫ�ƣ�������ֵ
    int idx_in_tab;
    int shamt;
    if(op == "RET") return ret_midop;
    else if(str_is_num(op)){        // ��������������
        offset = str2int(op);       // offset Ϊ�ò���������ֵ
        return imm_midop;
    }
    else if(op[0] == '\''){  // ����Ϊ�ַ�
        offset = op[1];      // offset Ϊ�ַ�ascii��ֵ
        return imm_midop;
    }
    else if(op[0] == '$'){
        offset = (-4) * (str2int(op.substr(2))); // ��ʱ������ʽ$Tx,�ʴ�op��2�ַ���ʼ��ȡ�Ӵ���תΪ���ֺ�*4��Ϊ��$sp��ʼ��ƫ����
        return tmpvar_midop;
    }
    else{
        if(op.find("[") <= op.length()){   // �ò���Ϊ��op�����ҵ�������������Ԫ��
            op = op.substr(0,op.find("["));  // ��ȡ��������
        }
        idx_in_tab = lookup_cur_field(op , cur_func_idx);
        if(idx_in_tab == -1){                  // ���ھֲ�����Ӧ��ȫ�ֽ��в���
            idx_in_tab = lookup_global(op);
            if(idx_in_tab == -1){
                std::cout << lastidx << std::endl;
                std::cout << cur_func_idx << std::endl;
                std::cout << op << std::endl;
                std::cout << "###############ERROR in midop_analyse##############\n";
            }
            else{                              // ��ȫ���ҵ�
                tab_entry t = tab[idx_in_tab];
                if(t.obj == constobj){         // ����ǳ���
                    offset = t.adr;            // ��offsetΪ�ó���ֵ
                    return imm_midop;          // ���������֣��޸�offsetΪ�ó���ֵ
                }
                else if(t.obj == varobj){
                    shamt = tab[idx_in_tab].adr * 4; // ��ȡ�ñ�����ȫ���е�λ��*4��ƫ��
                    offset = global_var_beg + shamt; // ȫ�ִ�.data�п�ʼ���㣬���ؽ��ֱ��.data�еĵ�ַ
                    return global_var_midop;
                }
                else if(t.obj == arrobj){
                    shamt = tab[idx_in_tab].adr * 4; // ��ȡ��������ȫ���е���λ��*4��ƫ��
                    offset = global_var_beg + shamt; // ȫ�ִ�.data�п�ʼ���㣬���ؽ��ֱ��.data�еĵ�ַ
                    return global_arr_midop;
                }
            }
        }
        else{            // �ñ����ھֲ�
            tab_entry t = tab[idx_in_tab];
            if(t.obj == constobj){         // ����ǳ���
                offset = t.adr;
                return imm_midop;          // ���������֣��޸�offsetΪ�ó���ֵ
            }
            else if(t.obj == varobj || t.obj == paraobj){  // �ֲ�����Ҳ����Ϊ�ֲ���������
                shamt = tab[idx_in_tab].adr * (-4); // ��ȡ�ñ����ھֲ��е�λ��*4��ƫ��
                offset = shamt;                  // ���offset��Ҫ����$fp
                return var_midop;
            }
            else if(t.obj == arrobj){
                shamt = tab[idx_in_tab].adr * (-4); // ��ȡ�������׵�ַ�ھֲ��е�λ��*4��ƫ��
                offset = shamt;                  // ���offset��Ҫ����$fp
                return arr_midop;
            }
        }
    }
    return err_midop; // ���ش�������
}

void gen_mips_str(){  // ����Ŀ�������.data����ַ���
    int slength = 0;  // �ַ����ۼƳ��ȣ����ڼ���ȫ�ֱ�����ʼλ��
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
    slength += (4 - slength % 4);    // ��ַ���밴�ֶ���
    global_var_beg += slength;
    enter_mips_0(".text");
}

void loadOp(std::string op , std::string reg){       // ����op����ֵ���Ĵ��� ������δ��顿
    int offset;
    midoptyp MOT = midop_analyse(op , offset);
    if(MOT == imm_midop){
        enter_mips_2(mli , reg , offset);  // offset Ϊ�ò���������ֵ
    }
    else if(MOT == tmpvar_midop){       // ��ʱ����
        int off = offset + (-4) * varlen;
        enter_mips_5(mlw , reg , "$fp" , off);
    }
    else if(MOT == var_midop){
        enter_mips_5(mlw , reg , "$fp" , offset);
    }
    else if(MOT == global_arr_midop){           // ȫ�����顾δ��顿
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * 4;
            enter_mips_2(mlw , reg , offset);
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t0" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in loadOp global_arr_midop###########\n";
        }
    }
    else if(MOT == arr_midop){     // �ֲ�����
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * (-4);
            enter_mips_5(mlw , reg , "$fp" , offset);
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t0" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , reg , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in loadOp arr_midop###########\n";
        }
    }
    else if(MOT == global_var_midop){           // ȫ�ֱ���
        enter_mips_2(mlw , reg , offset);
    }
}

void storeOp(std::string op , std::string reg){    // ����op���Ĵ���ֵд���ڴ���Ӧλ�á�δ��ɡ�
    int offset;
    midoptyp MOT = midop_analyse(op , offset);
    if(MOT == tmpvar_midop){       // ��ʱ����
        int off = offset + (-4) * varlen;
        enter_mips_5(msw , reg , "$fp" , off);
    }
    else if(MOT == var_midop){      // �ֲ�����
        enter_mips_5(msw , reg , "$fp" , offset);
    }
    else if(MOT == arr_midop){           // �ֲ����顾δ��ɡ�
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * (-4);
            enter_mips_5(msw , reg ,"$fp" , offset);
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t1" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t1" , "$t1" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t1" , "$t1" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else{
            std::cout << "******ERROR in storeOp arr_midop*******\n";
        }
    }
    else if(MOT == global_arr_midop){     // ȫ�����顾δ��ɡ�
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * 4;
            enter_mips_2(msw , reg , offset);
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(msw , reg , "$t1" , 0);              // sw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t1" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(msw , reg , "$t1" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t1" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t1" , "$t1" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t1" , "$t1" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
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
        return intiden;   // ���ֱ�Ӵ�ӡ���� ����int��
    }
    if(op[0] == '\''){
        return chariden;  // ���ֱ�Ӵ�ӡ�ַ� ����char��
    }
    if(op[0] == '$'){          // ���д������ʱ����������ʱֱ�ӷ���int���ͣ�ֻ��char����ֵ���������char,֮���ǣ�
        if(tmptype == chariden && mid_code_tab[lastidx].op == RET){
            //tmptype = intiden;     // ��������Ҫ�ظ�ԭ״������Ժ����������š�
            return chariden;
        }
        return intiden;
    }
    if(op.find("[") < op.length()){  // ������ڡ�����Ϊ����Ԫ�أ���Ҫ��ȡ�����������в���������쳣
        op = op.substr(0 , op.find("["));
    }
    int idx_in_tab;
    idx_in_tab = lookup_cur_field(op , cur_func_idx);
    if(idx_in_tab == -1){  // ��ȫ�ֲ���
        idx_in_tab = lookup_global(op);
        if(idx_in_tab == -1){  // ��δ�ҵ�
            std::cout << op << std::endl;
            std::cout << cur_func_idx << std::endl;
            std::cout << "##########error in getType ############\n";
        }
        else{  // ������ȫ��
            return tab[idx_in_tab].type;
        }
    }
    else{
        return tab[idx_in_tab].type;
    }
    return notdef;
}

void readOp(){                               // ����read����
    std::string name = cur_mid.z;
    int idx_in_tab = lookup_cur_field(name , cur_func_idx);
    if(idx_in_tab != -1){                 // �ֲ������Ķ��������
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
        idx_in_tab = lookup_global(name);    // ȫ�ֱ����Ķ���
        if(idx_in_tab != -1){    // ��ȫ���ҵ�
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

void writeOp(){ // ����write����
    std::string op = cur_mid.z;
    if(op[0] == '\"'){              // �����Ϊһ���ַ���
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
    std::string fname = cur_mid.x;  // z�������͡�����
    if(fname == "main"){
        enter_mips_0(fname + ":");
    }
    else{
        enter_mips_0("func_" + fname + ":");
    }

    enter_mips_3(mmove , "$fp" , "$sp");            // �浱ǰ����λ�õ�$fp
    varlen = tab[lookup_func(fname)].len;   // ���Һ���λ�ã������ú����б�������
    tmplen = tab[lookup_func(fname)].adr;   // ���Һ���λ�ã������ú�������ʱ��������
    shamt = (varlen + tmplen + 2) * 4;          // Ԥ�����ֲ���������ʱ��������$ra,$fp��λ��
    if(shamt > 0){
        enter_mips_4(msubi , "$sp" , "$sp" , shamt);
    }
    cur_func_idx = lookup_func_idx(fname);
}

void funcendOp(){
    enter_mips_4(maddi , "$sp" ,"$sp" , shamt);
    enter_mips_6(mjr , "$ra");
}

void pushOp(){  // ��δ��ɡ�
    std::string op = cur_mid.z;        // z �в����ܳ���RET
    int offset;
    if(midop_analyse(op , offset) == imm_midop){
        enter_mips_2(mli , "$t0" , offset);  // offset Ϊ�ò���������ֵ
        enter_mips_5(msw , "$t0" , "$sp" , -4 * push_num);
        push_num ++;
    }
    else if(midop_analyse(op , offset) == tmpvar_midop){       //��ʱ������ѹ��
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
    else if(midop_analyse(op , offset) == arr_midop){           //////// �ֲ����顾δ��ɡ�
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * (-4);
            enter_mips_5(mlw , "$t1" , "$fp" , offset);      // ȡ����ֵ��$t1
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t0" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t0" , "$t0" , -4);          // mul $t0 $t0 -4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , -4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_7(madd , "$t0" , "$t0" , "$fp");       // add $t0 $t0 $fp  �ҵ���Ԫ��λ��
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in pushOp arr_midop###########\n";
        }
        enter_mips_5(msw , "$t1" , "$sp" , -4 * push_num);  // ����ȡ�����ֵ����$t1��
        push_num ++;
    }
    else if(midop_analyse(op , offset) == global_arr_midop){     //////// ȫ�����顾δ��ɡ�
        std::string idx = op.substr(op.find("[") + 1 , op.find("]") - op.find("[") - 1);   // ��ȡ�����±�����
        int off;
        midoptyp mot = midop_analyse(idx , off);
        if(mot == imm_midop){        // �����±��ǳ���
            offset += off * 4;
            enter_mips_2(mlw , "$t1" , offset);
        }
        else if(mot == var_midop){      // �����±��Ǿֲ�����
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == global_var_midop){  // �����±���ȫ�ֱ���
            enter_mips_2(mlw , "$t0" , off);           // ��ͬ�����ģ�ֱ�Ӵӵ�ַ��ȡ��
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else if(mot == tmpvar_midop){      // �����±�����ʱ����
            off = (-4) * varlen + off;
            enter_mips_5(mlw , "$t0" , "$fp" , off);          // off Ϊ�±������ں����е�ƫ���� lw $t0 off($fp)
            enter_mips_4(mmul , "$t0" , "$t0" , 4);           // mul $t0 $t0 4
            enter_mips_4(maddi , "$t0" , "$t0" , offset);     // offset Ϊ������׵�ַ  addi $t0 $t0 offset
            enter_mips_5(mlw , "$t1" , "$t0" , 0);              // lw reg 0($t0)
        }
        else{
            std::cout << "########ERROR in pushOp global_arr_midop###########\n";
        }
        enter_mips_5(msw , "$t1" , "$sp" , -4 * push_num); // ����ȡ�����ֵ����$t1��
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
    lastcall = fname;        // ���һ�ε��õĺ������������жϸú����ķ���ֵ���ͣ����Կ������
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
            tmptype = tab[tab_idx[lookup_func_idx(lastcall)]].type; // ����Ƿ���ֵ�ĸ�ֵ����Ҫ��¼�÷���ֵ���ͣ����ڴ�ӡchar��������ʱȷ������
            //std::cout << "####" << lastcall << std::endl;
            //std::cout << "####" << tab[tab_idx[lookup_func_idx(lastcall)]].name << std::endl;
            //std::cout << "####" << iden_type[tmptype] << std::endl;
            // ֻ�е�ǰ��������Ϊchar��ʱ��������Ͳ�Ϊchar������Ϊint
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
