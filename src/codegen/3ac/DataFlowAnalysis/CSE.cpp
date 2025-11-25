#include "CSE.h"
#include <algorithm>

CSE::CSE(std::vector<TACGenerator::Quaternion> &tac) : tac(tac), dfg(tac)
{
    recordGlobalArraysPointers();
}

void CSE::opt()
{
    for (int i = 0; i < dfg.count(); i ++)
    {
        Block::ptr_t entry = dfg.getEntry(i);
        const auto &depthOrderBlocks = DataFlow::depthFirstOrder(entry);
        computePtrInOut(depthOrderBlocks);
        computeGenKill(depthOrderBlocks);
        computeInOut(depthOrderBlocks);
        cse(depthOrderBlocks);
    }
    int offset = 0;
    for (auto idx : m_invalidTacIndices)
    {
        tac.erase(tac.begin() + idx - offset ++);
    }
}

void CSE::kill(Block::ptr_t block, const std::string &kill_value, std::set<TACGenerator::Quaternion> &dest)
{
    const auto &ptrRecords = PTR_OUT[block];
    
    if (kill_value != "~")
    {
        for (auto iter = dest.begin(); iter != dest.end(); )
        {
            const TACGenerator::Quaternion &code = *iter;
            if (code.op != TACGenerator::Op_address && (code.arg1 == kill_value || code.arg2 == kill_value))
            {
                iter = dest.erase(iter);
                continue;
            }
            if (code.op == TACGenerator::Op_ref)
            {
                auto it = ptrRecords.find(code.arg1);
                if (it != ptrRecords.end() && it->second == kill_value)
                {
                    iter = dest.erase(iter);
                    continue;
                }
            }
            iter ++;
        }
    }
    else
    {
        // function: Determine whether it is a constant.
        auto isConstant = [dest](const std::string &var)
        {
            if (var[0] == '-' || std::isdigit(var[0]) || var == "")
            {
                return true;
            }
            for (auto iter = dest.begin(); iter != dest.end(); iter ++)
            {
                if (iter->result == var && iter->op == TACGenerator::Op_address)
                {
                    return true;
                }
            }
            return false;
        };

        for (auto iter = dest.begin(); iter != dest.end(); )
        {
            if (iter->op != TACGenerator::Op_address)
            {
                if (!isConstant(iter->arg1) || !isConstant(iter->arg2))
                {
                    iter = dest.erase(iter);
                    continue;
                }
            }
            iter ++;
        }
    }
}

bool CSE::subExprTest(const std::set<TACGenerator::Quaternion> &available, const TACGenerator::Quaternion &expr)
{
    for (const TACGenerator::Quaternion &code : available)
    {
        if (code.op == expr.op && code.arg1 == expr.arg1 && code.arg2 == expr.arg2)
        {
            m_replaceVars[expr.result] = code.result;
            return true;
        }
    }
    return false;
}

void CSE::replaceVars(TACGenerator::Quaternion &code)
{
    if (code.result[0] == '*')
    {
        auto iter = m_replaceVars.find(code.result.substr(1));
        if (iter != m_replaceVars.end())
        {
            code.result = "*" + iter->second;
        }
    }
    else if (code.op == TACGenerator::Op_param)
    {
        auto iter = m_replaceVars.find(code.result);
        if (iter != m_replaceVars.end())
        {
            code.result = iter->second;
        }
    }
    auto iter = m_replaceVars.find(code.arg1);
    if (iter != m_replaceVars.end())
    {
        code.arg1 = iter->second;
    }
    iter = m_replaceVars.find(code.arg2);
    if (iter != m_replaceVars.end())
    {
        code.arg2 = iter->second;
    }
}

void CSE::cse(const std::vector<Block::ptr_t> &depthOrderBlocks)
{
    m_replaceVars.clear();
    for (Block::ptr_t block : depthOrderBlocks)
    {
        PTR_OUT[block] = PTR_IN[block];
        const auto &ptrRecords = PTR_OUT[block];

        for (int tac_index : block->tac_indices)
        {
            TACGenerator::Quaternion &code = tac[tac_index];
            if (code.op == TACGenerator::Op_local_var || code.op == TACGenerator::Op_dec_param || code.op == TACGenerator::Op_label)
            {
                continue;
            }
            replaceVars(code);
            if (code.op == TACGenerator::Op_param || code.op == TACGenerator::Op_if)
            {
                continue;
            }
            handlePtrAssgin(block, code);
            // assign
            if (code.op == TACGenerator::Op_assign)
            {
                // *ptr = ...
                if (code.result[0] == '*')
                {
                    std::string ptr = code.result.substr(1);
                    auto iter = ptrRecords.find(ptr);
                    if (iter != ptrRecords.end() && iter->second != "")
                    {
                        // kill
                        kill(block, iter->second, IN[block]);
                    }
                    else
                    {
                        // kill except constant
                        kill(block, "~", IN[block]);
                    }
                }
                // var = ...
                else
                {
                    // kill
                    kill(block, code.result, IN[block]);
                }
            }
            else if (code.op == TACGenerator::Op_call_func)
            {
                // kill except constant
                kill(block, "~", IN[block]);
            }
            else if (subExprTest(IN[block], code))
            {
                m_invalidTacIndices.insert(tac_index);
            }
            else
            {
                IN[block].insert(code);
            }
        }
    }
}

