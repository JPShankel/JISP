/*
** JISPStackMachine.h
**
** JISPStackMachine.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/


#ifndef JISPSTACKMACHINE_H
#define JISPSTACKMACHINE_H

#include <stack>
#include <vector>

namespace JISP {

typedef enum
{
    Add_k,
    Subtract_k,
    Multiply_k,
    Divide_k,
    Power_k,
    PushLiteral_k,
    PushVariable_k,
    Pop_k,
    Assign_k,
    NoOp_k,
    PushRandom_k,
    GreaterThan_k,
    LessThan_k,
    GreaterThanOrEqual_k,
    LessThanOrEqual_k,
    Equal_k,
    NotEqual_k,
    If_k,
    Else_k,
    Endif_k,
    And_k,
    Or_k,
    Goto_k,
    Gosub_k,
    Return_k,
    Floor_k,
    Ceil_k,
    Sin_k,
    Cos_k,
    Tan_k,
    ASin_k,
    ACos_k,
    ATan_k
} StackOpcode_t;

struct StackInstruction_t
{
    StackOpcode_t operation_;
    float literal_;
    int variable_;
    StackInstruction_t(StackOpcode_t op = NoOp_k,float v = 0) : operation_(op),literal_(v),variable_(-1){}
    StackInstruction_t(StackOpcode_t op,int v) : operation_(op),literal_(0),variable_(v){}
};

class StackMachine
{
private:
    std::stack<float> stack_;
    std::stack<int> gosubStack_;
public:
    int Evaluate(const std::vector<StackInstruction_t> &formula,float *variables,int start = -1,int end = -1);

};

}

#endif // JISPSTACKMACHINE_H