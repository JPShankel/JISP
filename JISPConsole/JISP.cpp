#include <fstream>
#include <iostream>
#include <string>
#include "..\JISPParse\JISPCompile.h"
#include "..\JISPEvaluate\JISPList.h"

static void JISPOutputHandlerConsole(const char *text,void *data)
{
    std::cout << text << std::endl;
}

int main(int argc,char **argv)
{


    JISP::ListElementUnitTest();
  
    std::string str;

    JISP::JISPContext_t *context = JISP::CreateJISPContext();
    JISP::SetJISPContextErrorHandler(context,JISPOutputHandlerConsole,0);
    JISP::SetJISPContextOutputHandler(context,JISPOutputHandlerConsole,0);

    std::cout << "JISP Scheme Version 0.0.0" << std::endl;
    std::cout << "Copyright (c) 2010 - Jason Shankel" << std::endl << std::endl;

    std::cout << ">";
    while (JISP::GetJISPContextIsActive(context) && getline(std::cin,str))
    {
        JISP::ListElement_t compiledElement,evaluatedElement;
        JISP::StringToListElement(str.c_str(),&compiledElement);

        if (JISP::EvaluateListElement(context,&compiledElement,&evaluatedElement))
        {
            std::string output;
            if (JISP::ListElementToStringConcise(&evaluatedElement,&output))
            {
                std::cout << output.c_str() << std::endl;
            }
        }
        std::cout << ">";
    }

    JISP::DestroyJISPContext(context);

    return 0;
}
