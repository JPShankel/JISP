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
    typedef std::vector<unsigned char> ListElement_t;
    typedef std::vector<ListElement_t> ListElementVector_t;

    struct JISPContext_t;


    enum ListElementTypes_t 
    {
        jleTypeCharacter_k = 0,
        jleTypeString_k,
        jleTypeInteger_k,
        jleTypeRational_k,
        jleTypeFloat_k,
        jleTypeComplex_k,
        jleTypeQuoted_k,
        jleTypeBoolean_k,
        jleTypeIdentifier_k,
        jleTypeList_k,
        jleTypeEllipsis_k,
        jleTypeUnknown_k
    };


    bool JISPCons(const ListElement_t *car,const ListElement_t *cdr,ListElement_t *output);
    bool JISPAppend(const ListElement_t *list,const ListElement_t *toAppend,ListElement_t *output);
    bool JISPCAR(const ListElement_t *jle,ListElement_t *output);
    bool JISPCDR(const ListElement_t *jle,ListElement_t *output);


    bool CreateListElement(ListElementTypes_t type,const void *data,unsigned int dataLength,ListElement_t *jlw);
    bool ListElementToStringConcise(const ListElement_t *jle,std::string *str);
    bool ListElementToStringVerbose(const ListElement_t *jle,std::string *str);
    bool StringToListElement(const char *str,ListElement_t *jle);

    bool EvaluateListElement(JISPContext_t *context,const ListElement_t *input,ListElement_t *output);

    ListElementTypes_t GetListElementType(const ListElement_t *element);

    JISPContext_t *CreateJISPContext();
    void DestroyJISPContext(JISPContext_t *context);

    typedef void (*JISPOutputHandler_t)(const char *text,void *handlerData);

    void SetJISPContextErrorHandler(JISPContext_t *context,JISPOutputHandler_t eh,void *handlerData);
    void SetJISPContextOutputHandler(JISPContext_t *context,JISPOutputHandler_t oh,void *handlerData);

    bool GetJISPContextIsActive(JISPContext_t *context);
    bool BuildStandardFunctions(JISPContext_t *context);

    bool ListElementUnitTest();
}

#endif //JISPLIST_H