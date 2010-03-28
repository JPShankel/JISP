/*
** JISPFunctions.cpp
**
** JISPFunctions.cpp - JISP list library functions
**
** Copyright (c) 2010 - Jason Shankel
*/

#include "JISPList.h"

namespace JISP
{
    const char *JISPFunctions_k[] =
    {
        "(define caar (lambda (l) (car (car l))))",

        "(define cadr (lambda (l) (car (cdr l))))",

        "(define lat? (lambda (l) \
            (cond \
                ((null? l) #t) \
                ((atom? (car l)) (lat? (cdr l))) \
                (else #f) \
            ) \
          ))"
                
    };

    bool BuildStandardFunctions(JISPContext_t *context)
    {
        bool ret = true;

        size_t numFunctions = sizeof(JISPFunctions_k)/sizeof(const char *);
        for (size_t i=0;i<numFunctions;++i)
        {
            ListElement_t func;
            JISP::StringToListElement(JISPFunctions_k[i],&func);
            ret = JISP::EvaluateListElement(context,&func,&func) && ret;
        }
        return ret;
    }

}