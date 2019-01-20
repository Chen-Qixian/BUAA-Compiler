#include "main.h"

using namespace compiler;

int main()
{
    init();
    grammar_output();
    err_open();
    grammar();
    mid2mips();
    return 0;
}