void CSE::computeGenKill(const std::vector<Block::ptr_t> &depthOrderBlocks)
{
    // init
    GEN.clear();
    KILL.clear();
    for (Block::ptr_t block : depthOrderBlocks)
    {
        std::set<TACGenerator::Quaternion> records;
        GEN.insert(std::pair<Block::ptr_t, std::set<TACGenerator::Quaternion>>(block, records));
        KILL.insert(std::pair<Block::ptr_t, std::set<std::string>>(block, std::set<std::string>()));
    }

    for (Block::ptr_t block : depthOrderBlocks)
    {
        PTR_OUT[block] = PTR_IN[block];
        const auto &ptrRecords = PTR_OUT[block];
        
        for (int tac_index : block->tac_indices)
        {
            const TACGenerator::Quaternion &code = tac[tac_index];
            if (code.op == TACGenerator::Op_local_var || code.op == TACGenerator::Op_dec_param || code.op == TACGenerator::Op_param || code.op == TACGenerator::Op_label || code.op == TACGenerator::Op_if)
            {
                continue;
            }
            handlePtrAssgin(block, code);
            // assign
            if (code.op == TACGenerator::Op_assign)
            {
                // *ptr = ...
                if (code.result[0] == '*')
                {
                    std::string ptr = code.result.substr(1);
                    auto iter = ptrRecords.find(ptr);
                    if (iter != ptrRecords.end() && iter->second != "")
                    {
                        // kill
                        KILL[block].insert(iter->second);
                        kill(block, iter->second, GEN[block]);
                    }
                    else
                    {
                        // kill except constant
                        KILL[block].insert("~");
                        kill(block, "~", GEN[block]);
                    }
                }
                // var = ...
                else
                {
                    // kill
                    KILL[block].insert(code.result);
                    kill(block, code.result, GEN[block]);
                }
            }
            else if (code.op == TACGenerator::Op_call_func)
            {
                // kill except constant
                KILL[block].insert("~");
                kill(block, "~", GEN[block]);
            }
            else
            {
                // Gen
                GEN[block].insert(code);
            }
        }
    }
}

void CSE::computeInOut(const std::vector<Block::ptr_t> &depthOrderBlocks)
{
    // init
    IN.clear();
    OUT.clear();
    for (Block::ptr_t block : depthOrderBlocks)
    {
        std::set<TACGenerator::Quaternion> records;
        IN.insert(std::pair<Block::ptr_t, std::set<TACGenerator::Quaternion>>(block, records));
        OUT.insert(std::pair<Block::ptr_t, std::set<TACGenerator::Quaternion>>(block, records));
    }

    // computer out
    for (auto block : depthOrderBlocks) 
    {
        OUT[block] = GEN[block];
    }

    // update out and compute in
    bool changed;
    bool first_loop = true;
    std::unordered_set<Block::ptr_t> jumpBlocks;
    do {
        changed = false;
        for (Block::ptr_t block : depthOrderBlocks)
        {
            bool first = true;
            std::set<TACGenerator::Quaternion> in;
            for (Block::ptr_t prev : block->prevs)
            {
                const auto &out = OUT[prev];

                if (first_loop && block->prevs.size() > 1)
                {
                    bool sign = false;
                    auto paths = DataFlow::findPaths(block, prev);
                    for (const auto &path : paths)
                    {
                        sign = std::find_first_of(path.begin(), path.end(), jumpBlocks.begin(), jumpBlocks.end()) != path.end();
                        if (sign)
                        {
                            break;
                        }
                    }
                    if (!paths.empty() && !sign)
                    {
                        jumpBlocks.insert(block);
                        continue;
                    }
                }
                
                if (first)
                {
                    in = out;
                    first = false;
                    continue;
                }
                // IN[B] = ∩ OUT[B.prevs]
                std::set<TACGenerator::Quaternion> result;
                std::set_intersection(in.begin(), in.end(), out.begin(), out.end(), std::inserter(result, result.begin()));
                in = result;
            }
            IN[block] = in;

            // OUT[B] = GEN[B] ∪ (IN[B] - KILL[B])
            std::set<TACGenerator::Quaternion> newOut = GEN[block];
            std::set<TACGenerator::Quaternion> diffSet = IN[block];
            for (const std::string &kill_value : KILL[block])
            {
                kill(block, kill_value, diffSet);
            }
            for (const auto &code : diffSet)
            {
                newOut.insert(code);
            }

            // compare old out and new out
            if (newOut != OUT[block]) 
            {
                OUT[block] = newOut;
                changed = true;
            }
        }
        first_loop = false;
    } while (changed);
}

