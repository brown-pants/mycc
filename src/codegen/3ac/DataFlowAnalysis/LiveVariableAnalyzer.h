#ifndef __LIVEVARIABLEANALYZER_H__
#define __LIVEVARIABLEANALYZER_H__

#include "DataFlow.h"
#include <map>
#include <set>

struct Interval
{
    int begin;
    int end;
};

struct VarNameCompare
{
    bool operator()(const std::string& v1, const std::string& v2) const;
};

class LiveVariableAnalyzer
{
public:
    LiveVariableAnalyzer(std::vector<TACGenerator::Quaternion> &tac);
    std::map<std::string, Interval, VarNameCompare> result() const;
    void opt();

private:
    std::vector<TACGenerator::Quaternion> &tac;
    DFG dfg;
    std::unordered_map<Block::ptr_t, std::unordered_set<std::string>> IN; // Active variables after block B
    std::unordered_map<Block::ptr_t, std::unordered_set<std::string>> OUT;
    std::unordered_map<Block::ptr_t, std::unordered_set<std::string>> USE;
    std::unordered_map<Block::ptr_t, std::unordered_set<std::string>> DEF; // Defined variables after block B
    std::map<std::string, Interval, VarNameCompare> m_liveIntervals;
    
    void computeDef(const std::vector<Block::ptr_t> &reverseDepthOrderBlocks);
    void computeInOut(const std::vector<Block::ptr_t> &reverseDepthOrderBlocks);
    void computeLiveIntervals(const std::vector<Block::ptr_t> &depthOrderBlocks);
    std::string getDef(const TACGenerator::Quaternion &code);
    std::set<std::string> getUse(const TACGenerator::Quaternion &code);
    bool analyzable(const std::string &var);
};

#endif