#include "main.h"

using namespace compiler;

int main()
{
    init();
    //lexical_output();
    grammar_output();
    err_open();
    grammar();
    mid2mips();
    if(!has_error){
        std::cout << "Compile succeed !\n";
    }
    return 0;
}
