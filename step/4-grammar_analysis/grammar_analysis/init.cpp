#include "init.h"

namespace compiler{
    void readFile(){
        std::string filename;
        std::cin >> filename;
        std::ifstream srcCodeFile(filename); //���ļ�ifstream,д�ļ�ofstream���ɶ���дfstream
        std::stringstream buffer;
        buffer << srcCodeFile.rdbuf();
        code = buffer.str();
        srcCodeFile.close();
    }

    void init(){
        readFile();
    }
}
