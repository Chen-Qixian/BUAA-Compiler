#include "grammar.h"

namespace compiler{
// �﷨�����׶�����ļ�����
std::ofstream grammar_analysis_result;
// �м��������ļ�����
//std::ofstream mid_code_result;
// ��ʼ���ż���������������
symbol block_beg_set[4] = {CONST , INT , CHARKW , VOID};
symbol compound_beg_set[11] = {IDEN , INT , CHARKW , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH};
symbol statement_beg_set[9] = {IDEN , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH}; //������{�����}
symbol statementlist_beg_set[10] = {IDEN , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH , RDKH};
symbol expression_beg_set[6] = {PLUS , MINUS , IDEN , UINT , CHAR , LPAR};
symbol item_beg_set[6] = {IDEN , UINT , CHAR , LPAR , PLUS , MINUS};  // ������ӿ�ʼ���ż���ͬ
// ����������ż�
symbol var_set[4] = {COMMA , LZKH , SEMI , LPAR};
symbol vardec_set[3] = {COMMA , LZKH , SEMI};
symbol func_beg_set[1] = {LDKH};
symbol relationOp[6] = {EQUAL , NEQ , GREAT , GEQ , LESS , LEQ };
// �����ֹ���ż�
symbol statement_end_set[2] = {SEMI , RDKH};
// һЩ��������
symbol typeiden; // ��������Ԥ���ı�������
symbol funcname; // Ǳ�ں��������ͣ���ʶ����main��
int var_off; // �����ڸò���µĵ�ַƫ����
identyp this_type; // ��ǰ�ж������������
identyp return_type;
std::string idname; // ��ʶ����
int paraidx; // ����λ�ã���valueParaList ��ʹ��
bool returnflag = false;

std::string new_tmp(){
    return "$T" + int2str(tmp_var ++);
}

void constDeclare(){
    int sign = 1;// ��������ķ���λ������1�� -1����
    int inum = 0;   // ���ֳ���ֵ

    grammar_analysis_result << "a const declaration statement.\n";
    getsym();
    if(sy != INT && sy != CHARKW){
        error(0); // ȱ�����Ͷ��� int �� char.
        skip(block_beg_set , 4);
        return;  // �������õķ��ţ�ֱ��������������ʼ���ŷ��أ���ʱsyΪ������ʼ���ż���ķ���
    }
    this_type = sym2type(sy);// ����ת�����������ں������
    do{
        getsym();
        if(sy != IDEN){
            error(2); // ��ʶ��ȱʧ
            skip(block_beg_set , 4);
            return;
        }
        idname = id; // string = char[]
        getsym();
        if(sy != ASSIGN){
            error(3); // ȱ�ٸ�ֵ����
            skip(block_beg_set , 4);
            return;
        }
        getsym();
        if(sy == PLUS || sy == MINUS || sy == UINT){ // int const
            if(this_type != intiden){
                error(5); // ����������������Ͳ�ƥ��
                skip(block_beg_set , 4);
                return;
            }
            if(sy == PLUS){
                getsym();
            }
            else if(sy == MINUS){
                sign = -1;
                getsym();
            }
            if(sy != UINT){
                error(4); // ȱ�ٳ�����ֵ
                skip(block_beg_set , 4);
                return;
            }
            inum = sign * std::atoi(id);// ����sign���㳣��ֵ�����
            enter_mid_code_only("int" , idname , con , int2str(inum));
            print_const_mid();
        }
        else if(sy == CHAR){             // char const
            if(this_type != chariden){
                error(5); // ����������������Ͳ�ƥ��
                skip(block_beg_set , 4);
                return;
            }
            inum = (int) id[1];// ���
            enter_mid_code_only("char" , idname , con , id );
            print_const_mid();
        }
        else{
            error(6);  // �������岻����
            skip(block_beg_set , 4);
            return;
        }
        getsym();
        enter_tab(idname , constobj , this_type , 0 , inum);
    }while(sy == COMMA);
    if(sy != SEMI){
        error(1); // ����βȱ�ٷֺ�
        skip(block_beg_set , 4);
        return;
    }
    //�˳���������ҪԤ��һ�����ŵ�;����һ������
    getsym();
    return;
}

void arrayDeclare(){
    if(sy == LZKH){
        getsym();
        if(sy != UINT){
            error(10); // ��������ֵ����Ϊ����
            skip(block_beg_set , 4);
            return;
        }
        else{
            int arr_length = std::atoi(id);// �˴����
            enter_tab(idname , arrobj , this_type , arr_length , var_off);
            enter_mid_code_only(type2str(this_type) , idname , arr , "");
            print_var_mid();
            var_off += arr_length;
            getsym();
            if(sy != RZKH){
                error(11); // ȱ����������
                skip(block_beg_set , 4);
                return;
            }
            else{
                getsym();
            }
        }
        if(sy != COMMA && sy != SEMI){
            error(12); // ȱ�ٶ��Ż�ֺ�
            skip(block_beg_set , 4);
            return;
        }
    }
    else{
        enter_tab(idname , varobj , this_type , 0 , var_off);
        var_off ++;// ����grammar()�ж����ı������
        enter_mid_code_only(type2str(this_type) , idname , var , "");
        print_var_mid();
    }
}

void varDeclare(){
    // ��ڴ�Ӧ���Ƕ���,;[�����ֽ��
    grammar_analysis_result << "variable declaration statement.\n";
    // �����ж��Ƿ�[ȷ���Ƿ����飬�����¼Ϊ����
    this_type = sym2type(typeiden);
    arrayDeclare();
    while(sy == COMMA){
        getsym();
        if(sy != IDEN){
            error(2); //��ʶ��ȱʧ
            skip(block_beg_set , 4);
            return;
        }
        idname = id;
        getsym();
        if(!find_sym(vardec_set , 3)){
            error(8); // ��������ʶ�𵽷Ƿ�����
            skip(block_beg_set , 4);
            continue;
        }
        arrayDeclare();
    }
    if(sy != SEMI){
        error(1); // ����βȱ�ٷֺ�
        skip(block_beg_set , 4);
        return;
    }
    getsym();// ����Ϊ������һ����
    return;
}

void addPara(){
    enter_tab(idname , paraobj , this_type , 0 , var_off);
    var_off ++;
    enter_mid_code_only(type2str(this_type) , idname , para , "");
    print_var_mid();
}

void paraList(){
    // ���Ϊ��С����
    grammar_analysis_result << "this is a parameter list.\n";
    getsym();
    if(sy == RPAR){
        // �����ŷ��أ��޲���
        getsym(); // ����Ϊ������һ����
        return;
    }
    if(sy != INT && sy != CHARKW){
        error(0); // ȱ�����Ͷ��� int �� char.
        skip(func_beg_set , 1);
        return;
    }
    this_type = sym2type(sy);
    getsym(); // ��������
    if(sy != IDEN){
        error(2);//��ʶ��ȱʧ
        skip(func_beg_set , 1);
        return;
    }
    idname = id;
    addPara(); // ��ǰ�Ķ���
    getsym();
    while(sy == COMMA){
        getsym();
        if(sy != INT && sy != CHARKW){
            error(0); // ȱ�����Ͷ��� int �� char.
            skip(func_beg_set , 1);
            return;
        }
        this_type = sym2type(sy);
        getsym();
        if(sy != IDEN){
            error(2);//��ʶ��ȱʧ
            skip(func_beg_set , 1);
            return;
        }
        idname = id;
        addPara();
        getsym();
    }
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(func_beg_set , 1);
        return;
    }
    getsym(); // ���ڶ��������б����������һ����
    return;
}

