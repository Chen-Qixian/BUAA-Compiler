#include "grammar.h"

namespace compiler{
// 语法分析阶段输出文件定义
std::ofstream grammar_analysis_result;
// 中间代码输出文件定义
//std::ofstream mid_code_result;
// 开始符号集，用于跳读部分
symbol block_beg_set[4] = {CONST , INT , CHARKW , VOID};
symbol compound_beg_set[11] = {IDEN , INT , CHARKW , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH};
symbol statement_beg_set[9] = {IDEN , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH}; //空语句和{语句列}
symbol statementlist_beg_set[10] = {IDEN , IF , SWITCH , WHILE , PRINTF , SCANF , RETURN , SEMI , LDKH , RDKH};
symbol expression_beg_set[6] = {PLUS , MINUS , IDEN , UINT , CHAR , LPAR};
symbol item_beg_set[6] = {IDEN , UINT , CHAR , LPAR , PLUS , MINUS};  // 项和因子开始符号集相同
// 变量定义符号集
symbol var_set[4] = {COMMA , LZKH , SEMI , LPAR};
symbol vardec_set[3] = {COMMA , LZKH , SEMI};
symbol func_beg_set[1] = {LDKH};
symbol relationOp[6] = {EQUAL , NEQ , GREAT , GEQ , LESS , LEQ };
// 语句终止符号集
symbol statement_end_set[2] = {SEMI , RDKH};
// 一些变量定义
symbol typeiden; // 变量定义预读的变量类型
symbol funcname; // 潜在函数名类型（标识符或main）
int var_off; // 变量在该层次下的地址偏移量
identyp this_type; // 当前行定义的数据类型
identyp return_type;
std::string idname; // 标识符名
int paraidx; // 参数位置，在valueParaList 中使用
bool returnflag = false;

std::string new_tmp(){
    return "$T" + int2str(tmp_var ++);
}

void constDeclare(){
    int sign = 1;// 常量定义的符号位正负（1正 -1负）
    int inum = 0;   // 数字常量值

    grammar_analysis_result << "a const declaration statement.\n";
    getsym();
    if(sy != INT && sy != CHARKW){
        error(0); // 缺少类型定义 int 或 char.
        skip(block_beg_set , 4);
        return;  // 跳读无用的符号，直到读到程序块的起始符号返回，此时sy为程序起始符号集里的符号
    }
    this_type = sym2type(sy);// 类型转换并保存用于后续填表
    do{
        getsym();
        if(sy != IDEN){
            error(2); // 标识符缺失
            skip(block_beg_set , 4);
            return;
        }
        idname = id; // string = char[]
        getsym();
        if(sy != ASSIGN){
            error(3); // 缺少赋值符号
            skip(block_beg_set , 4);
            return;
        }
        getsym();
        if(sy == PLUS || sy == MINUS || sy == UINT){ // int const
            if(this_type != intiden){
                error(5); // 声明类型与变量类型不匹配
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
                error(4); // 缺少常数数值
                skip(block_beg_set , 4);
                return;
            }
            inum = sign * std::atoi(id);// 根据sign计算常数值，填表
            enter_mid_code_only("int" , idname , con , int2str(inum));
            print_const_mid();
        }
        else if(sy == CHAR){             // char const
            if(this_type != chariden){
                error(5); // 声明类型与变量类型不匹配
                skip(block_beg_set , 4);
                return;
            }
            inum = (int) id[1];// 填表
            enter_mid_code_only("char" , idname , con , id );
            print_const_mid();
        }
        else{
            error(6);  // 常量定义不完善
            skip(block_beg_set , 4);
            return;
        }
        getsym();
        enter_tab(idname , constobj , this_type , 0 , inum);
    }while(sy == COMMA);
    if(sy != SEMI){
        error(1); // 语句结尾缺少分号
        skip(block_beg_set , 4);
        return;
    }
    //退出函数后需要预读一个符号到;的下一个符号
    getsym();
    return;
}

void arrayDeclare(){
    if(sy == LZKH){
        getsym();
        if(sy != UINT){
            error(10); // 数组索引值必须为整数
            skip(block_beg_set , 4);
            return;
        }
        else{
            int arr_length = std::atoi(id);// 此处填表
            enter_tab(idname , arrobj , this_type , arr_length , var_off);
            enter_mid_code_only(type2str(this_type) , idname , arr , "");
            print_var_mid();
            var_off += arr_length;
            getsym();
            if(sy != RZKH){
                error(11); // 缺少右中括号
                skip(block_beg_set , 4);
                return;
            }
            else{
                getsym();
            }
        }
        if(sy != COMMA && sy != SEMI){
            error(12); // 缺少逗号或分号
            skip(block_beg_set , 4);
            return;
        }
    }
    else{
        enter_tab(idname , varobj , this_type , 0 , var_off);
        var_off ++;// 将从grammar()中读到的变量填表
        enter_mid_code_only(type2str(this_type) , idname , var , "");
        print_var_mid();
    }
}

void varDeclare(){
    // 入口处应该是读到,;[三个分界符
    grammar_analysis_result << "variable declaration statement.\n";
    // 进入判断是否[确定是否数组，否则记录为变量
    this_type = sym2type(typeiden);
    arrayDeclare();
    while(sy == COMMA){
        getsym();
        if(sy != IDEN){
            error(2); //标识符缺失
            skip(block_beg_set , 4);
            return;
        }
        idname = id;
        getsym();
        if(!find_sym(vardec_set , 3)){
            error(8); // 变量定义识别到非法符号
            skip(block_beg_set , 4);
            continue;
        }
        arrayDeclare();
    }
    if(sy != SEMI){
        error(1); // 语句结尾缺少分号
        skip(block_beg_set , 4);
        return;
    }
    getsym();// 出口为；的下一符号
    return;
}

void addPara(){
    enter_tab(idname , paraobj , this_type , 0 , var_off);
    var_off ++;
    enter_mid_code_only(type2str(this_type) , idname , para , "");
    print_var_mid();
}

void paraList(){
    // 入口为左小括号
    grammar_analysis_result << "this is a parameter list.\n";
    getsym();
    if(sy == RPAR){
        // 右括号返回，无参数
        getsym(); // 出口为）的下一符号
        return;
    }
    if(sy != INT && sy != CHARKW){
        error(0); // 缺少类型定义 int 或 char.
        skip(func_beg_set , 1);
        return;
    }
    this_type = sym2type(sy);
    getsym(); // 读参数名
    if(sy != IDEN){
        error(2);//标识符缺失
        skip(func_beg_set , 1);
        return;
    }
    idname = id;
    addPara(); // 在前文定义
    getsym();
    while(sy == COMMA){
        getsym();
        if(sy != INT && sy != CHARKW){
            error(0); // 缺少类型定义 int 或 char.
            skip(func_beg_set , 1);
            return;
        }
        this_type = sym2type(sy);
        getsym();
        if(sy != IDEN){
            error(2);//标识符缺失
            skip(func_beg_set , 1);
            return;
        }
        idname = id;
        addPara();
        getsym();
    }
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(func_beg_set , 1);
        return;
    }
    getsym(); // 出口读到参数列表）结束后的下一符号
    return;
}

