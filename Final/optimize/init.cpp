#include "init.h"

namespace compiler{
    void readFile(){
        std::string fileName;
        std::cin >> fileName;
        std::ifstream srcCodeFile(fileName); //���ļ�ifstream,д�ļ�ofstream���ɶ���дfstream
        //std::ifstream srcCodeFile("16061160_error_test.txt"); //���ļ�ifstream,д�ļ�ofstream���ɶ���дfstream
        std::stringstream buffer;
        buffer << srcCodeFile.rdbuf();
        code = buffer.str();
        srcCodeFile.close();
        if(code.length() == 0){
            fatal(3); // ��ȡ�ļ��쳣
        }
    }

    void init(){
        readFile();
    }
}