std::string factor(bool& is_char){
    // ����Ϊ���¶�һ������
    grammar_analysis_result << "this is a factor.\n";
    std::string result , tmp;
    bool idx_type = false;
    if(sy == IDEN){
        std::string Name = id; // ��ֹŪ��������һ����¼������
        getsym();
        if(sy == LZKH){ // ����
            int idx_in_tab = lookup_var(Name);
            if(idx_in_tab == -1){
                error(44); // ����δ����
            }
            else if(tab[idx_in_tab].len == 0){
                error(50); // �ñ���������������
            }
            else{
                if(tab[idx_in_tab].type == chariden) is_char = true;
            }
            getsym();
            if(!find_sym(expression_beg_set , 6)){
                error(22); // δʶ����ȷ�ı��ʽͷ����
                skip(expression_beg_set , 6);
            }
            //���Ϊexpression_beg_set�з���
            tmp = expression(idx_type);// ����Ϊ���ʽ��һ����
            if(idx_type){
                error(43); // �����±겻��Ϊ�ַ�
            }
            else{ // ��������Խ��
                if(str_is_num(tmp)){
                     if(tab[idx_in_tab].len > 0 && str2int(tmp) >= tab[idx_in_tab].len){
                        error(46); // ����Խ��
                     }
                }
            }
            if(sy != RZKH){
                error(25); // ȱ����������
            }
            getsym();
            result = Name + "[" + tmp + "]";
        }
        else if(sy == LPAR){ // ��������
            int idx_in_tab = lookup_func(Name);
            if(idx_in_tab == -1){
                error(41); // ����δ����
            }
            else{
                if(tab[idx_in_tab].type == chariden) is_char = true;
            }
            idname = Name;
            retFuncCall();
            std::string tmp = new_tmp();
            enter_mid_code(tmp , "RET" , RET , "");
            result = tmp;
            getsym();
        }
        else{   // ����
            int idx_in_tab = lookup_var(Name);
            if(idx_in_tab == -1){
                error(44); // ����δ����
            }
            else if(tab[idx_in_tab].len > 0){
                error(51); // �������ֵ�����������
            }
            else{
                if(tab[idx_in_tab].type == chariden) is_char = true;
            }
            result = Name;
        }
    }
    else if(sy == LPAR){   // ��С���ŵı��ʽ
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); // δʶ����ȷ�ı��ʽͷ����
            skip(expression_beg_set , 6);
        }
        //���Ϊexpression_beg_set�з���
        result = expression(idx_type);// ����Ϊ���ʽ��һ����
        if(sy != RPAR){
            error(15); //ȱ����С����
        }
        getsym();
    }
    else if(sy == MINUS){
        std::string x = "0";
        opration op = sub;
        getsym();
        if(sy != UINT){
            error(26); //ȱ������ֵ
        }
        std::string y = id;
        result = new_tmp();
        enter_mid_code(result , x , op , y);
        getsym();
    }
    else if(sy == PLUS){
        getsym();
        if(sy != UINT){
            error(26); //ȱ������ֵ
        }
        result = id;
        getsym();
    }
    else if(sy == UINT){
        result = id;
        getsym();
    }
    else if(sy == CHAR){
        result = id;
        is_char = true;
        getsym();
    }
    else{
        error(27); //���ӽṹ����
    }
    return result;
}

