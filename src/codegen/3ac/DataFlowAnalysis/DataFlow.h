#ifndef __DATAFLOW_H__
#define __DATAFLOW_H__

#include "../TACGenerator.h"
#include <memory>

struct Block
{
    using ptr_t = std::shared_ptr<Block>;
    std::vector<int> tac_indices;
    std::unordered_set<ptr_t> prevs;
    std::unordered_set<ptr_t> succs;
};

class DFG
{
public:
    DFG(const std::vector<TACGenerator::Quaternion> &tac);
    Block::ptr_t getEntry(int index);
    inline int count() const { return entries.size(); }

private:
    std::vector<Block::ptr_t> entries;
};

class DataFlow
{
public:
    static std::vector<Block::ptr_t> depthFirstOrder(Block::ptr_t entry);
    static std::vector<Block::ptr_t> reverseDepthFirstOrder(Block::ptr_t entry);
    static std::vector<std::vector<Block::ptr_t>> findPaths(Block::ptr_t start, Block::ptr_t dest);
    static Block::ptr_t findExitBlocks(Block::ptr_t entry);
    
private:
    DataFlow() {}
    static void depthFirst(Block::ptr_t block, std::vector<Block::ptr_t> &order, std::unordered_set<Block::ptr_t>& visited);
    static void reverseDepthFirst(Block::ptr_t block, std::vector<Block::ptr_t> &order, std::unordered_set<Block::ptr_t>& visited);
    static void dfsFindPaths(Block::ptr_t current, Block::ptr_t dest, std::vector<Block::ptr_t>& current_path, std::unordered_set<Block::ptr_t>& visited, std::vector<std::vector<Block::ptr_t>>& all_paths);
};

#endif