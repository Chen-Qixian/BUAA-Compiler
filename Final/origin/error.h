#ifndef _ERROR_H_
#define _ERROR_H_

#include "parameters.h"
namespace compiler{

void error(int errnum);
void fatal(int fatalno);
void err_open();

}

#endif // _ERROR_H_