std::string item(bool& is_char){
    //std::cout << "this is an item.\n";
    grammar_analysis_result << "this is an item.\n";
    std::string z , x , y;
    opration op;
    bool place_holder = false; // �������õ�ռλ������֤��ȷ���У�
    x = factor(is_char); // ���Ϊitem_beg_set�е�Ԫ�أ�item��factor��beg_set��ͬ������Ϊfactor����һ������
    while(sy == MULT || sy == DIV){
        is_char = false;
        if(sy == MULT){
            op = mult;
        }
        else if(sy == DIV){
            op = div;
        }
        getsym();
        if(!find_sym(item_beg_set , 6)){
            error(24); // ��ṹ����
            skip(item_beg_set , 6);
        }
        y = factor(place_holder);
        z = new_tmp();
        enter_mid_code(z , x , op , y);
        x = z;
    }
    return x;
}

std::string expression(bool& is_char){
    //���Ϊexpression_beg_set�з���
    //std::cout << "this is an expression.\n";
    grammar_analysis_result << "this is an expression.\n";
    std::string z,x,y;
    opration op = add;
    bool place_holder = false;
    bool flag = false; // ��ʾ����+ - ���У���һ������char
    if(sy == PLUS || sy == MINUS){
        if(sy == MINUS){
            op = sub;
        }
        getsym();
        flag = true;
    }
    if(!find_sym(item_beg_set , 6)){
        error(23); // ���ʽ�ṹ����
        skip(item_beg_set , 6);
    }
    if(op == sub){
        x = "0";
        y = item(place_holder);
        z = new_tmp();
        enter_mid_code(z , x , op , y);
        x = z;
    }
    else{
        if(flag){
            is_char = false;
            x = item(place_holder);
        }
        else{
            x = item(is_char);// ���Ϊitem_beg_set�е�Ԫ�أ�����Ϊ�����һ����
        }

    }
    while(sy == PLUS || sy == MINUS){
        is_char = false;
        if(sy == MINUS){
            op = sub;
        }
        else if(sy == PLUS){
            op = add;
        }
        getsym();
        if(!find_sym(item_beg_set , 6)){
            error(23); // ���ʽ�ṹ����
            skip(item_beg_set , 6);
        }
        y = item(place_holder);// ���Ϊitem_beg_set�е�Ԫ�أ�����Ϊ�����һ����
        z = new_tmp();
        enter_mid_code(z , x , op , y);
        x = z;
    }
    return x;// ����Ϊ���ʽ��һ����
}

void condition(std::string labels){
    // ���Ϊ�������ַ�
    grammar_analysis_result << "this is a condition.\n";
    bool left_is_char = false;
    bool right_is_char = false;
    std::string left , right;
    opration relation;
    if(!find_sym(expression_beg_set , 6)){
        error(22); // δʶ����ȷ�ı��ʽͷ����
        skip(expression_beg_set , 6);
    }
    //���Ϊexpression_beg_set�з���
    left = expression(left_is_char);// ����Ϊ���ʽ��һ����
    if(find_sym(relationOp , 6)){
        relation = getNegRelation(sy);
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); // δʶ����ȷ�ı��ʽͷ����
            skip(expression_beg_set , 6);
        }
        right = expression(right_is_char);
        if(left_is_char ^ right_is_char){
            error(54); // �������ʽ�����������Ͳ�ƥ��
        }
        enter_mid_code_condition(relation , labels , left , right);
    }
    else{
        enter_mid_code_condition(beqz , labels , left , "");
    }
    if(sy != RPAR){
        error(15); //ȱ����С����
    }

}

void ifStatement(){
    // ���Ϊif
    grammar_analysis_result << "this is a condition statement.\n";
    std::string else_label;
    else_label = create_if_else_label();
    getsym();
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != RPAR){
        condition(else_label); // ���Ϊ�������ַ�������Ϊ��С����
    }
    else{
        error(20); // ����ȱʧ
    }
    getsym();
    if(!find_sym(statement_beg_set , 9)){
        error(21); // δʶ����ȷ�����ͷ
        skip(statement_beg_set , 9);
    }
    // ���Ϊstatement_beg_set�еķ���
    statement();// ����Ϊstatement_end_set�еķ���
    enter_mid_code_only(else_label+":" , "" , label , "");
    std::cout << else_label << ":\n";
    mid_code_result << else_label << ":\n";
}