std::string factor(bool& is_char){
    // 出口为向下读一个符号
    grammar_analysis_result << "this is a factor.\n";
    std::string result , tmp;
    bool idx_type = false;
    if(sy == IDEN){
        std::string Name = id; // 防止弄混新引入一个记录变量名
        getsym();
        if(sy == LZKH){ // 数组
            int idx_in_tab = lookup_var(Name);
            if(idx_in_tab == -1){
                error(44); // 变量未定义
            }
            else if(tab[idx_in_tab].len == 0){
                error(50); // 该变量不是数组类型
            }
            else{
                if(tab[idx_in_tab].type == chariden) is_char = true;
            }
            getsym();
            if(!find_sym(expression_beg_set , 6)){
                error(22); // 未识别到正确的表达式头符号
                skip(expression_beg_set , 6);
            }
            //入口为expression_beg_set中符号
            tmp = expression(idx_type);// 出口为表达式下一符号
            if(idx_type){
                error(43); // 数组下标不能为字符
            }
            else{ // 处理数组越界
                if(str_is_num(tmp)){
                     if(tab[idx_in_tab].len > 0 && str2int(tmp) >= tab[idx_in_tab].len){
                        error(46); // 数组越界
                     }
                }
            }
            if(sy != RZKH){
                error(25); // 缺少右中括号
            }
            getsym();
            result = Name + "[" + tmp + "]";
        }
        else if(sy == LPAR){ // 函数调用
            int idx_in_tab = lookup_func(Name);
            if(idx_in_tab == -1){
                error(41); // 函数未定义
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
        else{   // 变量
            int idx_in_tab = lookup_var(Name);
            if(idx_in_tab == -1){
                error(44); // 变量未定义
            }
            else if(tab[idx_in_tab].len > 0){
                error(51); // 单独出现的数组名错误
            }
            else{
                if(tab[idx_in_tab].type == chariden) is_char = true;
            }
            result = Name;
        }
    }
    else if(sy == LPAR){   // 带小括号的表达式
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); // 未识别到正确的表达式头符号
            skip(expression_beg_set , 6);
        }
        //入口为expression_beg_set中符号
        result = expression(idx_type);// 出口为表达式下一符号
        if(sy != RPAR){
            error(15); //缺少右小括号
        }
        getsym();
    }
    else if(sy == MINUS){
        std::string x = "0";
        opration op = sub;
        getsym();
        if(sy != UINT){
            error(26); //缺少整数值
        }
        std::string y = id;
        result = new_tmp();
        enter_mid_code(result , x , op , y);
        getsym();
    }
    else if(sy == PLUS){
        getsym();
        if(sy != UINT){
            error(26); //缺少整数值
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
        error(27); //因子结构错误
    }
    return result;
}

std::string item(bool& is_char){
    //std::cout << "this is an item.\n";
    grammar_analysis_result << "this is an item.\n";
    std::string z , x , y;
    opration op;
    bool place_holder = false; // 函数调用的占位符（保证正确运行）
    x = factor(is_char); // 入口为item_beg_set中的元素，item和factor的beg_set相同；出口为factor的下一个符号
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
            error(24); // 项结构错误
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
    //入口为expression_beg_set中符号
    //std::cout << "this is an expression.\n";
    grammar_analysis_result << "this is an expression.\n";
    std::string z,x,y;
    opration op = add;
    bool place_holder = false;
    bool flag = false; // 表示有无+ - 若有，则一定不是char
    if(sy == PLUS || sy == MINUS){
        if(sy == MINUS){
            op = sub;
        }
        getsym();
        flag = true;
    }
    if(!find_sym(item_beg_set , 6)){
        error(23); // 表达式结构错误
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
            x = item(is_char);// 入口为item_beg_set中的元素，出口为项的下一符号
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
            error(23); // 表达式结构错误
            skip(item_beg_set , 6);
        }
        y = item(place_holder);// 入口为item_beg_set中的元素，出口为项的下一符号
        z = new_tmp();
        enter_mid_code(z , x , op , y);
        x = z;
    }
    return x;// 出口为表达式下一符号
}

