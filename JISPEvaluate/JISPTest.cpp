/*
** JISPTest.cpp
**
** JISPTest.cpp - JISP unit tests
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPList.h"
#include <string>

namespace JISP
{
    void ListElementUnitTestOutputHandler(const char *msg,void *data)
    {
        std::string *str = reinterpret_cast<std::string *>(data);
        (*str) = msg;
    }


    bool ListElementUnitTest()
    {
        std::string testString,outputString;
        bool ret = true;
        ListElement_t element1,element2,element3,element4;

        JISPContext_t *context = CreateJISPContext();
        SetJISPContextErrorHandler(context,ListElementUnitTestOutputHandler,&outputString);

        // element1 = "string1", element2 = "string2"
        JISP::CreateListElement(jleTypeString_k,"\"string1\"",(unsigned int)strlen("\"string1\"")+1,&element1);
        JISP::CreateListElement(jleTypeString_k,"\"string2\"",(unsigned int)strlen("\"string2\"")+1,&element2);

        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "\"string1\"");
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "\"string2\"");

        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "\"string1\"");
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "\"string2\"");


        // (cons "string1" "string2") = ("string1" . "string2")
        JISP::JISPCons(&element1,&element2,&element3);

        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "(\"string1\" . \"string2\")");
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "(\"string1\" . \"string2\")");

        //(list "string2") = ("string2")
        JISP::CreateListElement(jleTypeList_k,&element2[0],static_cast<unsigned int>(element2.size()),&element3);

        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "(\"string2\" . ())");
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "(\"string2\")");

        // (cons ("string1") "string2")) = (("string1") . "string2")
        JISP::JISPCons(&element3,&element1,&element2);

        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "((\"string2\" . ()) . \"string1\")");
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "((\"string2\") . \"string1\")");

        // (car (("string2") . "string1") = ("string1")
        JISP::JISPCAR(&element2,&element3);
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "(\"string2\")");
        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "(\"string2\" . ())");

        // (cdr (("string2") . "string1") = "string1");
        JISP::ListElementToStringConcise(&element2,&testString);
        JISP::JISPCDR(&element2,&element3);
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "\"string1\"");
        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "\"string1\"");


        // (cons "string1" ("string2")) = ("string1" "string2")
        JISP::CreateListElement(jleTypeString_k,"\"string2\"",(unsigned int)strlen("\"string2\"")+1,&element2);
        JISP::CreateListElement(jleTypeList_k,&element2[0],static_cast<unsigned int>(element2.size()),&element3);
        JISP::JISPCons(&element1,&element3,&element2);
        
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(\"string1\" . (\"string2\" . ()))");
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(\"string1\" \"string2\")");

        // (car ("string1" "string2") = "string1"
        JISP::JISPCAR(&element2,&element3);
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "\"string1\"");
        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "\"string1\"");

        // (cdr ("string1" "string2") = ("string2");
        JISP::JISPCDR(&element2,&element3);
        JISP::ListElementToStringConcise(&element3,&testString);
        ret = ret && (testString == "(\"string2\")");
        JISP::ListElementToStringVerbose(&element3,&testString);
        ret = ret && (testString == "(\"string2\" . ())");


        //(cons ("string1" "string2") "string3")) = (("string1")("string2") . "string3")
        JISP::CreateListElement(jleTypeString_k,"\"string3\"",(unsigned int)strlen("\"string3\"")+1,&element3);
        JISP::JISPCons(&element2,&element3,&element1);

        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "((\"string1\" . (\"string2\" . ())) . \"string3\")");
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "((\"string1\" \"string2\") . \"string3\")");

        //(cons "string3" ("string1" "string2")) = ("string3" "string1" "string2")
        JISP::JISPCons(&element3,&element2,&element1);

        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(\"string3\" \"string1\" \"string2\")");

        JISP::StringToListElement("(A)",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(A . ())");

        JISP::StringToListElement("(A B)",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(A . (B . ()))");

        JISP::StringToListElement("((A) B)",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "((A . ()) . (B . ()))");

        JISP::StringToListElement("(A (B))",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(A . ((B . ()) . ()))");
        
        JISP::StringToListElement("(A (B) (C D) E F (G (H I) J))",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(A (B) (C D) E F (G (H I) J))");

        JISP::StringToListElement("(((((A)B)C)D)E)",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(((((A . ()) . (B . ())) . (C . ())) . (D . ())) . (E . ()))");
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(((((A) B) C) D) E)");

        JISP::StringToListElement("'(A (B) (C D) (E (F) G))",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "' (A (B) (C D) (E (F) G))");

        JISP::StringToListElement("'(A B C)",&element1);
        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(quote (A . (B . (C . ()))))");

        JISP::StringToListElement("'(A B '(C D) E)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "' (A B ' (C D) E)");

        JISP::ListElementToStringVerbose(&element1,&testString);
        ret = ret && (testString == "(quote (A . (B . ((quote (C . (D . ()))) . (E . ())))))");

        JISP::StringToListElement("'(A B C D)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(A B C D)");

        JISP::StringToListElement("(cons 'A '(B))",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(cons ' A ' (B))");
        testString.clear();

        JISP::StringToListElement("(A . B)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(A . B)");
        testString.clear();

        JISP::StringToListElement("((A B) . C)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "((A B) . C)");
        testString.clear();

        JISP::StringToListElement("(A B . C)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(A B . C)");
        testString.clear();

        // Evaluation
        JISP::StringToListElement("'(B . C)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(B . C)");

        JISP::StringToListElement("(cdr '(A B . C))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(B . C)");
        
        JISP::StringToListElement("(cdr '(B . C))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "C");

        JISP::StringToListElement("(cons 'A '(B))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(A B)");

        JISP::StringToListElement("(car '(A B))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "A");

        JISP::StringToListElement("(car '((A B) C D))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(A . (B . ()))");

        JISP::StringToListElement("(cdr '((A B) C D))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(C . (D . ()))");

        JISP::StringToListElement("(car (cdr '((A B) C D)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "C");

        JISP::StringToListElement("(cdr (car '((A B) C D)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(B)");

        JISP::StringToListElement("(cons (car '(1 2 3 4))(cdr '(1 2 3 4)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(1 2 3 4)");

        JISP::StringToListElement("(eq? 1 1)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "#t");
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "#t");

        JISP::StringToListElement("(eq? 1 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "#f");
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "#f");

        JISP::StringToListElement("(+ 1 2)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "3");

        JISP::StringToListElement("(- (+ 3 4) (/ 4 4) (* 2 2.0))",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "2.000000");
        
        JISP::StringToListElement("(/ 120 4 2)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "15");

        JISP::StringToListElement("(define a 1)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("a",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "1");

        JISP::StringToListElement("(define b a)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("b",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "1");

        JISP::StringToListElement("(define b 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("b",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "2");

        JISP::StringToListElement("a",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "1");


        JISP::StringToListElement("(+ a 1)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "2");

        JISP::StringToListElement("(define plus +)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("(plus 1 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");

        JISP::StringToListElement("(define doubleplus plus)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("(doubleplus 3 4)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "7");
        
        JISP::StringToListElement("()",&element1);

        JISP::StringToListElement("'()",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element1,&testString);
        
        ret = ret && (testString == "' ()");

        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "()");

        JISP::StringToListElement("(atom? 1)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");

        JISP::StringToListElement("(atom? debbie)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#f");

        JISP::StringToListElement("(atom? 'harry)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#t");

        JISP::StringToListElement("(null? 1)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#f");
        
        JISP::StringToListElement("(null? '(a list))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#f");

        JISP::StringToListElement("(null? '())",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#t");

        JISP::StringToListElement("(list? 0)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#f");

        JISP::StringToListElement("(list? (a b))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#f");

        JISP::StringToListElement("(list? '(a b))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#t");

        JISP::StringToListElement("(list? '())",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        
        ret = ret && (testString == "#t");

        // illegal syntax ()
        JISP::StringToListElement("(list? ())",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        // cond
        JISP::StringToListElement("(cond ((eq? 1 1) 1) (else 0))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "1");

        JISP::StringToListElement("(cond ((eq? 1 2) 1) (else 0))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "0");

        JISP::StringToListElement("(cond ((eq? 1 1) (+ 1 2)) (else (- 5 3)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");

        JISP::StringToListElement("(cond ((eq? 1 2) (+ 1 2)) (else (- 5 3)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "2");

        JISP::StringToListElement("(cond ((eq? 1 2) (+ 1 2)) ((eq? 3 3) (+ 1 5)) (else (- 5 3)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "6");

        outputString.clear();
        JISP::StringToListElement("(cond ((eq? 1 2) (+ 1 2)) (else (- 5 3)) ((eq? 3 3) (+ 1 5)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);

        ret = ret && (outputString == "Error - misplaced keyword else");

        outputString.clear();
        JISP::StringToListElement("(cond ition)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);

        ret = ret && (outputString == "Error - invalid syntax");

        outputString.clear();
        JISP::StringToListElement("(cond (ition al))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);

        ret = ret && (outputString == "Error - undefined identifier ition");

        JISP::StringToListElement("((lambda (x) (+ x 1)) 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");

        JISP::StringToListElement("((lambda (a b) (eq? a (+ b 1))) 3 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");

        JISP::StringToListElement("(define a (lambda (x) (+ x 1)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("(a 2)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");

        JISP::StringToListElement("(define a (lambda (l) (car l)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("(a '(1 2))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "1");

        JISP::StringToListElement("((lambda x x) 1 2 3 4)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "(1 2 3 4)");
        

        JISP::StringToListElement("((lambda (x y . z) z) (+ 10 1) 12 (+ 2 11) 14)",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "(13 14)");

        // recursion
        JISP::StringToListElement("(define a (lambda (x) (cond ((eq? x 1) 1) (else (* x (a (- x 1)))))))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::StringToListElement("(a 4)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "24");

        // and, or, not
        JISP::StringToListElement("(and #t #t)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");

        JISP::StringToListElement("(and #t #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        JISP::StringToListElement("(or #t #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");

        JISP::StringToListElement("(or #f #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        JISP::StringToListElement("(not #t)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        JISP::StringToListElement("(not #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");
        
        JISP::StringToListElement("(xor #t #t)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        JISP::StringToListElement("(xor #t #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#t");

        JISP::StringToListElement("(xor #f #f)",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "#f");

        JISP::StringToListElement("(let ([x 1]) (+ x 2))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");
        

        // let, let*
        JISP::StringToListElement("(let ([x 1]) (+ x 1) (+ x 2) (+ x 3))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "4");

        JISP::StringToListElement("(let ([x 3]) (let ((y x)) (+ y 2)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "5");
        
        JISP::StringToListElement("(let ([x 1][y 2]) (let ([y x][x y]) (+ x 2)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "4");
 
        JISP::StringToListElement("(let ([x 1][y 2]) (let ([y x][x y]) (+ x y)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "3");
        
        JISP::StringToListElement("(let ([x 1][y 2]) (let* ([y x][x y]) (+ x y)))",&element1);
        JISP::EvaluateListElement(context,&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);

        ret = ret && (testString == "2");

        DestroyJISPContext(context);
        return ret;
    }
}