void CSE::computePtrInOut(const std::vector<Block::ptr_t> &depthOrderBlocks)
{
    m_arrays = m_globlArr;
    m_pointers = m_globalPtrs;
    
    // init
    PTR_IN.clear();
    PTR_OUT.clear();
    for (Block::ptr_t block : depthOrderBlocks)
    {
        std::unordered_map<std::string, std::string> records;
        PTR_IN.insert(std::pair<Block::ptr_t, std::unordered_map<std::string, std::string>>(block, records));
        PTR_OUT.insert(std::pair<Block::ptr_t, std::unordered_map<std::string, std::string>>(block, records));
    }

    // computer pointer out
    for (Block::ptr_t block : depthOrderBlocks)
    {
        for (int tac_index : block->tac_indices)
        {
            const TACGenerator::Quaternion &code = tac[tac_index];
            handlePtrAssgin(block, code);
            // record pointer and array
            if (code.op == TACGenerator::Op_local_var)
            {
                const std::string &type = code.arg2.substr(0, code.arg2.find('_'));
                if (type == "arr")
                {
                    m_arrays[code.result] = std::stoll(code.arg1);
                }
                else if (type == "ptr")
                {
                    m_pointers.insert(code.result);
                }
            }
        }
    }

    // update pointer out and compute pointer in
    bool changed;
    do {
        changed = false;
        for (Block::ptr_t block : depthOrderBlocks)
        {
            std::unordered_map<std::string, std::string> ptrIn;

            // IN[B] = ∪ OUT[B.prevs]
            for (const Block::ptr_t &prev : block->prevs)
            {
                for (auto &[ptr, address] : PTR_OUT[prev])
                {
                    auto iter = ptrIn.find(ptr);
                    if (iter != ptrIn.end())
                    {
                        // diff
                        if (address != iter->second)
                        {
                            ptrIn[ptr] = "";
                        }
                    }
                    else
                    {
                        ptrIn[ptr] = address;
                    }
                }
            }
            PTR_IN[block] = ptrIn;
            
            // record old out
            auto oldOut = PTR_OUT[block];
            PTR_OUT[block] = ptrIn;
            
            // OUT[B] = IN[B] ∪ GEN[B]
            for (int tac_index : block->tac_indices)
            {
                const TACGenerator::Quaternion &code = tac[tac_index];
                handlePtrAssgin(block, code);
            }

            // compare old out and new out
            if (oldOut != PTR_OUT[block])
            {
                changed = true;
            }
        }
    } while (changed);
}

void CSE::handlePtrAssgin(Block::ptr_t block, const TACGenerator::Quaternion &code)
{
    auto &ptrRecords = PTR_OUT[block];
    // p = &a
    if (code.op == TACGenerator::Op_address)
    {
        ptrRecords[code.result] = code.arg1;
    }
    else if (code.op == TACGenerator::Op_assign)
    {
        auto iter = ptrRecords.find(code.arg1);
        if (iter != ptrRecords.end())
        {
            ptrRecords[code.result] = iter->second;
        }
        else if (m_pointers.find(code.result) != m_pointers.end())
        {
            ptrRecords[code.result] = "";
        }
    }
    else if (code.op == TACGenerator::Op_add || code.op == TACGenerator::Op_sub)
    {
        auto iter = ptrRecords.find(code.arg1);
        // arg1 is not a recorded pointer or arg2 is not a constant
        if (iter == ptrRecords.end() || code.arg2[0] != '-' && !std::isdigit(code.arg2[0]))
        {
            return;
        }
        long long offset = std::stoll(code.arg2);
        if (code.op == TACGenerator::Op_sub)
        {
            offset *= -1;
        }
        std::string record = iter->second;
        std::size_t pos = record.find('[');
        std::string arr = record;
        // p = arr[N]
        if (pos != std::string::npos)
        {
            arr = record.substr(0, pos);
            offset += std::stoll(record.substr(pos + 1, record.length() - 1));
        }
        long long arrSize = 1;
        auto arr_iter = m_arrays.find(arr);
        if (arr_iter != m_arrays.end())
        {
            arrSize = arr_iter->second;
        }
        if (offset < 0 || offset >= arrSize)
        {
            return;
        }
        std::string newRecord = record + '[' + std::to_string(offset) + ']';
        ptrRecords[code.result] = newRecord;
    }
}

void CSE::recordGlobalArraysPointers()
{
    for (auto code : tac)
    {
        if (code.op == TACGenerator::Op_global_var || code.op == TACGenerator::Op_global_init)
        {
            const std::string &type = code.arg2.substr(0, code.arg2.find('_'));
            if (type == "arr")
            {
                m_globlArr[code.result] = std::stoll(code.arg1);
            }
            else if (type == "ptr")
            {
                m_globalPtrs.insert(code.result);
            }
        }
    }
}