#ifndef __TACOPTIMIZER_H__
#define __TACOPTIMIZER_H__

#include "TACGenerator.h"

class TACOptimizer
{
public:
    TACOptimizer();
    void opt(std::vector<TACGenerator::Quaternion> &tac);

private:
    std::unordered_map<std::string, std::string> m_replaceLabels;
    std::unordered_set<std::string> m_validLabels;
    void optGotoLabel(std::vector<TACGenerator::Quaternion> &tac);
};

#endif