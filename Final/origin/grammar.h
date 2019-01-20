#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include "functional.h"
#include "enter.h"
namespace compiler{
    void constDeclare();
    void varDeclare();
    void addPara();
    void paraList();
    std::string factor(bool& is_char);
    std::string item(bool& is_char);
    std::string expression(bool& is_char);
    void condition();
    void ifStatement();
    void whileStatement();
    void paraCheck(bool is_char);
    void valueParaList(int idx_in_tab);
    void voidFuncCall();
    void retFuncCall();
    void assignStatement();
    void read_mid_code();
    void scanfStatement();
    void printfStatement();
    void returnStatement();
    void constant();
    void caseStatement();
    void caseTable();
    void defaultCase();

    void switchStatement();
    void statement();
    void compoundStatement();
    void addFunc();
    void funcDeclares();
    void voidFuncDeclare();
    void returnFuncDeclare();
    void mainFuncDeclare();
    void grammar();
    void grammar_output();
}
#endif // _GRAMMAR_H_
