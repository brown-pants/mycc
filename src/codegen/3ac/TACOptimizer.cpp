#include "TACOptimizer.h"
#include <algorithm>

TACOptimizer::TACOptimizer(std::vector<TACGenerator::Quaternion> &tac)
    : tac(tac)
{
    
}

void TACOptimizer::opt()
{
    optGotoLabel();
    optGotoLabel();
    genCFG();
    cse();
}

void TACOptimizer::optGotoLabel()
{
    m_replaceLabels.clear();
    m_validLabels.clear();
    for (int idx = 1; idx < tac.size(); idx ++)
    {
        if (idx < 1)
        {
            continue;
        }
        const TACGenerator::Quaternion &preCode = tac[idx - 1];
        const TACGenerator::Quaternion &code = tac[idx];
        if (preCode.op == TACGenerator::Op_label)
        {
            /*
                L1:
                L2: (invalid)
            */
            if (code.op == TACGenerator::Op_label)
            {
                m_replaceLabels[code.result] = preCode.result;
                tac.erase(tac.begin() + idx);
                idx --;
            }
            /*
                L1: (invalid)
                goto L2
            */
            else if (code.op == TACGenerator::Op_goto)
            {
                m_replaceLabels[preCode.result] = code.result;
                tac.erase(tac.begin() + idx - 1);
                idx -= 2;
            }
        }
        else if (preCode.op == TACGenerator::Op_goto)
        {
            /*
                goto L1
                goto L2 (invalid)
            */
            if (code.op != TACGenerator::Op_label)
            {
                tac.erase(tac.begin() + idx);
                idx --;
            }
            /*
                goto L1 (invalid)
                L1:
            */
            else if (code.op == TACGenerator::Op_label && code.result == preCode.result)
            {
                tac.erase(tac.begin() + idx - 1);
                idx -= 2;
            }
            else
            {
                m_validLabels.insert(preCode.result);
            }
        }
        /*
            if goto L1 (invalid)
            L1:
        */
        else if (preCode.op == TACGenerator::Op_if && code.op == TACGenerator::Op_label && code.result == preCode.result)
        {
            tac.erase(tac.begin() + idx - 1);
            idx -= 2;
        }
        else
        {
            m_validLabels.insert(preCode.result);
        }
    }

    for (auto iter = m_validLabels.begin(); iter != m_validLabels.end(); )
    {
        bool sign = false;
        std::string label = *iter;
        while (m_replaceLabels.find(label) != m_replaceLabels.end())
        {
            label = m_replaceLabels[label];
            sign = true;
        }
        if (sign)
        {
            iter = m_validLabels.erase(iter);
            m_validLabels.insert(label);
        }
        else
        {
            iter ++;
        }
    }

    for (int idx = 0; idx < tac.size(); idx ++)
    {
        TACGenerator::Quaternion &code = tac[idx];
        if (code.op == TACGenerator::Op_goto || code.op == TACGenerator::Op_if)
        {
            std::string label = code.result;
            while (m_replaceLabels.find(label) != m_replaceLabels.end())
            {
                label = m_replaceLabels[label];
            }
            code.result = label;
        }
        else if (code.op == TACGenerator::Op_label && m_validLabels.find(code.result) == m_validLabels.end())
        {
            tac.erase(tac.begin() + idx);
            idx --;
        }
    }
}

void TACOptimizer::genCFG()
{
    bool sign = false;
    for (int idx = 0; idx < tac.size(); idx ++)
    {
        TACGenerator::Quaternion &code = tac[idx];
        if (code.op == TACGenerator::Op_begin_func)
        {
            Block *block = new Block();
            m_CFG.push_back(block);
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
            Block *block = new Block();
            block->tac_indices.push_back(idx);
            curBlock->succs.insert(block);
            block->prevs.insert(curBlock);
            m_labelBlocks[code.result] = block;
            auto iter = m_jumpBlocks.find(code.result);
            if (iter != m_jumpBlocks.end())
            {
                for (Block *pre_block : iter->second)
                {
                    pre_block->succs.insert(block);
                    block->prevs.insert(pre_block);
                }
                m_jumpBlocks.erase(iter);
            }
            curBlock = block;
        }
        else if (code.op == TACGenerator::Op_goto || code.op == TACGenerator::Op_if)
        {
            auto iter = m_labelBlocks.find(code.result);
            if (iter != m_labelBlocks.end())
            {
                Block *succBlock = iter->second;
                curBlock->succs.insert(succBlock);
                succBlock->prevs.insert(curBlock);
            }
            else if (m_jumpBlocks.find(code.result) != m_jumpBlocks.end())
            {
                m_jumpBlocks[code.result].push_back(curBlock);
            }
            else
            {
                m_jumpBlocks[code.result] = std::vector<Block *>({ curBlock });
            }
        }
        else
        {
            curBlock->tac_indices.push_back(idx);
        }
    }
}

