#ifndef _MID2MIPS_H_
#define _MID2MIPS_H_

#include "grammar.h"

namespace compiler{
void mid2mips();
void gen_mips_str();
void loadOp(std::string op , std::string reg);
void storeOp(std::string op , std::string reg);
}

#endif // _MID2MIPS_H_
