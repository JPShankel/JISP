/*
** JISPCompile.cpp
**
** JISPCompile.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPCompile.h"
#include "..\JISPEvaluate\JISPList.h"

#include <stdio.h>
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
#include <string>

static std::vector<std::string> compilerStrings_s;
static JISP::ListElementVector_t listElements_s;
static std::vector<std::vector<int> > elementListLists_s;

extern "C"
{
    unsigned int AddToken(const char *token)
    {
        compilerStrings_s.push_back(token);
        return static_cast<int>(compilerStrings_s.size()-1);
    }

    const char *GetToken(unsigned int index)
    {
        return compilerStrings_s[index].c_str();
    }

    unsigned int AddJISPElementToList(unsigned int elist, unsigned int element)
    {
        elementListLists_s[elist].push_back(element);
        return elist;
    }

    unsigned int EndJISPElementList(unsigned int elist)
    {
        std::vector<int> &elementList = elementListLists_s[elist];

        JISP::ListElement_t newList;
        JISP::CreateListElement(JISP::jleTypeList_k,&listElements_s[elementList.back()][0],static_cast<unsigned int>(listElements_s[elementList.back()].size()),&newList);

        for (int i=(int)elementList.size()-2;i>=0;--i)
        {
            JISP::JISPCons(&listElements_s[elementList[i]],&newList,&newList);
        }

        listElements_s.push_back(newList);
        return static_cast<int>(listElements_s.size()-1);
    }

    unsigned int BeginJISPElementList(unsigned int element)
    {
        elementListLists_s.push_back(std::vector<int>());
        elementListLists_s.back().push_back(element);
        return static_cast<int>(elementListLists_s.size()-1);
    }

    unsigned int NullJISPList()
    {
        return 0;
    }


    unsigned int AddJISPElement(unsigned int type, unsigned int dataIndex)
    {
        if (type == JISP::jleTypeList_k || type == JISP::jleTypeQuoted_k)
        {
            JISP::ListElement_t jle;
            JISP::CreateListElement((JISP::ListElementTypes_t)type,&listElements_s[dataIndex][0],static_cast<unsigned int>(listElements_s[dataIndex].size()),&jle);
            listElements_s.push_back(jle);
            return static_cast<int>(listElements_s.size()-1);
        }
        else
        {
            JISP::ListElement_t jle;
            std::string &str = compilerStrings_s[dataIndex];
            JISP::CreateListElement((JISP::ListElementTypes_t)type,str.c_str(),static_cast<unsigned int>(str.size()+1),&jle);
            listElements_s.push_back(jle);
            return static_cast<int>(listElements_s.size()-1);
        }
    }

    unsigned int AddQuotedElement(unsigned int type, unsigned int dataIndex)
    {
        return AddJISPElement(type,dataIndex);
    }

    void compile(const char *data);

    void yyerror(const char *error)
    {
        std::cout << "Error in compile: " << error << std::endl;
    }
}

namespace JISP
{
    bool StringToListElement(const char *str,ListElement_t *jle)
    {
        CompileString(str);

        if (!listElements_s.empty())
        {
            (*jle) = listElements_s.back();
        }
        return true;
    }


    bool CompileString(const char *data)
    {
        compilerStrings_s.clear();
        listElements_s.clear();
        elementListLists_s.clear();

        // null
        JISP::ListElement_t newList;
        JISP::CreateListElement(JISP::jleTypeList_k,0,0,&newList);
        listElements_s.push_back(newList);

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
            compilerStrings_s.clear();
            listElements_s.clear();
            compile(data);
            delete[] data;
            return true;
        }
        return false;
    }
}