void whileStatement(){
    grammar_analysis_result << "this is a while loop statement.\n";
    std::string while_beg_label,while_end_label;
    while_beg_label = create_while_beg();
    while_end_label = create_while_end();
    enter_mid_code_only(while_beg_label + ":" , "" , label , "");
    std::cout << while_beg_label << ":\n";
    mid_code_result << while_beg_label << ":\n";
    getsym();
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != RPAR){
        condition(while_end_label); // ���Ϊ�������ַ�������Ϊ��С����
    }
    else{
        error(20); // ����ȱʧ
    }
    getsym();
    if(!find_sym(statement_beg_set , 9)){
        error(21); // δʶ����ȷ�����ͷ
        skip(statement_beg_set , 9);
    }
    // ���Ϊstatement_beg_set�еķ���
    statement();// ����Ϊstatement_end_set�еķ���
    enter_mid_code_only(while_beg_label , "" , j , "");  // ֻ������������ĸ�ʽ��¼�������j while_beg_label
    std::cout << "j " << while_beg_label << "\n";
    mid_code_result <<"j " << while_beg_label << "\n";
    enter_mid_code_only(while_end_label + ":" , "" , label , "");
    std::cout << while_end_label << ":\n";
    mid_code_result << while_end_label << ":\n";
}

void read_mid_code(){
    //�������ɶ�ָ����м���룬����������
    int idx_in_tab = lookup_var(id);
    if(idx_in_tab == -1){
        error(44); // ����δ����
    }
    else if(tab[idx_in_tab].obj == constobj){
        error(52); //����Ϊ������ֵ
    }
    else{
        enter_mid_code_only(id , "" , read , "");
        print_var_mid();
    }

}

void scanfStatement(){
    grammar_analysis_result << "this is a read statement.\n";
    getsym();
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != IDEN){
        error(2); //��ʶ��ȱʧ
        skip(statement_beg_set , 9);
        return;
    }
    read_mid_code();
    getsym();
    while(sy == COMMA){
        getsym();
        if(sy != IDEN){
            error(2); //��ʶ��ȱʧ
            skip(statement_beg_set , 9);
            return;
        }
        read_mid_code();
        getsym();
    }
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_beg_set , 9);
    }
    return; // ����Ϊ��С����
}

void printfStatement(){
    grammar_analysis_result << "this is a write statement.\n";
    bool placeholder = false;
    std::string write_text;
    getsym();
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != STRING && !find_sym(expression_beg_set , 6)){
        error(28); // д������ݴ���
        skip(statement_beg_set , 9);
        return;
    }
    if(sy == STRING){
        string_tab[str_idx ++] = id;               // ���ַ�����¼���ַ�����
        enter_mid_code_only(id , "" , write , "");
        print_var_mid();
        getsym();
        if(sy == COMMA){
            getsym();
        }
    }
    if(find_sym(expression_beg_set , 6)){
        write_text = expression(placeholder); // ����Ϊ���ʽ��һ����
        enter_mid_code_only(write_text , "" , write , "");
        print_var_mid();
    }
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_beg_set , 9);
        return;
    }
    return;
}

void returnStatement(){
    grammar_analysis_result << "this is a return statement.\n";
    bool is_char = false;
    getsym();
    if(sy == SEMI){
        enter_mid_code_only("" , "" , funcend , ""); // �������������к�����������
        std::cout << opration_type[funcend] << std::endl;
        mid_code_result << opration_type[funcend] << std::endl;
        returnflag = true;    // ��ǰ�����з������
        return;
    }
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(find_sym(expression_beg_set , 6)){
        std::string ret_value;
        ret_value = expression(is_char); // ����Ϊ���ʽ��һ����
        if(is_char){
            return_type = chariden;
        }
        else{
            return_type = intiden;
        }
        enter_mid_code_only(ret_value , "" , ret , "");
        print_var_mid();
    }
    else{
        error(29); // ����ֵ���ʽ����
        skip(statement_end_set , 2);
        return;
    }
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(sy != SEMI){
        //std::cout << sym_type[sy] << ' ' << id << std::endl;
        error(1); // ����βȱ�ٷֺ�
        skip(statement_end_set , 2);
    }
    enter_mid_code_only("" , "" , funcend , ""); // �������������к�����������
    std::cout << opration_type[funcend] << std::endl;
    mid_code_result << opration_type[funcend] << std::endl;
    returnflag = true;    // ��ǰ�����з������
    return;
}

