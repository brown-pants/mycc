#ifndef __TACOPTIMIZER_H__
#define __TACOPTIMIZER_H__

#include "TACGenerator.h"
#include <set>

struct Block
{
    std::vector<int> tac_indices;
    std::unordered_set<Block *> prevs;
    std::unordered_set<Block *> succs;
};

class TACOptimizer
{
public:
    TACOptimizer(std::vector<TACGenerator::Quaternion> &tac);
    void opt();

private:
    std::vector<TACGenerator::Quaternion> &tac;

    std::unordered_map<std::string, std::string> m_replaceLabels;
    std::unordered_set<std::string> m_validLabels;
    void optGotoLabel();

    std::vector<Block *> m_CFG;
    std::unordered_map<std::string, Block *> m_labelBlocks;
    std::unordered_map<std::string, std::vector<Block *>> m_jumpBlocks;
    Block *curBlock;
    void genCFG();
    std::unordered_map<std::string, long long> m_globlArr;

    std::unordered_map<Block *, std::set<TACGenerator::Quaternion>> m_OUT;
    std::unordered_map<std::string, long long> m_arrays;
    std::unordered_map<std::string, std::string> m_ptrRecords;
    std::unordered_map<std::string, std::string> m_replaceVars;
    std::unordered_set<Block *> m_visitedBlocks;
    std::set<int> m_invalidTacIndices;
    void cse();   //  Common Subexpression Elimination
    void cse_area(Block *block);
    bool subExprTest(const std::set<TACGenerator::Quaternion> &_in, const TACGenerator::Quaternion &expr);
    void replaceVars(TACGenerator::Quaternion &code);
    void kill(std::set<TACGenerator::Quaternion> &_in, const std::string &var);
    void killExceptConstant(std::set<TACGenerator::Quaternion> &_in);
};

#endif