void TACOptimizer::cse()
{
    for (Block *block : m_CFG)
    {
        m_OUT.clear();
        m_visitedBlocks.clear();
        cse_area(block);
    }
    int offset = 0;
    for (auto idx : m_invalidTacIndices)
    {
        tac.erase(tac.begin() + idx - offset ++);
    }
}

void TACOptimizer::cse_area(Block *block)
{
    if (m_visitedBlocks.find(block) != m_visitedBlocks.end())
    {
        return;
    }
    m_visitedBlocks.insert(block);
    std::unordered_set<std::string> m_KILL;
    std::set<TACGenerator::Quaternion> _IN;
    bool first = true;
    for (Block *pre : block->prevs)
    {
        if (m_OUT.find(pre) == m_OUT.end())
        {
            cse_area(pre);
        }
        std::set<TACGenerator::Quaternion> &out = m_OUT[pre];
        if (first)
        {
            _IN = out;
            first = false;
            continue;
        }
        // IN[B]  = ∩ OUT[P]
        std::set<TACGenerator::Quaternion> result;
        std::set_intersection(
            _IN.begin(), _IN.end(),
            out.begin(), out.end(),
            std::inserter(result, result.begin())
        );
        _IN = result;
    }
    for (int tac_index : block->tac_indices)
    {
        TACGenerator::Quaternion &code = tac[tac_index];
        if (code.op == TACGenerator::Op_local_var || code.op == TACGenerator::Op_dec_param || code.op == TACGenerator::Op_param || code.op == TACGenerator::Op_label)
        {
            continue;
        }
        replaceVars(code);
        if (code.op == TACGenerator::Op_assign)
        {
            if (code.result[0] == '*')
            {
                std::string result = code.result.substr(1);
                auto iter = m_ptrRecords.find(result);
                if (iter != m_ptrRecords.end())
                {
                    kill(_IN, iter->second);
                }
                else
                {
                    _IN.clear();
                }
            }
            else
            {
                auto iter = m_ptrRecords.find(code.arg1);
                if (iter != m_ptrRecords.end())
                {
                    m_ptrRecords[code.result] = iter->second;
                }
                kill(_IN, code.result);
            }
        }
        else if (code.op == TACGenerator::Op_address)
        {
            if (subExprTest(_IN, code))
            {
                m_invalidTacIndices.insert(tac_index);
            }
            else
            {
                m_ptrRecords[code.result] = code.arg1;
                _IN.insert(code);
            }
        }
        else if (code.op == TACGenerator::Op_call_func)
        {
            _IN.clear();
        }
        else if (subExprTest(_IN, code))
        {
            m_invalidTacIndices.insert(tac_index);
        }
        else
        {
            _IN.insert(code);
        }
    }
    m_OUT[block] = _IN;
    for (Block *succ : block->succs)
    {
        cse_area(succ);
    }
}

bool TACOptimizer::subExprTest(const std::set<TACGenerator::Quaternion> &_in, const TACGenerator::Quaternion &expr)
{
    for (const TACGenerator::Quaternion &code : _in)
    {
        if (code.op == expr.op && code.arg1 == expr.arg1 && code.arg2 == expr.arg2)
        {
            m_replaceVars[expr.result] = code.result;
            return true;
        }
    }
    return false;
}

void TACOptimizer::replaceVars(TACGenerator::Quaternion &code)
{
    if (code.result[0] == '*')
    {
        auto iter = m_replaceVars.find(code.result.substr(1));
        if (iter != m_replaceVars.end())
        {
            code.result = "*" + iter->second;
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

void TACOptimizer::kill(std::set<TACGenerator::Quaternion> &_in, const std::string &var)
{
    for (auto iter = _in.begin(); iter != _in.end(); )
    {
        const TACGenerator::Quaternion &code = *iter;
        if (code.arg1 == var || code.arg2 == var)
        {
            iter = _in.erase(iter);
        }
        else
        {
            iter ++;
        }
    }
}