void assignStatement(){
    // ���Ϊ��ֵ�Ż��������� idname����Ҫ����ֵ�ı���
    grammar_analysis_result << "this is an assign statement.\n";
    bool is_char = false;
    bool idx_type = false; // true Ϊ��char ��
    std::string arrName = idname;             // �����ڸ�ֵ��ʼʱ��¼����������֣��������д��к���������Ḳ��idname
    std::string Name;
    std::string value; // �Ҳ�ı��ʽ������
    int idx_in_tab = lookup_var(idname);
    if(idx_in_tab == -1){
        error(44); // ����δ����
    }
    else if(tab[idx_in_tab].obj == constobj){
        error(52); // ����Ϊ������ֵ
    }
    if(sy == LZKH){
        if(tab[idx_in_tab].len == 0){
            error(50); //�ñ���������������
        }
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); //δʶ����ȷ�ı��ʽͷ����
            skip(statement_beg_set , 9);
            return;
        }
        std::string arr_idx;
        arr_idx = expression(idx_type);// ����Ϊ���ʽ��һ����
        if(idx_type){
                error(43); // �����±겻��Ϊ�ַ�
            }
        else{ // ��������Խ��
            if(str_is_num(arr_idx)){
                if(tab[idx_in_tab].len > 0 && str2int(arr_idx) >= tab[idx_in_tab].len){
                    error(46); // ����Խ��
                }
            }
        }
        if(sy != RZKH){
            error(25); //ȱ����������
            skip(statement_beg_set , 9);
            return;
        }
        Name = arrName + "[" + arr_idx + "]"; // �����ڸ�ֵ��ʼʱ��¼�����������arrName���������д��к���������Ḳ��idname
        getsym();
    }
    else{
        if(tab[idx_in_tab].len != 0){
            error(51); //�������ֵ�����������
        }
        Name = idname;
    }
    if(sy != ASSIGN){
        error(30); //ȱ�ٸ�ֵ����
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(!find_sym(expression_beg_set , 6)){
        error(22); //δʶ����ȷ�ı��ʽͷ����
        skip(statement_beg_set , 9);
        return;
    }
    value = expression(is_char);// ����Ϊ���ʽ��һ����
    if((tab[idx_in_tab].type == chariden && !is_char) || (tab[idx_in_tab].type == intiden && is_char)){
        error(53); // ��ֵ���Ͳ���ȷ
    }
    else{
        enter_mid_code(Name , value , assignop , "");
    }
    return;
}

void paraCheck(bool is_char){
    if(tab[paraidx].obj != paraobj){
        error(47); //�����������
    }
    else{
        if(is_char && tab[paraidx].type != chariden){
            error(48); // �������Ͳ�ƥ��
        }
        else if(!is_char && tab[paraidx].type != intiden){
            error(48); // �������Ͳ�ƥ��
        }
        paraidx ++;
    }
}

void valueParaList(int idx_in_tab){
    std::string paras[PARAMAX]; // �������ʽ���� ����Ҫ���ڵ���push�м��б��ʽ�������ʹ��push�ܹ�ͳһ�����
    int i = 0; // �������ʽ�����±�����
    bool is_char = false;
    paras[i ++] = expression(is_char);// ����Ϊ���ʽ��һ����
    paraidx = idx_in_tab + 1;
    paraCheck(is_char);
    while(sy == COMMA){
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); //δʶ����ȷ�ı��ʽͷ����
            skip(statement_beg_set , 9);
            return;
        }
        is_char = false;
        paras[i ++] = expression(is_char);// ����Ϊ���ʽ��һ����
        paraCheck(is_char);
    }
    if(tab[paraidx].obj == paraobj){
        error(49); //�����������
    }
    for(int j = 0 ; j < i ; j ++){       // push para �����˳���޸ģ����м��б��ʽ�ļ��㣬��ͳһ�����е�push������
        enter_mid_code_only(paras[j] , "" , push , "");
        print_var_mid();
    }
    return; //����Ϊֵ��������һ����
}

void voidFuncCall(){
    // ���һ����С����
    std::string fname = idname;
    int idx_in_tab = lookup_func(idname); // idx_in_tabΪ�ú�����tab�е�λ�ã�������Ϣ������һλ���ǲ�����
    grammar_analysis_result << "this is a void function call.\n";
    getsym();
    if(sy == RPAR){
        if(tab[idx_in_tab + 1].obj == paraobj){
            error(49); // �����������
        }
        enter_mid_code_only(idname , "" , call , "");
        print_var_mid();
        return;//�޲�������
    }
    if(!find_sym(expression_beg_set , 6)){
        error(22); //δʶ����ȷ�ı��ʽͷ����
        skip(statement_beg_set , 9);
        return;
    }
    valueParaList(idx_in_tab); // ����Ǳ��ʽͷ����//����Ϊֵ��������һ����
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_end_set , 2);
    }
    enter_mid_code_only(fname , "" , call , ""); // �޸� ���ú����ĺ�������Ҫ���棬��ֵ�α��к�������ʱ���ܱ��޸�
    print_var_mid();
    return; // ����Ϊ��С����
}

void retFuncCall(){
    // ���һ����С����
    std::string fname = idname;
    int idx_in_tab = lookup_func(idname);
    grammar_analysis_result << "this is a return valueftion call.\n";
    getsym();
    if(sy == RPAR){
        if(tab[idx_in_tab + 1].obj == paraobj){
            error(49); // �����������
        }
        enter_mid_code_only(idname , "" , call , "");
        print_var_mid();
        return;//�޲�������
    }
    if(!find_sym(expression_beg_set , 6)){
        error(22); //δʶ����ȷ�ı��ʽͷ����
        skip(statement_beg_set , 9);
        return;
    }

    valueParaList(idx_in_tab); // ����Ǳ��ʽͷ����//����Ϊֵ��������һ����
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_end_set , 2);
    }
    enter_mid_code_only(fname , "" , call , "");// �޸� ���ú����ĺ�������Ҫ���棬��ֵ�α��к�������ʱ���ܱ��޸�
    print_var_mid();
    return; // ����Ϊ��С����
}

