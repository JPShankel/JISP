#include <fstream>
#include <iostream>
#include <string>
#include "..\JISPParse\JISPCompile.h"
#include "..\JISPEvaluate\JISPList.h"
 
int main(int argc,char **argv)
{

    JISP::ListElementUnitTest();
  
    std::string str;
    std::cout << ">";
    while (getline(std::cin,str))
    {
        std::cout << ":";
        std::cout << str;
        std::cout << std::endl;

        if (str == "exit") break;

        JISP::CompileString(str.c_str());
        std::cout << ">";
    }
    return 0;
}
