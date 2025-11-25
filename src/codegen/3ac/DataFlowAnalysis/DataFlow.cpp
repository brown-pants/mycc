#include "DataFlow.h"
#include <cassert>
#include <queue>

DFG::DFG(const std::vector<TACGenerator::Quaternion> &tac)
{
    Block::ptr_t curBlock;
    std::unordered_map<std::string, Block::ptr_t> labelBlocks;
    std::unordered_map<std::string, std::vector<Block::ptr_t>> jumpBlocks;
    bool sign = false;

    for (int idx = 0; idx < tac.size(); idx ++)
    {
        const TACGenerator::Quaternion &code = tac[idx];
        if (code.op == TACGenerator::Op_begin_func)
        {
            Block::ptr_t block = Block::ptr_t(new Block());
            entries.push_back(block);
            curBlock = block;
            sign = true;
        }
        else if (sign == false)
        {
            continue;
        }
        else if (code.op == TACGenerator::Op_end_func)
        {
            sign = false;
        }
        else if (code.op == TACGenerator::Op_label)
        {
            Block::ptr_t block = Block::ptr_t(new Block());
            block->tac_indices.push_back(idx);
            if (curBlock) 
            {
                curBlock->succs.insert(block);
                block->prevs.insert(curBlock);
            }
            labelBlocks[code.result] = block;
            auto iter = jumpBlocks.find(code.result);
            if (iter != jumpBlocks.end())
            {
                for (Block::ptr_t pre_block : iter->second)
                {
                    pre_block->succs.insert(block);
                    block->prevs.insert(pre_block);
                }
                jumpBlocks.erase(iter);
            }
            curBlock = block;
        }
        else if (code.op == TACGenerator::Op_goto || code.op == TACGenerator::Op_if)
        {
            if (!curBlock)
            {
                continue;
            }
            auto iter = labelBlocks.find(code.result);
            if (iter != labelBlocks.end())
            {
                Block::ptr_t succBlock = iter->second;
                curBlock->succs.insert(succBlock);
                succBlock->prevs.insert(curBlock);
            }
            else if (jumpBlocks.find(code.result) != jumpBlocks.end())
            {
                jumpBlocks[code.result].push_back(curBlock);
            }
            else
            {
                jumpBlocks[code.result] = std::vector<Block::ptr_t>({ curBlock });
            }
            if (code.op == TACGenerator::Op_goto)
            {
                curBlock = nullptr;
            }
            else
            {
                curBlock->tac_indices.push_back(idx);
            }
        }
        else if (curBlock)
        {
            curBlock->tac_indices.push_back(idx);
        }
    }
}

Block::ptr_t DFG::getEntry(int index)
{
    assert(index >= 0 && index < count());
    return entries[index];
}

std::vector<Block::ptr_t> DataFlow::depthFirstOrder(Block::ptr_t entry)
{
    std::vector<Block::ptr_t> order;
    std::unordered_set<Block::ptr_t> visited;
    depthFirst(entry, order, visited);
    return order;
}

void DataFlow::depthFirst(Block::ptr_t block, std::vector<Block::ptr_t> &order, std::unordered_set<Block::ptr_t>& visited)
{
    if (visited.find(block) != visited.end()) return;
    
    visited.insert(block);
    order.push_back(block);
    
    for (Block::ptr_t succ : block->succs) 
    {
        depthFirst(succ, order, visited);
    }
}

std::vector<Block::ptr_t> DataFlow::reverseDepthFirstOrder(Block::ptr_t entry)
{
    std::vector<Block::ptr_t> order;
    std::unordered_set<Block::ptr_t> visited;
    reverseDepthFirst(findExitBlocks(entry), order, visited);
    return order;
}

void DataFlow::reverseDepthFirst(Block::ptr_t block, std::vector<Block::ptr_t> &order, std::unordered_set<Block::ptr_t>& visited)
{
    if (visited.find(block) != visited.end()) return;
    
    visited.insert(block);
    order.push_back(block);
    
    for (Block::ptr_t prev : block->prevs) 
    {
        reverseDepthFirst(prev, order, visited);
    }
    
}

std::vector<std::vector<Block::ptr_t>> DataFlow::findPaths(Block::ptr_t start, Block::ptr_t dest)
{
    std::vector<std::vector<Block::ptr_t>> all_paths;
    std::vector<Block::ptr_t> current_path;
    std::unordered_set<Block::ptr_t> visited;
    
    dfsFindPaths(start, dest, current_path, visited, all_paths);
    return all_paths;
}

void DataFlow::dfsFindPaths(Block::ptr_t current, Block::ptr_t dest, std::vector<Block::ptr_t>& current_path, std::unordered_set<Block::ptr_t>& visited, std::vector<std::vector<Block::ptr_t>>& all_paths)
{
    current_path.push_back(current);
    visited.insert(current);
    
    if (current == dest) 
    {
        all_paths.push_back(current_path);
    } 
    else 
    {
        for (Block::ptr_t succ : current->succs) 
        {
            if (visited.find(succ) == visited.end()) 
            {
                dfsFindPaths(succ, dest, current_path, visited, all_paths);
            }
        }
    }
    
    current_path.pop_back();
    visited.erase(current);
}

Block::ptr_t DataFlow::findExitBlocks(Block::ptr_t entry)
{
    Block::ptr_t exitBlock;
    std::unordered_set<Block::ptr_t> visited;
    std::queue<Block::ptr_t> queue;
    
    queue.push(entry);
    visited.insert(entry);
    
    while (!queue.empty()) 
    {
        Block::ptr_t current = queue.front();
        queue.pop();
        
        if (current->succs.empty()) 
        {
            exitBlock = current;
            break;
        }
        
        for (Block::ptr_t succ : current->succs) 
        {
            if (visited.find(succ) == visited.end()) 
            {
                visited.insert(succ);
                queue.push(succ);
            }
        }
    }
    return exitBlock;
}