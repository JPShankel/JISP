/*
** JISPList.cpp
**
** JISPList.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPList.h"
#include <string>



namespace JISP
{


    bool JISPListElementUnitTest()
    {
        std::string testString;
        bool ret = true;
        JISPListElement_t element1,element2,element3;
        int deadbeef[] = {0,0,0,0,0,0};

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
        JISP::CreateListElement(jleTypeList_k,&element2[0],element2.size(),&element3);

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

        // (cons "string1" ("string2")) = ("string1" "string2")
        JISP::JISPCons(&element1,&element3,&element2);
        
        JISP::ListElementToStringVerbose(&element2,&testString);
        ret = ret && (testString == "(\"string1\" . (\"string2\" . ()))");
        JISP::ListElementToStringConcise(&element2,&testString);
        ret = ret && (testString == "(\"string1\" \"string2\")");

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


        return ret;
    }


    struct JISPListElementIterator_t
    {
        unsigned int type_,carLen_,cdrLen_;
        unsigned char data_[];
    };

    JISPListElementIterator_t *IteratorFromListElement(JISPListElement_t *le)
    {
        return reinterpret_cast<JISPListElementIterator_t*>(static_cast<void*>(&(*le)[0]));
    }

    const JISPListElementIterator_t *IteratorFromListElement(const JISPListElement_t *le)
    {
        return reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&(*le)[0]));
    }

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


    bool JISPCons(const JISPListElement_t *car,const JISPListElement_t *cdr,JISPListElement_t *output)
    {
        CreateListElement(jleTypeList_k,(const char *)&(*car)[0],car->size(),output);
        output->resize(output->size()+cdr->size());

        JISPListElementIterator_t *outputIterator = IteratorFromListElement(output);
        memcpy(&(*outputIterator).data_[outputIterator->carLen_],&(*cdr)[0],cdr->size());
        outputIterator->cdrLen_ = cdr->size();

        return true;
    }

    bool JISPCAR(const JISPListElement_t *jle,JISPListElement_t *output)
    {
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        (*output).clear();
        return true;
    }

    bool JISPCDR(const JISPListElement_t *jle,JISPListElement_t *output)
    {
        return true;
    }


    void ListElementToStringVerboseRecurse(const JISPListElementIterator_t *jle,std::string *str)
    {
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

        void ListElementToStringConciseRecurse(const JISPListElementIterator_t *jle,std::string *str,bool car)
    {
        if (jle->type_ == jleTypeList_k)
        {
            if (car)
            {
                (*str) += "(";
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
                if ( (*str)[(*str).length()-1] != ' ' && (*str)[(*str).length()-1] != '(')
                {
                    (*str) += " ";
                }
            }
            (*str) += (const char *)jle->data_;
        }
    }

    
    bool ListElementToStringConcise(const JISPListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringConciseRecurse(jleIterator,str,true);

        if ( (*str)[(*str).length()-1] == ' ')
        {
            (*str) = (*str).substr(0,(*str).length()-1);
        }

        return true;
    }

    bool ListElementToStringVerbose(const JISPListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringVerboseRecurse(jleIterator,str);

        if ( (*str)[(*str).length()-1] == ' ')
        {
            (*str) = (*str).substr(0,(*str).length()-1);
        }


        return true;
    }


}