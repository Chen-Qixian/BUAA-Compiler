#include "init.h"

namespace compiler{
    void readFile(){
        std::string filename;
        std::cin >> filename;
        std::ifstream srcCodeFile(filename); //读文件ifstream,写文件ofstream，可读可写fstream
        std::stringstream buffer;
        buffer << srcCodeFile.rdbuf();
        code = buffer.str();
        srcCodeFile.close();
    }

    void init(){
        readFile();
    }
}
