#ifndef _LEXICAL_H_
#define _LEXICAL_H_

//#include "parameters.h"
#include "error.h"
#include <cstring>
#include <cstdlib>

namespace compiler{
    void getsym();
    char getch();
    void retract();
    void catToken(char c);
    char catNRead(char c);
    void singalToken(char c);
    char skip_illegal_chars();
    symbol reserver(char *s);
    void getsym();
    void lexical_output();
}

#endif // _LEXICAL_H_