void condition(std::string labels){
    // 入口为条件首字符
    grammar_analysis_result << "this is a condition.\n";
    bool left_is_char = false;
    bool right_is_char = false;
    std::string left , right;
    opration relation;
    if(!find_sym(expression_beg_set , 6)){
        error(22); // 未识别到正确的表达式头符号
        skip(expression_beg_set , 6);
    }
    //入口为expression_beg_set中符号
    left = expression(left_is_char);// 出口为表达式下一符号
    if(find_sym(relationOp , 6)){
        relation = getNegRelation(sy);
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); // 未识别到正确的表达式头符号
            skip(expression_beg_set , 6);
        }
        right = expression(right_is_char);
        if(left_is_char ^ right_is_char){
            error(54); // 条件表达式左右两侧类型不匹配
        }
        enter_mid_code_condition(relation , labels , left , right);
    }
    else{
        enter_mid_code_condition(beqz , labels , left , "");
    }
    if(sy != RPAR){
        error(15); //缺少右小括号
    }

}

void ifStatement(){
    // 入口为if
    grammar_analysis_result << "this is a condition statement.\n";
    std::string else_label;
    else_label = create_if_else_label();
    getsym();
    if(sy != LPAR){
        error(19); // 缺少左小括号
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != RPAR){
        condition(else_label); // 入口为条件首字符，出口为右小括号
    }
    else{
        error(20); // 条件缺失
    }
    getsym();
    if(!find_sym(statement_beg_set , 9)){
        error(21); // 未识别到正确的语句头
        skip(statement_beg_set , 9);
    }
    // 入口为statement_beg_set中的符号
    statement();// 出口为statement_end_set中的符号
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
        error(19); // 缺少左小括号
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != RPAR){
        condition(while_end_label); // 入口为条件首字符，出口为右小括号
    }
    else{
        error(20); // 条件缺失
    }
    getsym();
    if(!find_sym(statement_beg_set , 9)){
        error(21); // 未识别到正确的语句头
        skip(statement_beg_set , 9);
    }
    // 入口为statement_beg_set中的符号
    statement();// 出口为statement_end_set中的符号
    enter_mid_code_only(while_beg_label , "" , j , "");  // 只是用这个函数的格式登录与输出：j while_beg_label
    std::cout << "j " << while_beg_label << "\n";
    mid_code_result <<"j " << while_beg_label << "\n";
    enter_mid_code_only(while_end_label + ":" , "" , label , "");
    std::cout << while_end_label << ":\n";
    mid_code_result << while_end_label << ":\n";
}

