
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexical.h"

char sym_type[50][100] = { "UINT"  ,"IDEN"  ,"PLUS"  ,"MINUS" ,"MULT"  ,"DIV"   ,\
                           "LESS"  ,"LEQ"   ,"GREAT" ,"GEQ"   ,"EQUAL" ,\
                           "NEQ"   ,"ASSIGN","CHAR"  ,"STRING","COMMA" ,\
                           "SEMI"  ,"COLON" ,"LPAR"  ,"RPAR"  ,"LZKH"  ,\
                           "RZKH"  ,"LDKH"  ,"RDKH"  ,"CONST" ,"INT"   ,\
                           "CHAR"  ,"VOID"  ,"MAIN"  ,"IF"    ,"SWITCH",\
                           "CASE"  ,"DEFAUL","SCANF" ,"PRINTF","RETURN"};

char code[1024 * 1024] = "";
char token[1024] = "";
int cptr = 0;
int tptr = 0;
int ll = 1;

void readFile(){
    FILE *in = fopen("16061160_test.txt","r");

    if(in == NULL){
		printf("Message: %s\n" , strerror(errno));
		return;
	}

	char buf[1024] = "";
	int readnum = 0;
	while(1){
		memset(buf , 0 , sizeof(buf));
		readnum = fread(buf , sizeof(char) , sizeof(buf) - 1 , in);
		if (readnum <= 0) break;
		strcat(code , buf);
	}

	fclose(in);
}

void error(char c){
    printf("Error in line: %d. Wrong character: %c\n" , ll , c);
}

char getch(){
    return code[cptr ++];
}

void retract(){
    cptr --;
    tptr = 0;
}

void catToken(char c){
    token[tptr ++] = c;
    token[tptr] = '\0';
}

char catNRead(char c){
    catToken(c);
    return getch();
}

void singalToken(char c){
    catToken(c);
	tptr = 0;
}

int reserver(char *s){
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
	return 0;
}

int getsym(){
	char c = getch();
	memset(token,0,sizeof(token));
	while(c == ' '|| c == '\n' || c == '\t'){
        if(c == '\n'){
            ll ++;
        }
        c = getch();   // bug fix : need to update c value!
	}

	if(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		c = catNRead(c);
		while(c == '_' || (c >= 'a' && c <= 'z') ||\
             (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
			c = catNRead(c);
		}

		retract();
		int resultValue = reserver(token);
		if (resultValue == 0) return IDEN;
		else return resultValue;
	}
	else if(c >= '0' && c <= '9'){
		c = catNRead(c);
		while(c >= '0' && c <= '9'){
			c = catNRead(c);
		}
		if(token[0] == '0' && tptr > 1){
            error('0');
            return -1;
		}
		retract();

		return UINT;
	}
	else if(c == '+'){
		singalToken(c);
		return PLUS;
	}
	else if(c == '-'){
		singalToken(c);
		return MINUS;
	}
	else if(c == '*'){
		singalToken(c);
		return MULT;
	}
	else if(c == '/'){
		singalToken(c);
		return DIV;
	}
	else if(c == '<'){
		c = catNRead(c);
		if(c == '='){
			singalToken(c);
			return LEQ;
		}
		else{
			retract();
			return LESS;
		}
	}
	else if(c == '>'){
		c = catNRead(c);
		if(c == '='){
			singalToken(c);
			return GEQ;
		}
		else{
			retract();
			return GREAT;
		}
	}
	else if(c == '='){
        c = catNRead(c);
        if(c == '='){
            singalToken(c);
            return ASSIGN;
        }
		retract();
		return EQUAL;
	}
	else if(c == '!'){
        c = catNRead(c);
        if(c == '='){
            singalToken(c);
            return NEQ;
        }
        else{
            error(c);
            return -1;
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
            error(c);
            return -1;
		}
		c = catNRead(c);
        if(c != '\''){
            error(c);
            return -1;
        }
        else{
            singalToken(c);
            return CHAR;
        }
	}
	else if(c =='\"'){
        c = catNRead(c);
        while(c >= 32 && c <= 126 && c != 34){
            c = catNRead(c);
        }
        if(c != '\"'){
            error(c);
            return -1;
        }
        else{
            singalToken(c);
            return STRING;
        }
	}
	else if(c == '('){
		singalToken(c);
		return LPAR;
	}
	else if(c == ')'){
		singalToken(c);
		return RPAR;
	}
	else if(c == '['){
		singalToken(c);
		return LZKH;
	}
	else if(c == ']'){
		singalToken(c);
		return RZKH;
	}
	else if(c == '{'){
		singalToken(c);
		return LDKH;
	}
	else if(c == '}'){
		singalToken(c);
		return RDKH;
	}
	else if(c == ','){
		singalToken(c);
		return COMMA;
	}
	else if(c == ';'){
		singalToken(c);
		return SEMI;
	}
	else if(c == ':'){
		singalToken(c);
		return COLON;
	}
	else if(c == '\0'){
		return -1;
	}
	else{
		error(c);
		return -1;
	}
}

void output(){
    printf("%s\n",code);
	FILE *out = fopen("16061160_result.txt","w");
	int type = getsym();
	//printf("%d\n",type);
	while(type != -1){
		fprintf(out , "%-10s%-10s\n" , sym_type[type] , token);
		type = getsym();
		//printf("%d\n",type);
	}
	printf("Program exit successfully.\nPlease check results in file '16061160_result.txt'.\n");
	fclose(out);
}

