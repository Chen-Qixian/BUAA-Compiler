#include "init.h"

namespace compiler{
    void readFile(){
        std::string fileName;
        std::cin >> fileName;
        std::ifstream srcCodeFile(fileName); //读文件ifstream,写文件ofstream，可读可写fstream
        //std::ifstream srcCodeFile("16061160_error_test.txt"); //读文件ifstream,写文件ofstream，可读可写fstream
        std::stringstream buffer;
        buffer << srcCodeFile.rdbuf();
        code = buffer.str();
        srcCodeFile.close();
        if(code.length() == 0){
            fatal(3); // 读取文件异常
        }
    }

    void init(){
        readFile();
    }
}
