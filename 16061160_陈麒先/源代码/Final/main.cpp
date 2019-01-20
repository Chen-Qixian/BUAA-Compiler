#include "main.h"

using namespace compiler;

int main()
{
    init();
    grammar_output();
    grammar();
    mid2mips();
    if(!has_error){
        std::cout << "Compile succeed !\n";
    }
    return 0;
}
