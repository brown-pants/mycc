#ifndef __CSE_H__
#define __CSE_H__

#include "DataFlow.h"
#include <set>

class CSE
{
public:
    CSE(std::vector<TACGenerator::Quaternion> &tac);
    void opt();

private:
    std::vector<TACGenerator::Quaternion> &tac;
    DFG dfg;
    std::set<int> m_invalidTacIndices;
    std::unordered_set<std::string> m_globalPtrs;
    std::unordered_set<std::string> m_pointers;
    std::unordered_map<std::string, long long> m_globlArr;
    std::unordered_map<std::string, long long> m_arrays;
    std::unordered_map<std::string, std::string> m_replaceVars;
    std::unordered_map<Block::ptr_t, std::set<TACGenerator::Quaternion>> IN;
    std::unordered_map<Block::ptr_t, std::set<TACGenerator::Quaternion>> OUT;
    std::unordered_map<Block::ptr_t, std::set<TACGenerator::Quaternion>> GEN;
    std::unordered_map<Block::ptr_t, std::set<std::string>> KILL;
    std::unordered_map<Block::ptr_t, std::unordered_map<std::string, std::string>> PTR_IN;
    std::unordered_map<Block::ptr_t, std::unordered_map<std::string, std::string>> PTR_OUT;

    void kill(Block::ptr_t block, const std::string &kill_value, std::set<TACGenerator::Quaternion> &dest);
    bool subExprTest(const std::set<TACGenerator::Quaternion> &available, const TACGenerator::Quaternion &expr);
    void replaceVars(TACGenerator::Quaternion &code);
    void cse(const std::vector<Block::ptr_t> &depthOrderBlocks);
    void computeGenKill(const std::vector<Block::ptr_t> &depthOrderBlocks);
    void computeInOut(const std::vector<Block::ptr_t> &depthOrderBlocks);
    void computePtrInOut(const std::vector<Block::ptr_t> &depthOrderBlocks);
    void handlePtrAssgin(Block::ptr_t block, const TACGenerator::Quaternion &code);
    void recordGlobalArraysPointers();
};

#endif