void caseTable(bool is_char , std::string target , std::string switch_end){
    // ���Ϊ�������
    grammar_analysis_result << "this is a case table.\n";
    getsym();
    if(sy != CASE){
        error(34); // ȱ��case�ؼ���
        skip(statement_end_set , 2);
        return;
    }
    while(sy == CASE){
        getsym();
        std::string case_end_label = create_case_end_label();
        if(sy == UINT || (sy == PLUS) || (sy == MINUS)){  // ��������
            if(is_char){                              // Ŀ�����ַ�
                error(55); // �������ʹ���
            }
            if(sy == UINT){                            // �޷���
                enter_mid_code_condition(bne , case_end_label , target , id);
            }
            else if(sy == PLUS){ //  +
                getsym();
                if(sy != UINT){                 // ������һ����������
                    error(31); // ����ֵ����
                    skip(statement_end_set , 2);
                    return;
                }
                enter_mid_code_condition(bne , case_end_label , target , id);
            }
            else if(sy == MINUS){
                getsym();
                if(sy != UINT){                    // ������һ����������
                    error(31); // ����ֵ����
                    skip(statement_end_set , 2);
                    return;
                }
                std::string numstr = id;
                enter_mid_code_condition(bne , case_end_label , target , "-" + numstr);
            }
        }
        else if(sy == CHAR){
            if(!is_char){
                error(55); // �������ʹ���
            }
            enter_mid_code_condition(bne , case_end_label , target , id);
        }
        else{
            error(31); // ����ֵ����
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(sy != COLON){
            error(32); // ȱ��ð��
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(!find_sym(statement_beg_set , 9)){
            error(21);//δʶ����ȷ�����ͷ
            skip(statement_end_set , 2);
            return;
        }
        statement(); // ����Ϊ } ��
        getsym();
        enter_mid_code_only(switch_end , "" , j , "");
        std::cout << "j " << switch_end << std::endl;
        mid_code_result << "j " << switch_end << std::endl;
        enter_mid_code_only(case_end_label + ":" , "" , label , "");
        std::cout << case_end_label + ":\n";
        mid_code_result << case_end_label + ":\n";
    }

    return;//����Ϊ}��
}

void switchStatement(){
    grammar_analysis_result << "this is a switch statement.\n";
    bool is_char = false; // ��ʾswitch�ı��ʽ������
    std::string target;
    std::string switch_end = create_switch_end_label();
    getsym();
    if(sy != LPAR){
        error(19); // ȱ����С����
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(!find_sym(expression_beg_set , 6)){
        error(22); //δʶ����ȷ�ı��ʽͷ����
        skip(statement_beg_set , 9);
        return;
    }
    target = expression(is_char);
    if(sy != RPAR){
        error(15); // ȱ����С����
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(sy != LDKH){
        error(16); //ȱ���������
        skip(statement_end_set , 2);
        return;
    }
    caseTable(is_char , target , switch_end);// ���Ϊ�������//����Ϊ}��default;
    if(sy == RDKH){      // ���Ҵ�����Ҳ��Ҫ����һ��switch_end!!
        enter_mid_code_only(switch_end + ":" , "" , label , "");
        std::cout << switch_end + ":\n";
        mid_code_result << switch_end + ":\n";
        return;
    }
    if(sy == DEFAUL){
        getsym();
        if(sy != COLON){
            error(32); // ȱ��ð��
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(!find_sym(statement_beg_set , 9)){
            error(21);//δʶ����ȷ�����ͷ
            skip(statement_end_set , 2);
            return;
        }
        statement();
        getsym(); // ������������һ����
    }

    enter_mid_code_only(switch_end + ":" , "" , label , "");
    std::cout << switch_end + ":\n";
    mid_code_result << switch_end + ":\n";
    return; //����Ϊcase/default��һ����
}

void statement(){
    // ���Ϊstatement_beg_set�еķ���
    grammar_analysis_result << "this is a statement.\n";
    returnflag = false;    // ÿ������һ�������ʱ����false.������һ��������䲻���ڵ�ǰ������������һ���������
    switch(sy){
        case IF:{ //�������
            ifStatement(); // ���Ϊif
            break;
        }
        case WHILE:{ // ѭ�����
            whileStatement();
            break;
        }
        case LDKH:{ // �����
            getsym();
            while(sy != RDKH){
                skip(statement_beg_set , 9);
                statement(); // ����Ϊstatement_end_set�еķ���{,;}
                getsym();
            }
            break;
        }
        case IDEN:{
            // �˴�����Ӧ��Ϊ��������
            idname = id; // ��¼��ǰ��ʶ����
            getsym();
            if(sy == LPAR){//��������
                identyp funcType = find_func(idname);
                if(funcType == notdef){
                    //std::cout << idname << std::endl;
                    error(41); // ����δ����
                }
                else if(funcType == voididen){
                    voidFuncCall();
                }
                else{
                    retFuncCall();
                }
                getsym();
                if(sy != SEMI){
                    //std::cout << sym_type[sy] << ' ' << id << std::endl;
                    error(1); // ����βȱ�ٷֺ�
                    skip(statement_end_set , 2);
                }
            }
            else if(sy == ASSIGN || sy == LZKH){ // ��ֵ
                assignStatement(); // ���Ϊ�ȺŻ���������
                if(sy != SEMI){
                    error(1); // ����βȱ�ٷֺ�
                    skip(statement_end_set , 2);
                }
            }
            else{
                error(30); // ��ֵ���ȱ�ٸ�ֵ����
                skip(statement_beg_set , 9);
            }
            break;
        }
        case SCANF:{ // �����
            scanfStatement();
            getsym();
            if(sy != SEMI){
                error(1); // ����βȱ�ٷֺ�
                skip(statement_end_set , 2);
            }
            break;
        }
        case PRINTF:{ // д���
            printfStatement();
            getsym();
            if(sy != SEMI){
                error(1); // ����βȱ�ٷֺ�
                skip(statement_end_set , 2);
            }
            break;
        }
        case SEMI:{ // �����
            break;
        }
        case SWITCH:{ // ������
            switchStatement();//����Ϊcase/default��һ����
            if(sy != RDKH){
                error(33); // ȱ���Ҵ�����
                skip(statement_end_set , 2);
            }
            break;
        }
        case RETURN:{ // �������
            returnStatement();
            //���ڿ����޷���ֵ�������ں��������д���ֺ�
            break;
        }
        default:{
            break;
        }
    }
    // ����Ϊstatement_end_set�еķ���

    if(!find_sym(statement_end_set , 2)){
        error(18); // δʶ����ȷ������β���ֺŻ��Ҵ�����ȱʧ
        skip(statement_end_set , 2);
    }
    return;
}

void compoundStatement(){
    // ���Ϊ{����һ���ַ�
    grammar_analysis_result << "this is a compound statement.\n";
    while(sy == CONST){
        constDeclare();
    }
    while(sy == INT || sy == CHARKW || sy == CONST){
        //std::cout << "a var declare.\n";
        if(end_of_file) break;
        if(sy == CONST){
            error(7); //�����������λ�ڱ��������뺯������֮ǰ
            skip(compound_beg_set , 11);
            continue;
        }
        else{
            typeiden = sy;
        }
        getsym();
        if(sy != IDEN){
            error(2); //��ʶ��ȱʧ
            skip(vardec_set , 3);
        }
        else{
            idname = id;
            getsym();
        }
        varDeclare(); //����Ϊ������һ����
    }
    if(sy == RDKH) {std::cout << "no statement!\n"; return;}
    if(!find_sym(statementlist_beg_set , 10)){
        error(17); //δʶ����ȷ�������
        skip(statementlist_beg_set , 10);
    }
    if(sy != RDKH){
        // ����д���
        while(sy != RDKH){
            skip(statement_beg_set , 9);
            statement(); //���Ϊstatement_beg_set�еķ���,����Ϊһ�����Ľ�βstatement_end_set
            getsym();
            if(!find_sym(statement_beg_set , 9) && sy != RDKH){
                error(17); //δʶ����ȷ�������
            }
        }
    }
    else{
        //std::cout << "no statement list.\n";
    }

}

void addFunc(){
    this_type = sym2type(typeiden);
    enter_tab(idname , funcobj , this_type , 0 , tabidxidx + 1);  // �ú�����len����ʾ������ŷ���ٸ��ֲ�����
    tabidxidx ++;      // tabidxidx�е�����ָ��ǰ������tab�е�λ�ã��ݹ���ò����Ҫע����ֹ����Ϊtabidxidx����
    tab_idx[tabidxidx] = tabidx - 1;  // �����������ָ��������εĿ�ͷ��enter����tabidx + 1�����������ȥ1
    if(tabidxidx > 1){                            // ����ڶ������Ժ�ĺ���������������Ϊ��һ��������¼��ʱ��������
        tab[tab_idx[tabidxidx - 1]].adr = tmp_var;    // �ҵ���һ�������ڱ��е�λ�ã�Ϊ��adr���¼��ʱ��������
        tab[tab_idx[tabidxidx - 1]].len = var_off; // ��һ��������¼���б�������
    }
    var_off = 0;   // ����һ���º���Ӧ�ý������;ֲ�������ƫ����0
    tmp_var = 0;   // ����ÿһ���º������壬���е���ʱ��������0��ʼ���
    enter_mid_code_only(type2str(this_type) , idname , func , "");
    print_var_mid();
    returnflag = false;  // ÿ����һ���º���������Ҫ�����ر�־���㣬��ʾ��δʶ�𵽷������
}

void funcDeclares(){
    addFunc();
    paraList(); // ���Ϊsy = (
    if(sy != LDKH){
        error(16); //ȱ���������
        skip(func_beg_set , 1);
    }
    getsym();// ��������ŵ���һ����
    compoundStatement();// ���Ϊ{����һ���ַ�,����Ϊ�Ҵ�����
    if(return_type != tab[tab_idx[tabidxidx]].type){
        error(42); // ��������ֵ���ʹ���
    }
    if(!returnflag){                                   // û�з��������Ϻ�����������ֹ�ظ�����
        enter_mid_code_only("" , "" , funcend , "");
        std::cout << opration_type[funcend] << std::endl;
        mid_code_result << opration_type[funcend] << std::endl;
    }

}

void voidFuncDeclare(){
    grammar_analysis_result << "this is a void return function declaration.\n";
    funcDeclares();
}

void returnFuncDeclare(){
    grammar_analysis_result << "this is a return value function declaration.\n";
    funcDeclares();
}

void mainFuncDeclare(){
    //���Ϊ��С����
    grammar_analysis_result << "==========this is a main function.==========\n";
    getsym();
    addFunc();
    if(sy != RPAR){
        error(15); //ȱ����С����
    }
    getsym(); // ���������
    if(sy != LDKH){
        error(16); //ȱ���������
        skip(func_beg_set , 1);
    }
    getsym();// ��������ŵ���һ����
    compoundStatement();// ���Ϊ{����һ���ַ�,����Ϊ�Ҵ�����
    if(return_type != tab[tab_idx[tabidxidx]].type){
        error(42); // ��������ֵ���ʹ���
    }
    tab[tab_idx[tabidxidx]].adr = tmp_var;
    tab[tab_idx[tabidxidx]].len = var_off;
    tabidxidx ++;
    tab_idx[tabidxidx] = tabidx;                  // main����������ҪΪ���һ��tab_idx��¼���ű����λ�á�
    if(mid_code_tab[midcodeidx - 1].op == funcend){   // main������β��return������funcend
        midcodeidx --;  // ע��˴������м��������funcend��Ŀ����벻����jr $ra
    }
    enter_mid_code_only("" , "" , exitprog , "");
    std::cout << opration_type[exitprog] << std::endl;
    mid_code_result << opration_type[exitprog] << std::endl;
}

void grammar(){

    // �﷨��������Ԥ��һ������
    getsym();
    // ����������������λ
    while(sy == CONST){
        constDeclare();
    }
    var_off = 0; // ȫ�ֱ�����ʼλ�ã�����λ�����㡣
    while(true){
        if(end_of_file) break;
        skip(block_beg_set , 4);
        if(sy == CONST){
            error(7); // �����������λ�ڱ�������֮ǰ
            getsym(); // ��Ϊ�˴��Ƕ���const�����Ĵ��󣬹�������ٶ�һ�������ٿ�ʼ����
            skip(block_beg_set , 4);
            continue;
        }
        typeiden = sy; // ��¼��ǰ����/�������������
        getsym();
        funcname = sy; // ���������ͣ�main / ����
        idname = id;   // ��ǰ������ʶ������
        if(sy != IDEN && sy != MAIN){
            error(8); // ��������ʶ�𵽷Ƿ�����
            skip(block_beg_set , 4);
            continue;
        }
        getsym();
        if(!find_sym(var_set , 4)){
            error(8); // ��������ʶ�𵽷Ƿ�����
            skip(block_beg_set , 4);
            continue;
        }
        if(sy == LPAR){
            return_type = voididen;
            break;
        }
        if(typeiden == VOID){
            error(9); // �������Ͳ���Ϊ��
            skip(block_beg_set , 4);
            continue;
        }
        varDeclare();
    }
    // �����һ����������
    if(funcname == MAIN){
        // ����������
        if(typeiden != VOID){
            error(14); // ����������ֵ���ͱ���Ϊvoid����������
        }
        else{
            mainFuncDeclare(); //���Ϊ��С����
        }
    }
    else{
        // �������С����
        if(typeiden != VOID){
            returnFuncDeclare();
        }
        else{
            voidFuncDeclare();
        }
        // �����������Ϊ } LDKH��һ����
        while(true){
            if(end_of_file) break;
            skip(block_beg_set , 4);
            if(sy == CONST){
                error(7); // �����������λ�ڱ��������뺯������֮ǰ
                getsym(); // ��Ϊ�˴��Ƕ���const�����Ĵ��󣬹�������ٶ�һ�������ٿ�ʼ����
                skip(block_beg_set , 4);
                continue;
            }
            typeiden = sy;
            getsym();
            funcname = sy;
            idname = id;
            if(sy != IDEN && sy != MAIN){
                error(8); // ��������ʶ�𵽷Ƿ�����
                skip(block_beg_set , 4);
                continue;
            }
            getsym();
            if(sy != LPAR){
                error(13); // �����������λ�ں�������֮ǰ
                skip(block_beg_set , 4);
                continue;
            }
            return_type = voididen;
            // ������С���ţ���֤������main / ������
            if(funcname == MAIN){
                if(typeiden != VOID){
                    error(14); // ����������ֵ���ͱ���Ϊvoid����������
                }
                else{
                    mainFuncDeclare(); //���Ϊ��С����
                }
                break;
            }
            else {
                if(typeiden != VOID){
                    returnFuncDeclare();
                }
                else{
                    voidFuncDeclare();
                }
            }
        }
    }
    print_table();
    mid_code_output_check();
}

// �﷨�����׶����
void grammar_output(){
    grammar_analysis_result.open("16061160_grammar_result.txt");
    mid_code_result.open("16061160_mid_result.txt");

}
}
