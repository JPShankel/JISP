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

        "(define caaar (lambda (l) (car (car (car l)))))",

        "(define cadr (lambda (l) (car (cdr l))))",

        "(define caddr (lambda (l) (car (cdr (cdr l)))))",

        "(define not (lambda (op) \
            (cond \
                (op #f) \
                (else #t) \
             ) \
             ))",
                
        "(define and (lambda (op1 op2) \
            (cond \
                ((not op1) #f) \
                ((not op2) #f) \
                (else #t) \
                )\
           ))",
              
        "(define or (lambda (op1 op2) \
            (cond \
                (op1 #t) \
                (op2 #t) \
                (else #f) \
            )\
            ))",

        "(define xor (lambda (op1 op2) \
            (and (or op1 op2) (not (and op1 op2))) \
            ))",

        "(define nor (lambda (op1 op2) \
            (not (or op1 op2)) \
            ))",

        "(define nand (lambda (op1 op2) \
            (not (and op1 op2)) \
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