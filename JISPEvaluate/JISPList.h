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
    bool ListElementToString(const JISPListElement_t *jle,std::string *str);
    bool StringToListElement(const char *str,JISPListElement_t *jle);
    bool ConstructList(const JISPListElement_t *car,const JISPListElement_t *cdr,JISPListElement_t *output);

    bool JISPListElementUnitTest();
}

#endif //JISPLIST_H