void read_mid_code(){
    //用于生成读指令的中间代码，输出，查表功能
    int idx_in_tab = lookup_var(id);
    if(idx_in_tab == -1){
        error(44); // 变量未定义
    }
    else if(tab[idx_in_tab].obj == constobj){
        error(52); //不能为常量赋值
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
        error(19); // 缺少左小括号
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != IDEN){
        error(2); //标识符缺失
        skip(statement_beg_set , 9);
        return;
    }
    read_mid_code();
    getsym();
    while(sy == COMMA){
        getsym();
        if(sy != IDEN){
            error(2); //标识符缺失
            skip(statement_beg_set , 9);
            return;
        }
        read_mid_code();
        getsym();
    }
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(statement_beg_set , 9);
    }
    return; // 出口为右小括号
}

void printfStatement(){
    grammar_analysis_result << "this is a write statement.\n";
    bool placeholder = false;
    std::string write_text;
    getsym();
    if(sy != LPAR){
        error(19); // 缺少左小括号
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(sy != STRING && !find_sym(expression_beg_set , 6)){
        error(28); // 写语句内容错误
        skip(statement_beg_set , 9);
        return;
    }
    if(sy == STRING){
        string_tab[str_idx ++] = id;               // 将字符串登录至字符串表
        enter_mid_code_only(id , "" , write , "");
        print_var_mid();
        getsym();
        if(sy == COMMA){
            getsym();
        }
    }
    if(find_sym(expression_beg_set , 6)){
        write_text = expression(placeholder); // 出口为表达式下一符号
        enter_mid_code_only(write_text , "" , write , "");
        print_var_mid();
    }
    if(sy != RPAR){
        error(15); // 缺少右小括号
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
        enter_mid_code_only("" , "" , funcend , ""); // 返回语句结束后即有函数结束返回
        std::cout << opration_type[funcend] << std::endl;
        mid_code_result << opration_type[funcend] << std::endl;
        returnflag = true;    // 当前函数有返回语句
        return;
    }
    if(sy != LPAR){
        error(19); // 缺少左小括号
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(find_sym(expression_beg_set , 6)){
        std::string ret_value;
        ret_value = expression(is_char); // 出口为表达式下一符号
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
        error(29); // 返回值表达式错误
        skip(statement_end_set , 2);
        return;
    }
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(sy != SEMI){
        //std::cout << sym_type[sy] << ' ' << id << std::endl;
        error(1); // 语句结尾缺少分号
        skip(statement_end_set , 2);
    }
    enter_mid_code_only("" , "" , funcend , ""); // 返回语句结束后即有函数结束返回
    std::cout << opration_type[funcend] << std::endl;
    mid_code_result << opration_type[funcend] << std::endl;
    returnflag = true;    // 当前函数有返回语句
    return;
}

void assignStatement(){
    // 入口为赋值号或左中括号 idname即是要被赋值的变量
    grammar_analysis_result << "this is an assign statement.\n";
    bool is_char = false;
    bool idx_type = false; // true 为是char 型
    std::string arrName = idname;             // 必须在赋值开始时记录左侧数组名字，若索引中带有函数调用则会覆盖idname
    std::string Name;
    std::string value; // 右侧的表达式计算结果
    int idx_in_tab = lookup_var(idname);
    if(idx_in_tab == -1){
        error(44); // 变量未定义
    }
    else if(tab[idx_in_tab].obj == constobj){
        error(52); // 不能为常量赋值
    }
    if(sy == LZKH){
        if(tab[idx_in_tab].len == 0){
            error(50); //该变量不是数组类型
        }
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); //未识别到正确的表达式头符号
            skip(statement_beg_set , 9);
            return;
        }
        std::string arr_idx;
        arr_idx = expression(idx_type);// 出口为表达式下一符号
        if(idx_type){
                error(43); // 数组下标不能为字符
            }
        else{ // 处理数组越界
            if(str_is_num(arr_idx)){
                if(tab[idx_in_tab].len > 0 && str2int(arr_idx) >= tab[idx_in_tab].len){
                    error(46); // 数组越界
                }
            }
        }
        if(sy != RZKH){
            error(25); //缺少右中括号
            skip(statement_beg_set , 9);
            return;
        }
        Name = arrName + "[" + arr_idx + "]"; // 必须在赋值开始时记录左侧数组名字arrName，若索引中带有函数调用则会覆盖idname
        getsym();
    }
    else{
        if(tab[idx_in_tab].len != 0){
            error(51); //单独出现的数组名错误
        }
        Name = idname;
    }
    if(sy != ASSIGN){
        error(30); //缺少赋值符号
        skip(statement_beg_set , 9);
        return;
    }
    getsym();
    if(!find_sym(expression_beg_set , 6)){
        error(22); //未识别到正确的表达式头符号
        skip(statement_beg_set , 9);
        return;
    }
    value = expression(is_char);// 出口为表达式下一符号
    if((tab[idx_in_tab].type == chariden && !is_char) || (tab[idx_in_tab].type == intiden && is_char)){
        error(53); // 赋值类型不正确
    }
    else{
        enter_mid_code(Name , value , assignop , "");
    }
    return;
}

