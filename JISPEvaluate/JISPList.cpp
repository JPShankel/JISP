/*
** JISPList.cpp
**
** JISPList.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPList.h"



namespace JISP
{

    bool JISPListElementUnitTest()
    {
        std::string testString;
        JISPListElement_t testElement1,testElement2,testElement3;
        int deadbeef[] = {0,0,0,0,0,0};
        JISP::CreateListElement(jleTypeString_k,"\"string1\"",(unsigned int)strlen("\"string1\"")+1,&testElement1);
        JISP::CreateListElement(jleTypeString_k,"\"string2\"",(unsigned int)strlen("\"string2\"")+1,&testElement2);

        JISP::ListElementToString(&testElement1,&testString);
        JISP::ListElementToString(&testElement2,&testString);

        JISP::ConstructList(&testElement1,&testElement2,&testElement3);
        JISP::ListElementToString(&testElement3,&testString);

        JISP::CreateListElement(jleTypeList_k,&testElement2[0],testElement2.size(),&testElement3);
        std::string testString2;
        JISP::ListElementToString(&testElement3,&testString2);

        JISP::ConstructList(&testElement3,&testElement1,&testElement2);
        JISP::ListElementToString(&testElement2,&testString);

        JISP::ConstructList(&testElement1,&testElement3,&testElement2);
        JISP::ListElementToString(&testElement2,&testString);

        JISP::CreateListElement(jleTypeString_k,"\"string3\"",(unsigned int)strlen("\"string3\"")+1,&testElement3);
        JISP::ConstructList(&testElement2,&testElement3,&testElement1);
        JISP::ListElementToString(&testElement1,&testString2);

        JISP::ConstructList(&testElement3,&testElement2,&testElement1);
        JISP::ListElementToString(&testElement1,&testString);

        testString = testString;

        return true;
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


    bool ConstructList(const JISPListElement_t *car,const JISPListElement_t *cdr,JISPListElement_t *output)
    {
        CreateListElement(jleTypeList_k,(const char *)&(*car)[0],car->size(),output);
        output->resize(output->size()+cdr->size());

        JISPListElementIterator_t *outputIterator = IteratorFromListElement(output);
        memcpy(&(*outputIterator).data_[outputIterator->carLen_],&(*cdr)[0],cdr->size());
        outputIterator->cdrLen_ = cdr->size();

        return true;
    }

    void ListElementToStringRecurse(const JISPListElementIterator_t *jle,std::string *str)
    {
        if (jle->type_ == jleTypeList_k)
        {
            (*str) += "(";
            if (jle->carLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
                ListElementToStringRecurse(jleComponent,str);
            }
            if (jle->cdrLen_ > 0)
            {
                const JISPListElementIterator_t *jleComponent = reinterpret_cast<const JISPListElementIterator_t*>(static_cast<const void*>(&jle->data_[jle->carLen_]));
                (*str) += ".";
                ListElementToStringRecurse(jleComponent,str);
            }
            (*str) += ")";
        }
        else
        {
            (*str) += (const char *)jle->data_;
            (*str) += " ";
        }
    }
    
    bool ListElementToString(const JISPListElement_t *jle,std::string *str)
    {
        str->clear();
        const JISPListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringRecurse(jleIterator,str);
        return true;
    }

}