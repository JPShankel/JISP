/*
** JISPList.h
**
** JISPList.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#ifndef JISPLIST_H
#define JISPLIST_H

#include <vector>

namespace JISP
{
    typedef std::vector<unsigned char> JISPListElement_t;

    enum JISPListElementTypes_t 
    {
     jleTypeCharacter_k = 0,
     jleTypeString_k,
     jleTypeInteger_k,
     jleTypeFloat_k,
     jleTypeComplex_k,
     jleTypeList_k
    };

    bool CreateListElement(JISPListElementTypes_t type,const void *data,unsigned int dataLength,JISPListElement_t *jlw);
    bool ListElementToStringConcise(const JISPListElement_t *jle,std::string *str);
    bool ListElementToStringVerbose(const JISPListElement_t *jle,std::string *str);
    bool StringToListElement(const char *str,JISPListElement_t *jle);
    bool JISPCons(const JISPListElement_t *car,const JISPListElement_t *cdr,JISPListElement_t *output);
    bool JISPCAR(const JISPListElement_t *jle,JISPListElement_t *output);
    bool JISPCDR(const JISPListElement_t *jle,JISPListElement_t *output);

    bool JISPListElementUnitTest();
}

#endif //JISPLIST_H