void paraCheck(bool is_char){
    if(tab[paraidx].obj != paraobj){
        error(47); //传入参数过多
    }
    else{
        if(is_char && tab[paraidx].type != chariden){
            error(48); // 参数类型不匹配
        }
        else if(!is_char && tab[paraidx].type != intiden){
            error(48); // 参数类型不匹配
        }
        paraidx ++;
    }
}

void valueParaList(int idx_in_tab){
    std::string paras[PARAMAX]; // 参数表达式数组 （主要用于调整push中间有表达式的情况，使得push能够统一输出）
    int i = 0; // 参数表达式数组下标索引
    bool is_char = false;
    paras[i ++] = expression(is_char);// 出口为表达式下一符号
    paraidx = idx_in_tab + 1;
    paraCheck(is_char);
    while(sy == COMMA){
        getsym();
        if(!find_sym(expression_beg_set , 6)){
            error(22); //未识别到正确的表达式头符号
            skip(statement_beg_set , 9);
            return;
        }
        is_char = false;
        paras[i ++] = expression(is_char);// 出口为表达式下一符号
        paraCheck(is_char);
    }
    if(tab[paraidx].obj == paraobj){
        error(49); //传入参数过少
    }
    for(int j = 0 ; j < i ; j ++){       // push para 的输出顺序修改：若中间有表达式的计算，则统一将所有的push最后输出
        enter_mid_code_only(paras[j] , "" , push , "");
        print_var_mid();
    }
    return; //出口为值参数表下一符号
}

void voidFuncCall(){
    // 入口一个左小括号
    std::string fname = idname;
    int idx_in_tab = lookup_func(idname); // idx_in_tab为该函数在tab中的位置（函数信息），下一位才是参数。
    grammar_analysis_result << "this is a void function call.\n";
    getsym();
    if(sy == RPAR){
        if(tab[idx_in_tab + 1].obj == paraobj){
            error(49); // 传入参数过少
        }
        enter_mid_code_only(idname , "" , call , "");
        print_var_mid();
        return;//无参数传递
    }
    if(!find_sym(expression_beg_set , 6)){
        error(22); //未识别到正确的表达式头符号
        skip(statement_beg_set , 9);
        return;
    }
    valueParaList(idx_in_tab); // 入口是表达式头符号//出口为值参数表下一符号
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(statement_end_set , 2);
    }
    enter_mid_code_only(fname , "" , call , ""); // 修改 调用函数的函数名需要保存，在值参表中函数调用时可能被修改
    print_var_mid();
    return; // 出口为右小括号
}

void retFuncCall(){
    // 入口一个左小括号
    std::string fname = idname;
    int idx_in_tab = lookup_func(idname);
    grammar_analysis_result << "this is a return valueftion call.\n";
    getsym();
    if(sy == RPAR){
        if(tab[idx_in_tab + 1].obj == paraobj){
            error(49); // 传入参数过少
        }
        enter_mid_code_only(idname , "" , call , "");
        print_var_mid();
        return;//无参数传递
    }
    if(!find_sym(expression_beg_set , 6)){
        error(22); //未识别到正确的表达式头符号
        skip(statement_beg_set , 9);
        return;
    }

    valueParaList(idx_in_tab); // 入口是表达式头符号//出口为值参数表下一符号
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(statement_end_set , 2);
    }
    enter_mid_code_only(fname , "" , call , "");// 修改 调用函数的函数名需要保存，在值参表中函数调用时可能被修改
    print_var_mid();
    return; // 出口为右小括号
}

