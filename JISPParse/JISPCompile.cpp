/*
** JISPCompile.cpp
**
** JISPCompile.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPCompile.h"

#include <stdio.h>
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
#include <string>

static std::vector<std::string> compilerStrings_s;

extern "C"
{
    int AddToken(const char *token)
    {
        compilerStrings_s.push_back(token);
        return static_cast<int>(compilerStrings_s.size()-1);
    }

    const char *GetToken(unsigned int index)
    {
        return compilerStrings_s[index].c_str();
    }

    void compile(const char *data);

    void yyerror(const char *error)
    {
        std::cout << "Error in compile: " << error << std::endl;
    }
}

namespace JISP
{
    bool CompileString(const char *data)
    {
        compile(data);
        return true;
    }


    bool CompileFile(const wchar_t *filepath)
    {
        FILE *fp = _wfopen(filepath,L"rb");
        if (fp != 0)
        {
            fseek(fp,0,SEEK_END);
            unsigned int len = ftell(fp);
            fseek(fp,0,SEEK_SET);
            char *data = new char[len+1];
            fread(fp,1,len,fp);
            fclose(fp);
            compile(data);
            delete[] data;
            return true;
        }
        return false;
    }
}