/*
** JISPStackMachine.h
**
** JISPStackMachine.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#include "JISPStackMachine.h"

#include <stdlib.h>
#include <math.h>

namespace JISP
{
    float RandomUnitInterval()
    {
        return static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    }



int StackMachine::Evaluate(const std::vector<StackInstruction_t>& formula, float* variables, int start, int end)
{
   if (-1 == start)
   {
      start = 0;
   }
   if (-1 == end)
   {
      end = static_cast<int>(formula.size());
   }
    float v1,v2;
   size_t i = start;
    for (;(int)i<end;i++)
    {
        switch(formula[i].operation_)
        {
            case Floor_k :
            {
                v1 = stack_.top();
                stack_.pop();
                v1 = floorf(v1);
                stack_.push(v1);
            }
            break;

            case Ceil_k :
            {
                v1 = stack_.top();
                stack_.pop();
                v1 = ceilf(v1);
                stack_.push(v1);
            }
            break;

            case Sin_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = sinf(v1);
                    stack_.push(v1);
                }
                break;
            case Cos_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = cosf(v1);
                    stack_.push(v1);
                }
                break;
            case Tan_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = tanf(v1);
                    stack_.push(v1);
                }
                break;
            case ASin_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = asinf(v1);
                    stack_.push(v1);
                }
                break;
            case ACos_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = acosf(v1);
                    stack_.push(v1);
                }
                break;
            case ATan_k :
                {
                    v1 = stack_.top();
                    stack_.pop();
                    v1 = atanf(v1);
                    stack_.push(v1);
                }
                break;

   
            case Return_k :
                if (0 != gosubStack_.size())
                {
                    i = gosubStack_.top();
                    gosubStack_.pop();
                }
                else
                {
                    return static_cast<int>(i+1);
                }
                break;

            case Gosub_k :
                gosubStack_.push(static_cast<int>(i));
                i = formula[i].variable_-1;
                break;

                case Goto_k :
                i = formula[i].variable_-1;
                break;

            case And_k :
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                if (v1 > 0.00001f && v2 > 0.0001f)
                {
                    stack_.push(1);
                }
                else
                {
                    stack_.push(0);
                }
                break;

            case Or_k :
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                if (v1 > 0.00001f || v2 > 0.0001f)
                {
                    stack_.push(1);
                }
                else
                {
                    stack_.push(0);
                }
                break;

            case Endif_k:
                break;

            case If_k:
                {
                v1 = stack_.top();
                stack_.pop();
                int ifcount = 0;
                int size = static_cast<int>(formula.size());
                if (v1 < 1)
                {
                    i++;
                    while ((i<formula.size() && formula[i].operation_ != Endif_k && formula[i].operation_ != Else_k) || ifcount > 0)
                    {
                        if (formula[i].operation_ == If_k)
                        {
                            ifcount++;
                        }
                        if (formula[i].operation_ == Endif_k)
                        {
                            ifcount--;
                        }
                        i++;
                    }
                }
                }
                break;

            case Else_k:
                {
                int ifcount=0;
                size_t s = formula.size();
                while ((i<formula.size() && formula[i].operation_ != Endif_k) || ifcount >= 0)
                {
                    i++;
                    if (formula[i].operation_ == If_k)
                    {
                        ifcount++;
                    }
                    if (formula[i].operation_ == Endif_k)
                    {
                        ifcount--;
                    }
                }
                }
                break;

            case Add_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                stack_.push(v2+v1);
                break;

            case Subtract_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                stack_.push(v2-v1);
                break;

            case Multiply_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                stack_.push(v2*v1);
                break;

            case Divide_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                stack_.push(v2/v1);
                break;

            case PushLiteral_k :
                stack_.push(formula[i].literal_);
                break;

            case PushVariable_k :
                v1 = variables[formula[i].variable_];
                stack_.push(variables[formula[i].variable_]);
                break;

            case PushRandom_k:
                v1 = RandomUnitInterval();
                stack_.push(v1);
                break;

            case Pop_k:
                v1 = stack_.top();
                variables[formula[i].variable_] = stack_.top();
                stack_.pop();
                break;

            case Assign_k:
                v1 = stack_.top();
                variables[formula[i].variable_] = stack_.top();
                stack_.pop();
                break;

            case Power_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                stack_.push(pow(v2,v1));
                break;

            case GreaterThan_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = v2 > v1;
                stack_.push(v1);
                break;

            case LessThan_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = v2 < v1;
                stack_.push(v1);
                break;

            case GreaterThanOrEqual_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = v2 >= v1;
                stack_.push(v1);
                break;
            
            case LessThanOrEqual_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = v2 <= v1;
                stack_.push(v1);
                break;

            case Equal_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = fabsf(v2-v1) < 0.0001f;
                stack_.push(v1);
                break;

            case NotEqual_k:
                v1 = stack_.top();
                stack_.pop();
                v2 = stack_.top();
                stack_.pop();
                v1 = fabsf(v2-v1) > 0.0001f;
                stack_.push(v1);
                break;
        }; //switch
    } //for
    return static_cast<int>(i);
}
}