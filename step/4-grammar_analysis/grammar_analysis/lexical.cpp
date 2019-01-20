#include "lexical.h"

namespace compiler{

char getch(){
    return code[cptr ++];
}

void retract(){
    cptr --;
    tptr = 0;
}

void catToken(char c){
    id[tptr ++] = c;
    id[tptr] = '\0';
}

char catNRead(char c){
    catToken(c);
    return getch();
}

void singalToken(char c){
    catToken(c);
	tptr = 0;
}

symbol reserver(char *s){
	if(!strcmp("const",s)) 	return CONST;
	if(!strcmp("int",s)) 	return INT;
	if(!strcmp("char",s))   return CHARKW;
	if(!strcmp("void",s)) 	return VOID;
	if(!strcmp("main",s)) 	return MAIN;
	if(!strcmp("if",s)) 	return IF;
	if(!strcmp("switch",s))	return SWITCH;
	if(!strcmp("case",s)) 	return CASE;
	if(!strcmp("default",s))return DEFAUL;
	if(!strcmp("scanf",s)) 	return SCANF;
	if(!strcmp("printf",s)) return PRINTF;
	if(!strcmp("return",s)) return RETURN;
	if(!strcmp("while",s))  return WHILE;
	return IDEN;
}

void getsym(){
	c = getch();
    memset(id,0,sizeof(id));
	while(c == ' '|| c == '\n' || c == '\t'){
        if(c == '\n'){
            linenum ++;
        }
        c = getch();   // bug fix : need to update c value!
	}

	if(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		c = catNRead(c);
		while(c == '_' || (c >= 'a' && c <= 'z') ||\
             (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
			c = catNRead(c);
		}
        //std::cout << tptr;
		retract();

		sy = reserver(id);
	}
	else if(c >= '0' && c <= '9'){
		c = catNRead(c);
		while(c >= '0' && c <= '9'){
			c = catNRead(c);
		}
		if(id[0] == '0' && tptr > 1){
            error(37); //整数以前导零开始
            end_of_file = true;
            return;
		}
		retract();

		sy = UINT;
	}
	else if(c == '+'){
		singalToken(c);
		sy = PLUS;
	}
	else if(c == '-'){
		singalToken(c);
		sy = MINUS;
	}
	else if(c == '*'){
		singalToken(c);
		sy = MULT;
	}
	else if(c == '/'){
		singalToken(c);
		sy = DIV;
	}
	else if(c == '<'){
		c = catNRead(c);
		if(c == '='){
			singalToken(c);
			sy = LEQ;
		}
		else{
			retract();
			sy = LESS;
		}
	}
	else if(c == '>'){
		c = catNRead(c);
		if(c == '='){
			singalToken(c);
			sy = GEQ;
		}
		else{
			retract();
			sy = GREAT;
		}
	}
	else if(c == '='){
        c = catNRead(c);
        if(c == '='){
            singalToken(c);
            sy = EQUAL;
        }
        else{
            retract();
            sy = ASSIGN;
        }

	}
	else if(c == '!'){
        c = catNRead(c);
        if(c == '='){
            singalToken(c);
            sy = NEQ;
        }
        else{
            error(c);
            end_of_file = true;
            return;
        }
	}
	else if(c == '\''){
        c = catNRead(c);
        if(c == '_' || (c >= 'a' && c <= 'z') ||\
             (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||\
             (c == '+' || c == '-' || c == '*' || c == '/')){
                 ;
		}
		else{
            error(38); // 字符类型出现非法字符
            end_of_file = true;
            return;
		}
		c = catNRead(c);
        if(c != '\''){
            error(39); // 字符类型不以'结尾
            end_of_file = true;
            return;
        }
        else{
            singalToken(c);
            sy = CHAR;
        }
	}
	else if(c =='\"'){
        c = catNRead(c);
        while(c >= 32 && c <= 126 && c != 34){
            c = catNRead(c);
        }
        if(c != '\"'){
            error(36); // 字符串不以“结尾
            end_of_file = true;
            return;
        }
        else{
            singalToken(c);
            sy = STRING;
        }
	}
	else if(c == '('){
		singalToken(c);
		sy = LPAR;
	}
	else if(c == ')'){
		singalToken(c);
		sy = RPAR;
	}
	else if(c == '['){
		singalToken(c);
		sy = LZKH;
	}
	else if(c == ']'){
		singalToken(c);
		sy = RZKH;
	}
	else if(c == '{'){
		singalToken(c);
		sy = LDKH;
	}
	else if(c == '}'){
		singalToken(c);
		sy = RDKH;
	}
	else if(c == ','){
		singalToken(c);
		sy = COMMA;
	}
	else if(c == ';'){
		singalToken(c);
		sy = SEMI;
	}
	else if(c == ':'){
		singalToken(c);
		sy = COLON;
	}
	else if(c == '\0'){
        end_of_file = true;
		return ;
	}
	else{
		error(35); // 词法分析读到非法字符
		end_of_file = true;
		return ;
	}
	return;
}

void lexical_output(){
    std::cout << code;
	lexical_analysis_result.open("16061160_result.txt");
    getsym();
	while(!end_of_file){
		lexical_analysis_result << wordnum ++ << ' ' << sym_type[sy] << ' ' << id << std::endl;
		getsym();
	}
	lexical_analysis_result.close();
}


}
