/*
** JISPList.cpp
**
** JISPList.h/cpp - JISP list core functions and evaluator
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPList.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <assert.h>


namespace JISP
{


    struct ListElementIterator_t
    {
        ListElementTypes_t type_;
        unsigned int carLen_,cdrLen_;
        unsigned char data_[];
    };
    /////////////////////////////

    ListElementIterator_t *IteratorFromListElement(ListElement_t *le)
    {
        return reinterpret_cast<ListElementIterator_t*>(static_cast<void*>(&(*le)[0]));
    }
    /////////////////////////////

    const ListElementIterator_t *IteratorFromListElement(const ListElement_t *le)
    {
        return reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&(*le)[0]));
    }
    /////////////////////////////

    bool CreateListElement(ListElementTypes_t type,const void *data,unsigned int dataLength,ListElement_t *jle)
    {
        unsigned int cdrLength = 0;
        void *cdrData = 0;
        ListElementIterator_t cdrList;
        if (type == jleTypeList_k)
        {
            if (dataLength != 0) 
            {
                cdrLength = sizeof(ListElementIterator_t);
            }
            cdrList.type_ = jleTypeList_k;
            cdrList.carLen_ = 0;
            cdrList.cdrLen_ = 0;
        }

        (*jle).resize(sizeof(ListElementIterator_t)+dataLength+cdrLength);

        ListElementIterator_t *jleIterator = IteratorFromListElement(jle);
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


    bool JISPCons(const ListElement_t *car,const ListElement_t *cdr,ListElement_t *output)
    {
        ListElement_t localOutput;


        CreateListElement(jleTypeList_k,(const char *)&(*car)[0],static_cast<unsigned int>(car->size()),&localOutput);
        localOutput.resize(sizeof (ListElementIterator_t)+car->size()+cdr->size());

        ListElementIterator_t *outputIterator = IteratorFromListElement(&localOutput);
        memcpy(&(*outputIterator).data_[outputIterator->carLen_],&(*cdr)[0],cdr->size());
        outputIterator->cdrLen_ = static_cast<unsigned int>(cdr->size());
        (*output) = localOutput;

        return true;
    }
    /////////////////////////////

    bool JISPCAR(const ListElement_t *jle,ListElement_t *output)
    {
        const ListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElement_t localOutput;
        localOutput.resize(jleIterator->carLen_);
        memcpy(&localOutput[0],jleIterator->data_,jleIterator->carLen_);
        (*output) = localOutput;
        return true;
    }
    /////////////////////////////

    bool JISPCDR(const ListElement_t *jle,ListElement_t *output)
    {
        const ListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElement_t localOutput;
        localOutput.resize(jleIterator->cdrLen_);
        memcpy(&localOutput[0],&jleIterator->data_[jleIterator->carLen_],jleIterator->cdrLen_);
        (*output) = localOutput;
        return true;
    }
    /////////////////////////////


    void ListElementToStringVerboseRecurse(const ListElementIterator_t *jle,std::string *str)
    {
        if (jle->type_ == jleTypeBoolean_k)
        {
            const bool *v = reinterpret_cast<const bool *>(jle->data_);
            if (*v)
            {
                (*str) += "#t";
            }
            else
            {
                (*str) += "#f";
            }
            return;
        }
        if (jle->type_ == jleTypeQuoted_k)
        {
            (*str) += "(quote ";
            const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
            ListElementToStringVerboseRecurse(jleComponent,str);
            (*str) += ")";
            return;
        }
        if (jle->type_ == jleTypeList_k)
        {
            (*str) += "(";
            if (jle->carLen_ > 0)
            {
                const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
                ListElementToStringVerboseRecurse(jleComponent,str);
            }
            if (jle->cdrLen_ > 0)
            {
                const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[jle->carLen_]));
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

    void ListElementToStringConciseRecurse(const ListElementIterator_t *jle,std::string *str,bool car)
    {
        if (jle->type_ == jleTypeBoolean_k)
        {
            const bool *v = reinterpret_cast<const bool *>(jle->data_);
            if (*v)
            {
                (*str) += "#t";
            }
            else
            {
                (*str) += "#f";
            }
            return;
        }
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
            const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
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
                const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[0]));
                ListElementToStringConciseRecurse(jleComponent,str,true);
            }
            if (jle->cdrLen_ > 0)
            {
                const ListElementIterator_t *jleComponent = reinterpret_cast<const ListElementIterator_t*>(static_cast<const void*>(&jle->data_[jle->carLen_]));
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

    
    bool ListElementToStringConcise(const ListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const ListElementIterator_t *jleIterator = IteratorFromListElement(jle);

        if (jleIterator != 0)
        {
            ListElementToStringConciseRecurse(jleIterator,str,true);

            while ( !(*str).empty() && (*str)[(*str).length()-1] == ' ')
            {
                (*str) = (*str).substr(0,(*str).length()-1);
            }
            

            return true;
        }
        return false;
    }
    /////////////////////////////

    bool ListElementToStringVerbose(const ListElement_t *jle,std::string *str)
    {
        (*str) = "";
        const ListElementIterator_t *jleIterator = IteratorFromListElement(jle);
        ListElementToStringVerboseRecurse(jleIterator,str);

        while ( !(*str).empty() && (*str)[(*str).length()-1] == ' ')
        {
            (*str) = (*str).substr(0,(*str).length()-1);
        }
        return true;
    }
    /////////////////////////////

    typedef bool (*JISPFunctionPtr_t)(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output);
    typedef std::map<std::string,JISPFunctionPtr_t> JISPFunctionMap_t;

    typedef std::map<std::string,ListElement_t> JISPDefineMap_t;

    typedef std::stack<JISPDefineMap_t>  JISPLetStack_t;
    /////////////////////////////

    struct JISPContext_t
    {
        JISPFunctionMap_t functionMap_;
        JISPDefineMap_t defineMap_;
        JISPLetStack_t letStack_;

        JISPOutputHandler_t outputHandler_,errorHandler_;
        void *outputHandlerData_,*errorHandlerData_;

        bool active_;
    };
    /////////////////////////////

    bool GetJISPContextIsActive(JISPContext_t *context)
    {
        return context->active_;
    }
    /////////////////////////////

    void SetJISPContextErrorHandler(JISPContext_t *context,JISPOutputHandler_t eh,void *handlerData)
    {
        context->errorHandler_ = eh;
        context->errorHandlerData_ = handlerData;
    }
    /////////////////////////////

    void SetJISPContextOutputHandler(JISPContext_t *context,JISPOutputHandler_t oh,void *handlerData)
    {
        context->outputHandler_ = oh;
        context->outputHandlerData_ = handlerData;
    }
    /////////////////////////////

    void JISPOutput(JISPContext_t *context,const char *text)
    {
        if (context->outputHandler_ != 0)
        {
            (*(context->outputHandler_))(text,context->outputHandlerData_);
        }
    }
    /////////////////////////////

    void JISPError(JISPContext_t *context,const char *errorMsg)
    {
        if (context->errorHandler_ != 0)
        {
            (*(context->errorHandler_))(errorMsg,context->errorHandlerData_);
        }
    }
    /////////////////////////////

    bool JISPConsFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() == 2)
        {
            ListElementVector_t evaluatedParameters(2);
            JISP::EvaluateListElement(context,&(*parameters)[0],&evaluatedParameters[0]);
            JISP::EvaluateListElement(context,&(*parameters)[1],&evaluatedParameters[1]);

            ListElement_t localOutput;
            JISPCons(&evaluatedParameters[0],&evaluatedParameters[1],&localOutput);
            (*output) = localOutput;
            return true;
        }
        else
        {
            JISPError(context,"Error - wrong number of parameters for cons");
            return false;
        }
    }
    /////////////////////////////

    bool JISPCarFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() == 1)
        {
            ListElement_t evaluatedParameter;
            JISP::EvaluateListElement(context,&(*parameters)[0],&evaluatedParameter);
            const ListElementIterator_t *paramIterator = JISP::IteratorFromListElement(&evaluatedParameter);
            if (paramIterator->type_ == jleTypeList_k)
            {
                if (paramIterator->carLen_ == 0)
                {
                    JISPError(context,"Error - () is not a pair");
                }
                ListElement_t localOutput;
                JISPCAR(&evaluatedParameter,&localOutput);
                (*output) = localOutput;
                return true;
            }
            else
            {
                JISPError(context,"Error - bad parameter for car");
            }
        }
        else
        {
            JISPError(context,"Error - wrong number of parameters for car");
        }
        return false;
    }
    /////////////////////////////

    bool JISPCdrFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() == 1)
        {
            ListElement_t evaluatedParameter;
            JISP::EvaluateListElement(context,&(*parameters)[0],&evaluatedParameter);
            const ListElementIterator_t *paramIterator = JISP::IteratorFromListElement(&evaluatedParameter);
            if (paramIterator && paramIterator->type_ == jleTypeList_k)
            {
                if (paramIterator->cdrLen_ == 0)
                {
                    JISPError(context,"Error - () is not a pair");
                    return false;
                }
                ListElement_t localOutput;
                JISPCDR(&evaluatedParameter,&localOutput);
                (*output) = localOutput;
                return true;
            }
            else
            {
                JISPError(context,"Error - bad parameter for cdr");
            }
        }
        else
        {
            JISPError(context,"Error - wrong number of parameters for cdr");
        }
        return false;
    }
    /////////////////////////////

    bool JISPDefineFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() != 2)
        {
            JISPError(context,"Error - wrong number of parameters for define");
            return false;
        }

        const ListElementIterator_t *it = IteratorFromListElement(&(*parameters)[0]);
        
        if (it->type_ != jleTypeIdentifier_k)
        {
            JISPError(context,"Error - attempt to define non-identifier");
            return false;
        }


        ListElement_t value;
        if (EvaluateListElement(context,&(*parameters)[1],&value))
        {
            std::string idName = reinterpret_cast<const char *>(it->data_);
            context->defineMap_.erase(idName);
            context->defineMap_.insert(JISPDefineMap_t::value_type(idName,value));
            return true;
        }
        return false;
    }
    /////////////////////////////

    bool JISPSystemFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        std::string fnStr = fn;
        if (fnStr == "exit")
        {
            context->active_ = false;
            return true;
        }

        return false;
    }
    /////////////////////////////

    bool JISPLetFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() > 1)
        {
            std::string fnStr = fn;
            bool letStar = false;
            if (fnStr == "let*")
            {
                letStar = true;
            }

            ListElement_t letBindings;
            const ListElementIterator_t *it;

            it = IteratorFromListElement(&(*parameters)[0]);
            if (it->type_ != jleTypeList_k)
            {
                JISPError(context,"Error - invalid syntax let");
                return false;
            }


            letBindings = (*parameters)[0];

            const ListElementIterator_t *pit = IteratorFromListElement(&letBindings);
            if (pit->type_ != jleTypeList_k)
            {
                JISPError(context,"Error - invalid syntax let");
                return false;
            }

            JISPDefineMap_t letParameterMap,letStarParameterMap;

            if (letStar)
            {
                letStarParameterMap = context->letStack_.top();
            }

            while (pit != 0 && pit->carLen_ > 0)
            {
                ListElement_t letBinding;
                JISPCAR(&letBindings,&letBinding);
                JISPCDR(&letBindings,&letBindings);
                pit = IteratorFromListElement(&letBindings);

                const ListElementIterator_t *lit = IteratorFromListElement(&letBinding);
                if (lit->type_ != jleTypeList_k)
                {
                    JISPError(context,"Error - invalid syntax let");
                    return false;
                }


                ListElement_t expr,var;
                
                JISPCAR(&letBinding,&var);
                JISPCDR(&letBinding,&expr);
                JISPCAR(&expr,&expr);

                lit = IteratorFromListElement(&var);
                if (lit->type_ != jleTypeIdentifier_k)
                {
                    JISPError(context,"Error - invalid syntax let");
                    return false;
                }

                if (letStar)
                {
                    context->letStack_.push(letStarParameterMap);
                }

                if (!EvaluateListElement(context,&expr,&expr))
                {
                    if (letStar)
                    {
                        context->letStack_.pop();
                    }
                    JISPError(context,"Error - invalid syntax let");
                    return false;
                }


                std::string varStr = reinterpret_cast<const char *>(lit->data_);

                letParameterMap.insert(JISPDefineMap_t::value_type(varStr,expr));
                if (letStar)
                {
                    context->letStack_.pop();
                    letStarParameterMap.erase(varStr);
                    letStarParameterMap.insert(JISPDefineMap_t::value_type(varStr,expr));
                }
            }

            context->letStack_.push(letParameterMap);

            for (size_t i=1,iend=parameters->size();i<iend;++i)
            {
                ListElement_t evaluated;
                if (!EvaluateListElement(context,&(*parameters)[i],&evaluated))
                {
                    context->letStack_.pop();
                    JISPError(context,"Error - invalid syntax let");
                    return false;
                }

                if (i==iend-1)
                {
                    (*output) = evaluated;
                }                
            }
            context->letStack_.pop();

            return true;

        }
        JISPError(context,"Error - let invalid syntax");
        return false;
    }
    /////////////////////////////

    bool JISPCondFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        size_t i,iend;
        bool hasElse = false;

        ListElement_t element;

        // check for syntax errors
        for (i=0,iend=parameters->size();i<iend;++i)
        {
            JISPCAR(&(*parameters)[i],&element);
            ListElementIterator_t *it = IteratorFromListElement(&element);
            if (it != 0)
            {
                if (it->type_ == JISP::jleTypeIdentifier_k)
                {
                    std::string idStr = reinterpret_cast<char*>(it->data_);
                    if (idStr == "else")
                    {
                        if (i != iend-1)
                        {
                            JISPError(context,"Error - misplaced keyword else");
                            return false;
                        }
                        else
                        {
                            hasElse = true;
                        }
                    }
                }
                else if (it->type_ != JISP::jleTypeList_k)
                {
                    JISPError(context,"Error - invalid syntax");
                    return false;
                }
            }
        }

        // evaulate conditions
        for (i=0,iend=parameters->size();i<iend;++i)
        {
            if (i==iend-1 && hasElse)
            {
                JISPCDR(&(*parameters)[i],&element);
                JISPCAR(&element,&element);
                EvaluateListElement(context,&element,output);
                return true;
            }
            
            JISPCAR(&(*parameters)[i],&element);
            EvaluateListElement(context,&element,&element);
            ListElementIterator_t *it = IteratorFromListElement(&element);
            if (it !=0 && it->type_ == JISP::jleTypeBoolean_k)
            {
                if (*reinterpret_cast<bool*>(it->data_))
                {
                    JISPCDR(&(*parameters)[i],&element);
                    JISPCAR(&element,&element);
                    EvaluateListElement(context,&element,output);
                    return true;
                }
            }
        }
        return true;
    }
    /////////////////////////////


    bool JISPApplyLambdaFunction(JISPContext_t *context,const ListElement_t *lambdaFunc,const ListElementVector_t *parameters,ListElement_t *output)
    {
        ListElement_t lambdaParams;
        JISPCDR(lambdaFunc,&lambdaParams);
        JISPCAR(&lambdaParams,&lambdaParams);
        if (lambdaParams.empty())
        {
            JISPError(context,"Error - invalid syntax");
            return false;
        }

        ListElement_t lp,lps;
        lps = lambdaParams;
        JISPCAR(&lps,&lp);

        const ListElementIterator_t *carIterator = IteratorFromListElement(&lp);
        if (carIterator == 0)
        {
            JISPError(context,"Error - invalid syntax");
            return false;
        }

        std::vector<std::string> paramNames;

        while (carIterator != 0)
        {
            if (carIterator->type_ != jleTypeIdentifier_k)
            {
                JISPError(context,"Error - invalid syntax");
                return false;
            }
            
            paramNames.push_back(reinterpret_cast<const char *>(carIterator->data_));

            JISPCDR(&lps,&lps);
            JISPCAR(&lps,&lp);
            carIterator = IteratorFromListElement(&lp);
        }

        if (paramNames.size()!= parameters->size())
        {
            JISPError(context,"Error - incorrect parameter count");
            return false;
        }

        JISPDefineMap_t parameterMap;

        size_t i,iend;
        for (i=0,iend=paramNames.size();i<iend;++i)
        {
            ListElement_t evaluatedParameter;
            EvaluateListElement(context,&(*parameters)[i],&evaluatedParameter);
            parameterMap.insert(JISPDefineMap_t::value_type(paramNames[i],evaluatedParameter));
        }

        ListElement_t functionExpression;
        JISPCDR(lambdaFunc,&functionExpression);
        JISPCDR(&functionExpression,&functionExpression);
        JISPCAR(&functionExpression,&functionExpression);
        
        context->letStack_.push(parameterMap);
        bool ret = EvaluateListElement(context,&functionExpression,output);
        context->letStack_.pop();
        return ret;
    }
    /////////////////////////////

    bool JISPApplyFunction(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (!context->letStack_.empty())
        {
            JISPDefineMap_t &dm = context->letStack_.top();
            JISPDefineMap_t::iterator it = dm.find(std::string(fn));
            if (it != dm.end())
            {
                ListElementIterator_t *lei = IteratorFromListElement(&(*it).second);
                if (lei->type_ == jleTypeIdentifier_k)
                {
                    std::string function = reinterpret_cast<const char *>(lei->data_);
                    return JISPApplyFunction(context,function.c_str(),parameters,output);
                }
                else
                {
                    std::string err = std::string("Error - attempt to apply non-function ") +  fn;
                    JISPError(context,err.c_str());
                    return false;
                }
            }
        }

        JISPDefineMap_t::iterator dit = context->defineMap_.find(std::string(fn));
        if (dit != context->defineMap_.end())
        {
            ListElementIterator_t *lei = IteratorFromListElement(&(*dit).second);
            if (lei->type_ == jleTypeIdentifier_k)
            {
                std::string function = reinterpret_cast<const char *>(lei->data_);
                return JISPApplyFunction(context,function.c_str(),parameters,output);
            }
            else if (lei->type_ == jleTypeList_k)
            {
                std::string str;
                JISP::ListElementToStringConcise(&(*dit).second,&str);
                ListElement_t car;
                JISPCAR(&(*dit).second,&car);
                ListElementIterator_t *carIterator = IteratorFromListElement(&car);
                if (carIterator !=0 && carIterator->type_ == jleTypeIdentifier_k)
                {
                    std::string shouldBeLambda = reinterpret_cast<char *>(carIterator->data_);
                    if (shouldBeLambda == "lambda")
                    {
                        return JISPApplyLambdaFunction(context,&(*dit).second,parameters,output);
                    }
                }
                JISPError(context,"Error - attempt to apply non-function list");
                return false;
            }
            else
            {
                std::string err =  std::string("Error - attempt to apply non-function ") + fn;
                JISPError(context,err.c_str());
                return false;
            }
        }

        JISPFunctionMap_t::iterator it = context->functionMap_.find(std::string(fn));
        if (it != context->functionMap_.end())
        {
            return (*(*it).second)(context,fn,parameters,output);
        }


        std::string err = std::string("Error - unidentified function ") + fn;
        JISPError(context,err.c_str());
        return false;
    }
    ////////////////////////////


    struct JISPRational_t
    {
        int first,second;
    };

    union JISPNumberUnion_t
    {
        float float_;
        int int_;
        JISPRational_t rational_;
    };

    struct JISPNumber_t
    {
        ListElementTypes_t type_;
        JISPNumberUnion_t number_;
    };


    JISPNumber_t JISPEvaluateNumber(const ListElement_t &element)
    {
        JISPNumber_t number;
        number.type_ = jleTypeUnknown_k;

        const ListElementIterator_t *lei = IteratorFromListElement(&element);

        if (lei->type_ == jleTypeInteger_k)
        {
            number.type_ = lei->type_;
            number.number_.int_ = atoi(reinterpret_cast<const char *>(lei->data_));
        }

        if (lei->type_ == jleTypeFloat_k)
        {
            number.type_ = lei->type_;
            number.number_.float_ = static_cast<float>(atof(reinterpret_cast<const char *>(lei->data_)));
        }

        if (lei->type_ == jleTypeRational_k)
        {
            number.type_ = lei->type_;
            int numer,denom;
            const char *rational = reinterpret_cast<const char *>(lei->data_);
            scanf("%d/%d",&numer,&denom);
            number.number_.rational_.first = numer;
            number.number_.rational_.second = denom;
        }

        return number;
    }
    static ListElementTypes_t EvaluateNumericalParameters(JISPContext_t *context,const ListElementVector_t *parameters,std::vector<JISPNumber_t> &numbers)
    {
        size_t i,iend;
        numbers.clear();
        ListElementTypes_t type = jleTypeInteger_k;

        for (i=0,iend=parameters->size();i<iend;++i)
        {
            numbers.push_back(JISPEvaluateNumber((*parameters)[i]));
            if (static_cast<int>(numbers.back().type_) > static_cast<int>(type))
            {
                type = numbers.back().type_;
            }
            if (type == jleTypeUnknown_k)
            {
                JISPError(context,"Error - unsupported type for function ");
                return type;
            }
        }

        if (type == jleTypeFloat_k)
        {
            for (i=0,iend=numbers.size();i<iend;++i)
            {
                if (numbers[i].type_ == jleTypeInteger_k)
                {
                    numbers[i].number_.float_ = static_cast<float>(numbers[i].number_.int_);                    
                    numbers[i].type_ = jleTypeFloat_k;
                }
            }
        }
        return type;
    }

    JISPNumber_t operator+(const JISPNumber_t &op1,const JISPNumber_t &op2)
    {
        const JISPNumber_t *pop1 = &op1;
        const JISPNumber_t *pop2 = &op2;;

        JISPNumber_t aop;

        if (op1.type_ != op2.type_)
        {
            if (static_cast<int>(op1.type_) > static_cast<int>(op2.type_))
            {
                pop2 = &aop;
                aop.type_ = op2.type_;
                if (op1.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
            if (static_cast<int>(op2.type_) > static_cast<int>(op1.type_))
            {
                pop1 = &aop;
                aop.type_ = op1.type_;
                if (op2.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
        }


        JISPNumber_t ret;
        ret.type_ = pop1->type_;

        if (pop1->type_ == jleTypeInteger_k)
        {
            ret.number_.int_ = pop1->number_.int_ + pop2->number_.int_;
        }
        if (pop2->type_ == jleTypeFloat_k)
        {
            ret.number_.float_ = pop1->number_.float_ + pop2->number_.float_;
        }

        return ret;
    }

    JISPNumber_t operator-(const JISPNumber_t &op1,const JISPNumber_t &op2)
    {
        const JISPNumber_t *pop1 = &op1;
        const JISPNumber_t *pop2 = &op2;;

        JISPNumber_t aop;

        if (op1.type_ != op2.type_)
        {
            if (static_cast<int>(op1.type_) > static_cast<int>(op2.type_))
            {
                pop2 = &aop;
                aop.type_ = op2.type_;
                if (op1.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
            if (static_cast<int>(op2.type_) > static_cast<int>(op1.type_))
            {
                pop1 = &aop;
                aop.type_ = op1.type_;
                if (op2.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
        }


        JISPNumber_t ret;
        ret.type_ = pop1->type_;

        if (pop1->type_ == jleTypeInteger_k)
        {
            ret.number_.int_ = pop1->number_.int_ - pop2->number_.int_;
        }
        if (pop2->type_ == jleTypeFloat_k)
        {
            ret.number_.float_ = pop1->number_.float_ - pop2->number_.float_;
        }

        return ret;
    }

    JISPNumber_t operator*(const JISPNumber_t &op1,const JISPNumber_t &op2)
    {
        const JISPNumber_t *pop1 = &op1;
        const JISPNumber_t *pop2 = &op2;;

        JISPNumber_t aop;

        if (op1.type_ != op2.type_)
        {
            if (static_cast<int>(op1.type_) > static_cast<int>(op2.type_))
            {
                pop2 = &aop;
                aop.type_ = op2.type_;
                if (op1.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
            if (static_cast<int>(op2.type_) > static_cast<int>(op1.type_))
            {
                pop1 = &aop;
                aop.type_ = op1.type_;
                if (op2.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
        }


        JISPNumber_t ret;
        ret.type_ = pop1->type_;

        if (pop1->type_ == jleTypeInteger_k)
        {
            ret.number_.int_ = pop1->number_.int_ * pop2->number_.int_;
        }
        if (pop2->type_ == jleTypeFloat_k)
        {
            ret.number_.float_ = pop1->number_.float_ * pop2->number_.float_;
        }

        return ret;
    }

    JISPNumber_t operator/(const JISPNumber_t &op1,const JISPNumber_t &op2)
    {
        const JISPNumber_t *pop1 = &op1;
        const JISPNumber_t *pop2 = &op2;;

        JISPNumber_t aop;

        if (op1.type_ != op2.type_)
        {
            if (static_cast<int>(op1.type_) > static_cast<int>(op2.type_))
            {
                pop2 = &aop;
                aop.type_ = op2.type_;
                if (op1.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
            if (static_cast<int>(op2.type_) > static_cast<int>(op1.type_))
            {
                pop1 = &aop;
                aop.type_ = op1.type_;
                if (op2.type_ == jleTypeFloat_k)
                {
                    if (aop.type_ == jleTypeInteger_k)
                    {
                        aop.type_ = jleTypeFloat_k;
                        aop.number_.float_ = static_cast<float>(aop.number_.int_);
                    }
                }
            }
        }


        JISPNumber_t ret;
        ret.type_ = pop1->type_;

        if (pop1->type_ == jleTypeInteger_k)
        {
            ret.number_.int_ = pop1->number_.int_ / pop2->number_.int_;
        }
        if (pop2->type_ == jleTypeFloat_k)
        {
            ret.number_.float_ = pop1->number_.float_ / pop2->number_.float_;
        }

        return ret;
    }

    bool JISPArithmeticFunction(JISPContext_t *context, const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() < 2)
        {
            std::string err = std::string("Error - incorrect number of parameters to function ") + fn;
            JISPError(context,err.c_str());
            return false;
        }

        size_t i,iend;
        ListElementVector_t evaluatedParameters(parameters->size());

        for (i=0,iend=parameters->size();i<iend;++i)
        {
            JISP::EvaluateListElement(context,&(*parameters)[i],&evaluatedParameters[i]);
        }


        std::string fnStr = fn;
        std::vector<JISPNumber_t> numbers;

        ListElementTypes_t type = EvaluateNumericalParameters(context,&evaluatedParameters,numbers);

        if (type == jleTypeUnknown_k)
        {
            return false;
        }

        JISPNumber_t result = numbers[0];
        for (i=1,iend=numbers.size();i<iend;++i)
        {
            JISPNumber_t &operand = numbers[i];
            if (fnStr == "+")
            {
                result = result + operand;
            }
            if (fnStr == "-")
            {
                result = result - operand;
            }
            if (fnStr == "/")
            {
                result = result / operand;
            }
            if (fnStr == "*")
            {
                result = result * operand;
            }

        }

        char buf[0xff];
        if (result.type_ == jleTypeInteger_k)
        {
            sprintf(buf,"%d",result.number_.int_);
        }
        if (result.type_ == jleTypeFloat_k)
        {
            sprintf(buf,"%f",result.number_.float_);
        }


        JISP::CreateListElement(result.type_,buf,static_cast<unsigned int>(strlen(buf)+1)*sizeof(buf[0]),output);
        return true;
    }

    bool JISPUnaryQuery(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        std::string fnStr = fn;
        if (parameters->size() > 1)
        {
            std::string err = std::string("Error - incorrect number of parameters to function ") + fn;
            JISPError(context,err.c_str());
            return false;
        }

        JISP::ListElement_t evaluatedParameter;

        bool result = false;
        
        if (!JISP::EvaluateListElement(context,&(*parameters)[0],&evaluatedParameter))
        {
            std::string err = std::string("Error - unidentified parameter in function ") + fn;
            JISPError(context,err.c_str());
        }
        else
        {

            const ListElementIterator_t *it = IteratorFromListElement(&evaluatedParameter);


            if (fnStr == "atom?")
            {
                result = (it->type_ != jleTypeList_k);
            }

            if (fnStr == "list?")
            {
                result = (it->type_ == jleTypeList_k);
            }

            if (fnStr == "null?")
            {
                result = ( (it->type_ == jleTypeList_k) && (it->carLen_ == 0 && it->cdrLen_ == 0) );
            }
        }

        CreateListElement(jleTypeBoolean_k,&result,sizeof(bool),output);


        return true;
    }



    bool JISPBinaryComparison(JISPContext_t *context,const char *fn,const ListElementVector_t *parameters,ListElement_t *output)
    {
        if (parameters->size() < 2)
        {
            std::string err = std::string("Error - incorrect number of parameters to function ") + fn;
            JISPError(context,err.c_str());
            return false;
        }

        std::string fnStr = fn;
        std::vector<JISPNumber_t> numbers;

        size_t i,iend;

        ListElementVector_t evaluatedParameters(parameters->size());
        for (i=0,iend=parameters->size();i<iend;++i)
        {
            JISP::EvaluateListElement(context,&(*parameters)[i],&evaluatedParameters[i]);
        }


        bool result = true;

        ListElementTypes_t type = EvaluateNumericalParameters(context,&evaluatedParameters,numbers);
        if (type == jleTypeUnknown_k)
        {
            return false;
        }



        for (i=0,iend=numbers.size()-1;i<iend;++i)
        {
            if (type == jleTypeInteger_k)
            {
                if (fnStr == ">")
                {
                    result = result && (numbers[i].number_.int_ > numbers[i+1].number_.int_);
                }
                if (fnStr == "<")
                {
                    result = result && (numbers[i].number_.int_ < numbers[i+1].number_.int_);
                }
                if (fnStr == ">=")
                {
                    result = result && (numbers[i].number_.int_ >= numbers[i+1].number_.int_);
                }
                if (fnStr == "<=")
                {
                    result = result && (numbers[i].number_.int_ <= numbers[i+1].number_.int_);
                }
                if (fnStr == "eq?" || fnStr == "=")
                {
                    result = result && (numbers[i].number_.int_ == numbers[i+1].number_.int_);
                }
            }

            if (type == jleTypeFloat_k)
            {
                if (fnStr == ">")
                {
                    result = result && (numbers[i].number_.float_ > numbers[i+1].number_.float_);
                }
                if (fnStr == "<")
                {
                    result = result && (numbers[i].number_.float_ < numbers[i+1].number_.float_);
                }
                if (fnStr == ">=")
                {
                    result = result && (numbers[i].number_.float_ >= numbers[i+1].number_.float_);
                }
                if (fnStr == "<=")
                {
                    result = result && (numbers[i].number_.float_ <= numbers[i+1].number_.float_);
                }
                if (fnStr == "eq?" || fnStr == "=")
                {
                    result = result && (numbers[i].number_.float_ == numbers[i+1].number_.float_);
                }
            }

        }
        

        CreateListElement(jleTypeBoolean_k,&result,sizeof(bool),output);


        return true;
    }
    ////////////////////////////

    bool EvaluateListElement(JISPContext_t *context,const ListElement_t *input,ListElement_t *output)
    {
        const ListElementIterator_t *inputIterator = IteratorFromListElement(input);

        if (inputIterator->type_ == jleTypeIdentifier_k)
        {
            std::string function = reinterpret_cast<const char *>(inputIterator->data_);

            if (!context->letStack_.empty())
            {
                JISPDefineMap_t &dm = context->letStack_.top();
                JISPDefineMap_t::iterator it = dm.find(function);
                if (it != dm.end())
                {
                    (*output) = (*it).second;
                    return true;
                }
            }

            JISPDefineMap_t::iterator dit = context->defineMap_.find(function);
            if (dit != context->defineMap_.end())
            {
                (*output) = (*dit).second;
                return true;
            }

            JISPFunctionMap_t::iterator fit = context->functionMap_.find(function);
            if (fit != context->functionMap_.end())
            {
                (*output) = (*input);
                return true;
            }

            std::string err = std::string("Error - undefined identifier ") + function;
            JISPError(context,err.c_str());
            return false;
        }

        if (inputIterator->type_ == jleTypeString_k ||
            inputIterator->type_ == jleTypeInteger_k ||
            inputIterator->type_ == jleTypeFloat_k)
        {
            (*output) = (*input);
            return true;
        }

        if (inputIterator->type_ == jleTypeBoolean_k)
        {
            std::string boolStr = reinterpret_cast<const char *>(inputIterator->data_);
            bool val = (boolStr == "#t" || boolStr == "#T");
            JISP::CreateListElement(jleTypeBoolean_k,&val,sizeof(val),output);
            return true;
        }

        if (inputIterator->type_ == jleTypeQuoted_k)
        {
            const ListElementIterator_t *dataIterator = reinterpret_cast<const ListElementIterator_t *>(inputIterator->data_);
            ListElement_t localOutput;
            size_t elementSize = dataIterator->carLen_ + dataIterator->cdrLen_ + sizeof(ListElementIterator_t);
            localOutput.resize(elementSize);
            memcpy(&localOutput[0],dataIterator,elementSize);
            (*output) = localOutput;
            return true;
        }


        if (inputIterator->type_ == jleTypeList_k)
        {
            ListElement_t car,cdr;
            ListElementVector_t parameters;

            if (inputIterator->carLen_ > 0)
            {
                JISPCAR(input,&car);
                ListElementIterator_t *carIterator = IteratorFromListElement(&car);
                if (carIterator->type_ == jleTypeIdentifier_k)
                {
                    JISPCDR(input,&cdr);
                    std::string function = reinterpret_cast<const char *>(carIterator->data_);

                    if (function == "lambda")
                    {
                        (*output) = (*input);
                        return true;
                    }

                    ListElementIterator_t *cdrIterator = IteratorFromListElement(&cdr);
                    while(cdrIterator->carLen_ > 0)
                    {
                        JISPCAR(&cdr,&car);
                        JISPCDR(&cdr,&cdr);

                        cdrIterator = IteratorFromListElement(&cdr);

                        parameters.push_back(car);
                    }

                    return JISPApplyFunction(context,function.c_str(),&parameters,output);

                }
                else if (carIterator->type_ == jleTypeList_k)
                {
                    ListElement_t shouldBeLambda;
                    JISPCAR(&car,&shouldBeLambda);
                    ListElementIterator_t *sblit = IteratorFromListElement(&shouldBeLambda);
                    if (sblit!=0 && sblit->type_ == jleTypeIdentifier_k)
                    {
                        std::string sblStr = reinterpret_cast<char *>(sblit->data_);
                        if (sblStr == "lambda")
                        {
                            ListElement_t lambdaFunc = car;

                            JISPCDR(input,&cdr);
                            ListElementIterator_t *cdrIterator = IteratorFromListElement(&cdr);
                            while(cdrIterator->carLen_ > 0)
                            {
                                JISPCAR(&cdr,&car);
                                JISPCDR(&cdr,&cdr);

                                cdrIterator = IteratorFromListElement(&cdr);

                                parameters.push_back(car);
                            }
                            return JISPApplyLambdaFunction(context,&lambdaFunc,&parameters,output);
                        }
                    }
                    return false;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                JISPError(context,"Invalid Syntax ()");
                return false;
            }
        }
        return false;
    }
    ////////////////////////////

    JISPContext_t *CreateJISPContext()
    {
        JISPContext_t *ret = new JISPContext_t();

        ret->errorHandler_ = 0;
        ret->errorHandlerData_ = 0;
        ret->outputHandler_ = 0;
        ret->outputHandlerData_ = 0;
        ret->active_ = true;
        
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("cons",JISPConsFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("car",JISPCarFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("cdr",JISPCdrFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("eq?",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("=",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type(">",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type(">=",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("<",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("<=",JISPBinaryComparison));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("+",JISPArithmeticFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("-",JISPArithmeticFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("*",JISPArithmeticFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("/",JISPArithmeticFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("define",JISPDefineFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("atom?",JISPUnaryQuery));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("list?",JISPUnaryQuery));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("null?",JISPUnaryQuery));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("exit",JISPSystemFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("cond",JISPCondFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("let",JISPLetFunction));
        ret->functionMap_.insert(JISPFunctionMap_t::value_type("let*",JISPLetFunction));

        bool bsf = BuildStandardFunctions(ret);
        assert(bsf);


        return ret;
    }
    ////////////////////////////
    
    void DestroyJISPContext(JISPContext_t *context)
    {
        delete context;
    }
    ////////////////////////////




}