void caseTable(bool is_char , std::string target , std::string switch_end){
    // 入口为左大括号
    grammar_analysis_result << "this is a case table.\n";
    getsym();
    if(sy != CASE){
        error(34); // 缺少case关键字
        skip(statement_end_set , 2);
        return;
    }
    while(sy == CASE){
        getsym();
        std::string case_end_label = create_case_end_label();
        if(sy == UINT || (sy == PLUS) || (sy == MINUS)){  // 整数处理
            if(is_char){                              // 目标是字符
                error(55); // 常量类型错误
            }
            if(sy == UINT){                            // 无符号
                enter_mid_code_condition(bne , case_end_label , target , id);
            }
            else if(sy == PLUS){ //  +
                getsym();
                if(sy != UINT){                 // 读入下一个不是整数
                    error(31); // 常量值错误
                    skip(statement_end_set , 2);
                    return;
                }
                enter_mid_code_condition(bne , case_end_label , target , id);
            }
            else if(sy == MINUS){
                getsym();
                if(sy != UINT){                    // 读入下一个不是整数
                    error(31); // 常量值错误
                    skip(statement_end_set , 2);
                    return;
                }
                std::string numstr = id;
                enter_mid_code_condition(bne , case_end_label , target , "-" + numstr);
            }
        }
        else if(sy == CHAR){
            if(!is_char){
                error(55); // 常量类型错误
            }
            enter_mid_code_condition(bne , case_end_label , target , id);
        }
        else{
            error(31); // 常量值错误
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(sy != COLON){
            error(32); // 缺少冒号
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(!find_sym(statement_beg_set , 9)){
            error(21);//未识别到正确的语句头
            skip(statement_end_set , 2);
            return;
        }
        statement(); // 出口为 } ；
        getsym();
        enter_mid_code_only(switch_end , "" , j , "");
        std::cout << "j " << switch_end << std::endl;
        mid_code_result << "j " << switch_end << std::endl;
        enter_mid_code_only(case_end_label + ":" , "" , label , "");
        std::cout << case_end_label + ":\n";
        mid_code_result << case_end_label + ":\n";
    }

    return;//出口为}或；
}

void switchStatement(){
    grammar_analysis_result << "this is a switch statement.\n";
    bool is_char = false; // 表示switch的表达式的类型
    std::string target;
    std::string switch_end = create_switch_end_label();
    getsym();
    if(sy != LPAR){
        error(19); // 缺少左小括号
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(!find_sym(expression_beg_set , 6)){
        error(22); //未识别到正确的表达式头符号
        skip(statement_beg_set , 9);
        return;
    }
    target = expression(is_char);
    if(sy != RPAR){
        error(15); // 缺少右小括号
        skip(statement_end_set , 2);
        return;
    }
    getsym();
    if(sy != LDKH){
        error(16); //缺少左大括号
        skip(statement_end_set , 2);
        return;
    }
    caseTable(is_char , target , switch_end);// 入口为左大括号//出口为}或default;
    if(sy == RDKH){      // 是右大括号也需要增加一个switch_end!!
        enter_mid_code_only(switch_end + ":" , "" , label , "");
        std::cout << switch_end + ":\n";
        mid_code_result << switch_end + ":\n";
        return;
    }
    if(sy == DEFAUL){
        getsym();
        if(sy != COLON){
            error(32); // 缺少冒号
            skip(statement_end_set , 2);
            return;
        }
        getsym();
        if(!find_sym(statement_beg_set , 9)){
            error(21);//未识别到正确的语句头
            skip(statement_end_set , 2);
            return;
        }
        statement();
        getsym(); // 读语句结束的下一符号
    }

    enter_mid_code_only(switch_end + ":" , "" , label , "");
    std::cout << switch_end + ":\n";
    mid_code_result << switch_end + ":\n";
    return; //出口为case/default下一符号
}

void statement(){
    // 入口为statement_beg_set中的符号
    grammar_analysis_result << "this is a statement.\n";
    returnflag = false;    // 每当处理一条新语句时，置false.表明上一条返回语句不属于当前函数定义的最后一条返回语句
    switch(sy){
        case IF:{ //条件语句
            ifStatement(); // 入口为if
            break;
        }
        case WHILE:{ // 循环语句
            whileStatement();
            break;
        }
        case LDKH:{ // 语句列
            getsym();
            while(sy != RDKH){
                skip(statement_beg_set , 9);
                statement(); // 出口为statement_end_set中的符号{,;}
                getsym();
            }
            break;
        }
        case IDEN:{
            // 此处后续应改为填表、查表处理
            idname = id; // 记录当前标识符名
            getsym();
            if(sy == LPAR){//函数调用
                identyp funcType = find_func(idname);
                if(funcType == notdef){
                    //std::cout << idname << std::endl;
                    error(41); // 函数未定义
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
                    error(1); // 语句结尾缺少分号
                    skip(statement_end_set , 2);
                }
            }
            else if(sy == ASSIGN || sy == LZKH){ // 赋值
                assignStatement(); // 入口为等号或左中括号
                if(sy != SEMI){
                    error(1); // 语句结尾缺少分号
                    skip(statement_end_set , 2);
                }
            }
            else{
                error(30); // 赋值语句缺少赋值符号
                skip(statement_beg_set , 9);
            }
            break;
        }
        case SCANF:{ // 读语句
            scanfStatement();
            getsym();
            if(sy != SEMI){
                error(1); // 语句结尾缺少分号
                skip(statement_end_set , 2);
            }
            break;
        }
        case PRINTF:{ // 写语句
            printfStatement();
            getsym();
            if(sy != SEMI){
                error(1); // 语句结尾缺少分号
                skip(statement_end_set , 2);
            }
            break;
        }
        case SEMI:{ // 空语句
            break;
        }
        case SWITCH:{ // 情况语句
            switchStatement();//出口为case/default下一符号
            if(sy != RDKH){
                error(33); // 缺少右大括号
                skip(statement_end_set , 2);
            }
            break;
        }
        case RETURN:{ // 返回语句
            returnStatement();
            //由于可能无返回值，所以在函数调用中处理分号
            break;
        }
        default:{
            break;
        }
    }
    // 出口为statement_end_set中的符号

    if(!find_sym(statement_end_set , 2)){
        error(18); // 未识别到正确的语句结尾，分号或右大括号缺失
        skip(statement_end_set , 2);
    }
    return;
}

void compoundStatement(){
    // 入口为{的下一个字符
    grammar_analysis_result << "this is a compound statement.\n";
    while(sy == CONST){
        constDeclare();
    }
    while(sy == INT || sy == CHARKW || sy == CONST){
        //std::cout << "a var declare.\n";
        if(end_of_file) break;
        if(sy == CONST){
            error(7); //常量定义必须位于变量定义与函数定义之前
            skip(compound_beg_set , 11);
            continue;
        }
        else{
            typeiden = sy;
        }
        getsym();
        if(sy != IDEN){
            error(2); //标识符缺失
            skip(vardec_set , 3);
        }
        else{
            idname = id;
            getsym();
        }
        varDeclare(); //出口为；的下一符号
    }
    if(sy == RDKH) {std::cout << "no statement!\n"; return;}
    if(!find_sym(statementlist_beg_set , 10)){
        error(17); //未识别到正确的语句列
        skip(statementlist_beg_set , 10);
    }
    if(sy != RDKH){
        // 语句列处理
        while(sy != RDKH){
            skip(statement_beg_set , 9);
            statement(); //入口为statement_beg_set中的符号,出口为一条语句的结尾statement_end_set
            getsym();
            if(!find_sym(statement_beg_set , 9) && sy != RDKH){
                error(17); //未识别到正确的语句列
            }
        }
    }
    else{
        //std::cout << "no statement list.\n";
    }

}

void addFunc(){
    this_type = sym2type(typeiden);
    enter_tab(idname , funcobj , this_type , 0 , tabidxidx + 1);  // 用函数的len来表示函数中欧多少个局部变量
    tabidxidx ++;      // tabidxidx中的内容指向当前函数在tab中的位置，递归调用查表需要注意终止条件为tabidxidx本身
    tab_idx[tabidxidx] = tabidx - 1;  // 该索引项必须指向函数定义段的开头，enter后有tabidx + 1动作，故需减去1
    if(tabidxidx > 1){                            // 如果第二个及以后的函数声明到来，则为上一个函数记录临时变量个数
        tab[tab_idx[tabidxidx - 1]].adr = tmp_var;    // 找到上一个函数在表中的位置，为其adr域记录临时变量个数
        tab[tab_idx[tabidxidx - 1]].len = var_off; // 上一个函数记录其中变量个数
    }
    var_off = 0;   // 对于一个新函数应该将参数和局部变量的偏移置0
    tmp_var = 0;   // 对于每一个新函数定义，其中的临时变量都从0开始编号
    enter_mid_code_only(type2str(this_type) , idname , func , "");
    print_var_mid();
    returnflag = false;  // 每遇到一个新函数定义需要将返回标志清零，表示还未识别到返回语句
}

void funcDeclares(){
    addFunc();
    paraList(); // 入口为sy = (
    if(sy != LDKH){
        error(16); //缺少左大括号
        skip(func_beg_set , 1);
    }
    getsym();// 读左大括号的下一符号
    compoundStatement();// 入口为{的下一个字符,出口为右大括号
    if(return_type != tab[tab_idx[tabidxidx]].type){
        error(42); // 函数返回值类型错误
    }
    if(!returnflag){                                   // 没有返回语句加上函数结束，防止重复结束
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
    //入口为左小括号
    grammar_analysis_result << "==========this is a main function.==========\n";
    getsym();
    addFunc();
    if(sy != RPAR){
        error(15); //缺少右小括号
    }
    getsym(); // 读左大括号
    if(sy != LDKH){
        error(16); //缺少左大括号
        skip(func_beg_set , 1);
    }
    getsym();// 读左大括号的下一符号
    compoundStatement();// 入口为{的下一个字符,出口为右大括号
    if(return_type != tab[tab_idx[tabidxidx]].type){
        error(42); // 函数返回值类型错误
    }
    tab[tab_idx[tabidxidx]].adr = tmp_var;
    tab[tab_idx[tabidxidx]].len = var_off;
    tabidxidx ++;
    tab_idx[tabidxidx] = tabidx;                  // main函数结束需要为最后一项tab_idx记录符号表结束位置。
    if(mid_code_tab[midcodeidx - 1].op == funcend){   // main函数结尾的return不能有funcend
        midcodeidx --;  // 注意此处尽管中间代码会输出funcend带目标代码不会有jr $ra
    }
    enter_mid_code_only("" , "" , exitprog , "");
    std::cout << opration_type[exitprog] << std::endl;
    mid_code_result << opration_type[exitprog] << std::endl;
}

void grammar(){

    // 语法分析最先预读一个符号
    getsym();
    // 常量定义必须放在首位
    while(sy == CONST){
        constDeclare();
    }
    var_off = 0; // 全局变量起始位置，变量位移清零。
    while(true){
        if(end_of_file) break;
        skip(block_beg_set , 4);
        if(sy == CONST){
            error(7); // 常量定义必须位于变量定义之前
            getsym(); // 因为此处是读到const发生的错误，故需向后再读一个符号再开始跳读
            skip(block_beg_set , 4);
            continue;
        }
        typeiden = sy; // 记录当前变量/函数定义的类型
        getsym();
        funcname = sy; // 函数名类型：main / 名字
        idname = id;   // 当前读到标识符名字
        if(sy != IDEN && sy != MAIN){
            error(8); // 变量定义识别到非法符号
            skip(block_beg_set , 4);
            continue;
        }
        getsym();
        if(!find_sym(var_set , 4)){
            error(8); // 变量定义识别到非法符号
            skip(block_beg_set , 4);
            continue;
        }
        if(sy == LPAR){
            return_type = voididen;
            break;
        }
        if(typeiden == VOID){
            error(9); // 变量类型不能为空
            skip(block_beg_set , 4);
            continue;
        }
        varDeclare();
    }
    // 处理第一个函数定义
    if(funcname == MAIN){
        // 处理主函数
        if(typeiden != VOID){
            error(14); // 主函数返回值类型必须为void，结束分析
        }
        else{
            mainFuncDeclare(); //入口为左小括号
        }
    }
    else{
        // 入口是左小括号
        if(typeiden != VOID){
            returnFuncDeclare();
        }
        else{
            voidFuncDeclare();
        }
        // 函数定义出口为 } LDKH下一符号
        while(true){
            if(end_of_file) break;
            skip(block_beg_set , 4);
            if(sy == CONST){
                error(7); // 常量定义必须位于变量定义与函数定义之前
                getsym(); // 因为此处是读到const发生的错误，故需向后再读一个符号再开始跳读
                skip(block_beg_set , 4);
                continue;
            }
            typeiden = sy;
            getsym();
            funcname = sy;
            idname = id;
            if(sy != IDEN && sy != MAIN){
                error(8); // 变量定义识别到非法符号
                skip(block_beg_set , 4);
                continue;
            }
            getsym();
            if(sy != LPAR){
                error(13); // 变量定义必须位于函数定义之前
                skip(block_beg_set , 4);
                continue;
            }
            return_type = voididen;
            // 读完做小括号，验证函数是main / 函数名
            if(funcname == MAIN){
                if(typeiden != VOID){
                    error(14); // 主函数返回值类型必须为void，结束分析
                }
                else{
                    mainFuncDeclare(); //入口为左小括号
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

// 语法分析阶段输出
void grammar_output(){
    grammar_analysis_result.open("16061160_grammar_result.txt");
    mid_code_result.open("16061160_mid_result.txt");

}
}
