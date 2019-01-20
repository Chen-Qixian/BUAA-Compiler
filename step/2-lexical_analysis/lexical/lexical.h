
#ifndef _LEXICAL_H
#define _LEXICAL_H
#include <stdio.h>
// constant define
#define UINT    0
#define IDEN    1
#define PLUS    2
#define MINUS   3
#define MULT    4
#define DIV     5
#define LESS    6
#define LEQ     7
#define GREAT   8
#define GEQ     9
#define EQUAL   10
#define NEQ     11
#define ASSIGN  12
#define CHAR    13   // ''
#define STRING  14
#define COMMA   15
#define SEMI    16
#define COLON   17
#define LPAR    18
#define RPAR    19
#define LZKH    20   // [
#define RZKH    21   // ]
#define LDKH    22   // {
#define RDKH    23   // }
#define CONST   24
#define INT     25
#define CHARKW  26
#define VOID    27
#define MAIN    28
#define IF      29
#define SWITCH  30
#define CASE    31
#define DEFAUL  32
#define SCANF   33
#define PRINTF  34
#define RETURN  35

// declaration definition
void readFile();
void error(char c);
char getch();
void retract();
void catToken(char c);
char catNRead(char c);
void singalToken(char c);
int reserver(char *s);
int getsym();
void output();






#endif // _LEXICAL_H

