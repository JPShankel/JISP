/*
** JISPList.cpp
**
** JISPList.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPList.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>



namespace JISP
{


    bool JISPListElementUnitTest()
    {
        std::string testString;
        bool ret = true;
        JISPListElement_t element1,element2,element3,element4;

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
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(A B C D)");

        JISP::StringToListElement("(cons 'A '(B))",&element1);
        JISP::ListElementToStringConcise(&element1,&testString);
        ret = ret && (testString == "(cons ' A ' (B))");
        testString.clear();

        // Evaluation
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(A B)");

        JISP::StringToListElement("(car '(A B))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "A");

        JISP::StringToListElement("(car '((A B) C D))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(A . (B . ()))");

        JISP::StringToListElement("(cdr '((A B) C D))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(C . (D . ()))");

        JISP::StringToListElement("(car (cdr '((A B) C D)))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "C");

        JISP::StringToListElement("(cdr (car '((A B) C D)))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(B)");

        JISP::StringToListElement("(cons (car '(1 2 3 4))(cdr '(1 2 3 4)))",&element1);
        JISP::EvaluateListElement(&element1,&element2);
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(1 2 3 4)");
        return ret;
    }
    /////////////////////////////

    struct JISPListElementIterator_t
    {
        unsigned int type_,carLen_,cdrLen_;
        unsigned char data_[];
    };
    /////////////////////////////

    JISPListElementIterator_t *IteratorFromListElement(JISPListElement_t *le)
    {
        return reinterpret_cast<JISPListElementIterator_t*>(static_cast<void*>(&(*le)[0]));
    }
    /////////////////////////////

    const JISPListElementIterator_t *IteratorFromListElement(const JISPListElement_t *le)
    {
        return reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&(*le)[0]));
    }
    /////////////////////////////

    bool CreateListElement(JISPListElementTypes_t type,const void *data,unsigned int dataLength,JISPListElement_t *jle)
    {
        unsigned int cdrLength = 0;
        void *cdrData = 0;
        JISPListElementIterator_t cdrList;
        if (type == jleTypeList_k)
        {
            cdrLength = sizeof(JISPListElementIterator_t);
            cdrList.type_ = jleTypeList_k;
            cdrList.carLen_ = 0;
            cdrList.cdrLen_ = 0;
        }

        (*jle).resize(sizeof(JISPListElementIterator_t)+dataLength+cdrLength);

        JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        jleIterator->type_ = type;
        jleIterator->carLen_ = dataLength;
        jleIterator->cdrLen_ = cdrLength;
        memcpy(jleIterator->data_,data,dataLength);
        if (cdrLength != 0)
        {
            memcpy(&jleIterator->data_[dataLength],&cdrList,cdrLength);
        }
        return true;
    }
    /////////////////////////////


    bool JISPCons(const JISPListElement_t *car,const JISPListElement_t *cdr,JISPListElement_t *output)
    {
        JISPListElement_t localOutput;


        CreateListElement(jleTypeList_k,(const char *)&(*car)[0],static_cast<unsigned int>(car->size()),&localOutput);
        localOutput.resize(sizeof (JISPListElementIterator_t)+car->size()+cdr->size());

        JISPListElementIterator_t *outputIterator = IteratorFromListElement(&localOutput);
        memcpy(&(*outputIterator).data_[outputIterator->carLen_],&(*cdr)[0],cdr->size());
        outputIterator->cdrLen_ = static_cast<unsigned int>(cdr->size());
        (*output) = localOutput;

        return true;
    }
    /////////////////////////////

    bool JISPCAR(const JISPListElement_t *jle,JISPListElement_t *output)
    {
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        JISPListElement_t localOutput;
        localOutput.resize(jleIterator->carLen_);
        memcpy(&localOutput[0],jleIterator->data_,jleIterator->carLen_);
        (*output) = localOutput;
        return true;
    }
    /////////////////////////////

    bool JISPCDR(const JISPListElement_t *jle,JISPListElement_t *output)
    {
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        JISPListElement_t localOutput;
        localOutput.resize(jleIterator->cdrLen_);
        memcpy(&localOutput[0],&jleIterator->data_[jleIterator->carLen_],jleIterator->cdrLen_);
        (*output) = localOutput;
        return true;
    }
    /////////////////////////////


    void ListElementToStringVerboseRecurse(const JISPListElementIterator_t *jle,std::string *str)
    {
        if (jle->type_ == jleTypeQuoted_k)
        {
            (*str) += "(quote ";
            const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
            ListElementToStringVerboseRecurse(jleComponent,str);
            (*str) += ")";
            return;
        }
        if (jle->type_ == jleTypeList_k)
        {
            (*str) += "(";
            if (jle->carLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
                ListElementToStringVerboseRecurse(jleComponent,str);
            }
            if (jle->cdrLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[jle->carLen_]));
                (*str) += " . ";
                ListElementToStringVerboseRecurse(jleComponent,str);
            }
            if ( (*str)[(*str).length()-1] == ' ')
            {
                (*str) = (*str).substr(0,(*str).length()-1);
            }
            (*str) += ")";
        }
        else
        {
            (*str) += (const char *)jle->data_;
        }
    }
    /////////////////////////////

    void ListElementToStringConciseRecurse(const JISPListElementIterator_t *jle,std::string *str,bool car)
    {
        if (jle->type_ == jleTypeQuoted_k)
        {
            if (!(*str).empty())
            {
                (*str) += " '";
            }
            else
            {
                (*str) += "'";
            }
            const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
            ListElementToStringConciseRecurse(jleComponent,str,true);
            return;
        }
        if (jle->type_ == jleTypeList_k)
        {
            if (car)
            {
                if (!(*str).empty() && (*str)[(*str).length()-1] != '(')
                {
                    (*str) +=" (";
                }
                else
                {
                    (*str) += "(";
                }
            }
            
            if (jle->carLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
                ListElementToStringConciseRecurse(jleComponent,str,true);
            }
            if (jle->cdrLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[jle->carLen_]));
                ListElementToStringConciseRecurse(jleComponent,str,false);
            }
            if (car)
            {
                if ( (*str)[(*str).length()-1] == ' ')
                {
                    (*str) = (*str).substr(0,(*str).length()-1);
                }
                (*str) += ")";
            }
        }
        else
        {
            if (!car)
            {
                (*str) += " . ";
            }
            if ( !(*str).empty())
            {
                while  ( !(*str).empty() && (*str)[(*str).length()-1] != ' ' && (*str)[(*str).length()-1] != '(')
                {
                    (*str) += " ";
                }
            }
            (*str) += (const char *)jle->data_;
        }
    }
    /////////////////////////////

    
    bool ListElementToStringConcise(const JISPListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringConciseRecurse(jleIterator,str,true);

        while ( !(*str).empty() && (*str)[(*str).length()-1] == ' ')
        {
            (*str) = (*str).substr(0,(*str).length()-1);
        }

        return true;
    }
    /////////////////////////////

    bool ListElementToStringVerbose(const JISPListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringVerboseRecurse(jleIterator,str);

        while ( !(*str).empty() && (*str)[(*str).length()-1] == ' ')
        {
            (*str) = (*str).substr(0,(*str).length()-1);
        }
        return true;
    }
    /////////////////////////////

    typedef bool (*JISPFunctionPtr_t)(const JISPListElementVector_t *parameters,JISPListElement_t *output);
    typedef std::map<std::string,JISPFunctionPtr_t> JISPFunctionMap_t;
    static JISPFunctionMap_t JISPFunctionMap_s;

    bool JISPConsFunction(const JISPListElementVector_t *parameters,JISPListElement_t *output)
    {
        if (parameters->size() == 2)
        {
            JISPListElement_t localOutput;
            JISPCons(&(*parameters)[0],&(*parameters)[1],&localOutput);
            (*output) = localOutput;
            return true;
        }
        else
        {
            std::cout << "Error - wrong number of parameters for cons" << std::endl;
            return false;
        }
    }

    bool JISPCarFunction(const JISPListElementVector_t *parameters,JISPListElement_t *output)
    {
        if (parameters->size() == 1)
        {
            const JISPListElementIterator_t *paramIterator = JISP::IteratorFromListElement(&(*parameters)[0]);
            if (paramIterator->type_ == jleTypeList_k)
            {
                JISPListElement_t localOutput;
                JISPCAR(&(*parameters)[0],&localOutput);
                (*output) = localOutput;
                return true;
            }
            else
            {
                std::cout << "Error - bad parameter for car" << std::endl;
            }
        }
        else
        {
            std::cout << "Error - wrong number of parameters for car" << std::endl;
        }
    }

    bool JISPCdrFunction(const JISPListElementVector_t *parameters,JISPListElement_t *output)
    {
        if (parameters->size() == 1)
        {
            const JISPListElementIterator_t *paramIterator = JISP::IteratorFromListElement(&(*parameters)[0]);
            if (paramIterator->type_ == jleTypeList_k)
            {
                JISPListElement_t localOutput;
                JISPCDR(&(*parameters)[0],&localOutput);
                (*output) = localOutput;
                return true;
            }
            else
            {
                std::cout << "Error - bad parameter for cdr" << std::endl;
            }
        }
        else
        {
            std::cout << "Error - wrong number of parameters for cdr" << std::endl;
        }
    }


    bool JISPApplyFunction(const char *fn,const JISPListElementVector_t *parameters,JISPListElement_t *output)
    {
        JISPFunctionMap_t::iterator it = JISPFunctionMap_s.find(std::string(fn));
        if (it != JISPFunctionMap_s.end())
        {
            return (*(*it).second)(parameters,output);
        }
        std::cout << "Error - unidentified function " << fn << std::endl;
        return false;
    }
    ////////////////////////////

    bool EvaluateListElement(const JISPListElement_t *input,JISPListElement_t *output)
    {
        const JISPListElementIterator_t *inputIterator = IteratorFromListElement(input);

        if (inputIterator->type_ == jleTypeString_k)
        {
            (*output) = (*input);
            return true;
        }

        if (inputIterator->type_ == jleTypeQuoted_k)
        {
            const JISPListElementIterator_t *dataIterator = reinterpret_cast<const JISPListElementIterator_t *>(inputIterator->data_);
            JISPListElement_t localOutput;
            size_t elementSize = dataIterator->carLen_ + dataIterator->cdrLen_ + sizeof(JISPListElementIterator_t);
            localOutput.resize(elementSize);
            memcpy(&localOutput[0],dataIterator,elementSize);
            (*output) = localOutput;
            return true;
        }


        if (inputIterator->type_ == jleTypeList_k)
        {
            JISPListElement_t car,cdr;
            JISPListElementVector_t parameters;

            if (inputIterator->carLen_ > 0)
            {
                JISPCAR(input,&car);
                JISPListElementIterator_t *carIterator = IteratorFromListElement(&car);
                if (carIterator->type_ == jleTypeIdentifier_k)
                {
                    JISPCDR(input,&cdr);
                    std::string function = reinterpret_cast<const char *>(carIterator->data_);

                    JISPListElementIterator_t *cdrIterator = IteratorFromListElement(&cdr);
                    while(cdrIterator->carLen_ > 0)
                    {
                        JISPCAR(&cdr,&car);
                        JISPCDR(&cdr,&cdr);

                        carIterator = IteratorFromListElement(&car);
                        cdrIterator = IteratorFromListElement(&cdr);

                        if (carIterator->type_ == jleTypeList_k || carIterator->type_ == jleTypeQuoted_k)
                        {
                            EvaluateListElement(&car,&car);
                        }
                        parameters.push_back(car);
                    }

                    return JISPApplyFunction(function.c_str(),&parameters,output);

                }
                else
                {
                    return false;
                }
            }
            else
            {
                (*output) = (*input);
                return true;
            }
        }
        return false;
    }

    void InitJISPSystem()
    {
        JISPFunctionMap_s.insert(JISPFunctionMap_t::value_type("cons",JISPConsFunction));
        JISPFunctionMap_s.insert(JISPFunctionMap_t::value_type("car",JISPCarFunction));
        JISPFunctionMap_s.insert(JISPFunctionMap_t::value_type("cdr",JISPCdrFunction));
    }


}