#ifndef __TACOPTIMIZER_H__
#define __TACOPTIMIZER_H__

#include "TACGenerator.h"

class TACOptimizer
{
public:
    TACOptimizer(std::vector<TACGenerator::Quaternion> &tac);
    void opt();

private:
    std::vector<TACGenerator::Quaternion> &tac;
    void optGotoLabel();
};

#endif