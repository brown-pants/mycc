#include "LiveVariableAnalyzer.h"
#include <algorithm>

LiveVariableAnalyzer::LiveVariableAnalyzer(std::vector<TACGenerator::Quaternion> &tac) : tac(tac), dfg(tac)
{
    for (int i = 0; i < dfg.count(); i ++)
    {
        const auto &depthOrderBlocks = DataFlow::depthFirstOrder(dfg.getEntry(i));
        const auto &reverseDepthOrderBlocks = DataFlow::reverseDepthFirstOrder(dfg.getEntry(i));
        computeDef(reverseDepthOrderBlocks);
        computeInOut(reverseDepthOrderBlocks);
        computeLiveIntervals(depthOrderBlocks);
    }
}

std::map<std::string, Interval, VarNameCompare> LiveVariableAnalyzer::result() const
{
    return m_liveIntervals;
}

void LiveVariableAnalyzer::opt()
{
    std::vector<int> erases;
    for (auto iter = m_liveIntervals.begin(); iter != m_liveIntervals.end();)
    {
        const std::string &var = iter->first;
        const Interval &interval = iter->second;
        if (interval.begin == interval.end)
        {
            erases.push_back(interval.begin);
            iter = m_liveIntervals.erase(iter);
        }
        else
        {
            iter ++;
        }
    }

    for (auto &[var, interval] : m_liveIntervals)
    {
        int count = 0;
        for (int index : erases)
        {
            if (interval.begin >= index)
            {
                count ++;
            }
            else
            {
                break;
            }
        }
        interval.begin -= count;

        count = 0;
        for (int index : erases)
        {
            if (interval.end >= index)
            {
                count ++;
            }
            else
            {
                break;
            }
        }
        interval.end -= count;
    }

    int count = 0;
    for (int index : erases)
    {
        tac.erase(tac.begin() + index - count ++);
    }
}

void LiveVariableAnalyzer::computeDef(const std::vector<Block::ptr_t> &reverseDepthOrderBlocks)
{
    // init
    DEF.clear();
    for (Block::ptr_t block : reverseDepthOrderBlocks)
    {
        std::unordered_set<std::string> records;
        DEF.insert(std::pair<Block::ptr_t, std::unordered_set<std::string>>(block, records));
    }

    for (Block::ptr_t block : reverseDepthOrderBlocks)
    {
        for (int tac_index : block->tac_indices)
        {
            const auto &code = tac[tac_index];
            const std::string &def = getDef(code);
            if (def != "")
            {
                DEF[block].insert(def);
            }
        }
    }

    bool changed;
    do {
        changed = false;
        for (Block::ptr_t block : reverseDepthOrderBlocks)
        {
            auto newDef = DEF[block];
            
            // DEF[B] ∪= DEF[B.succs]
            for (Block::ptr_t succ : block->succs)
            {
                const auto &def = DEF[succ];
                newDef.insert(def.begin(), def.end());
            }

            if (newDef != DEF[block])
            {
                DEF[block] = newDef;
                changed = true;
            }
        }

    } while (changed);
}

void LiveVariableAnalyzer::computeInOut(const std::vector<Block::ptr_t> &reverseDepthOrderBlocks)
{
    // init
    IN.clear();
    OUT.clear();
    USE.clear();
    for (Block::ptr_t block : reverseDepthOrderBlocks)
    {
        std::unordered_set<std::string> records;
        IN.insert(std::pair<Block::ptr_t, std::unordered_set<std::string>>(block, records));
        OUT.insert(std::pair<Block::ptr_t, std::unordered_set<std::string>>(block, records));
        USE.insert(std::pair<Block::ptr_t, std::unordered_set<std::string>>(block, records));
    }

    for (Block::ptr_t block : reverseDepthOrderBlocks)
    {
        for (int tac_index : block->tac_indices)
        {
            const auto &code = tac[tac_index];
            const auto &use = getUse(code);
            USE[block].insert(use.begin(), use.end());
        }
        OUT[block] = USE[block];
    }

    bool changed;
    do {
        changed = false;
        for (Block::ptr_t block : reverseDepthOrderBlocks)
        {
            // IN[B] = ∪ OUT[B.succs]
            for (Block::ptr_t succ : block->succs)
            {
                const auto &out = OUT[succ];
                IN[block].insert(out.begin(), out.end());
            }

            // OUT[B] = IN[B] ∪ USE[B]
            auto newOut = IN[block];
            const auto &use = USE[block];
            newOut.insert(use.begin(), use.end());
            
            if (newOut != OUT[block])
            {
                OUT[block] = newOut;
                changed = true;
            }
        }

    } while (changed);
}

void LiveVariableAnalyzer::computeLiveIntervals(const std::vector<Block::ptr_t> &depthOrderBlocks)
{
    auto insertToIntervals = [this](const std::string &var, int index)
    {
        if (m_liveIntervals.find(var) == m_liveIntervals.end())
        {
            Interval interval;
            interval.begin = index;
            interval.end = index;
            m_liveIntervals[var] = interval;
        }
        else
        {
            Interval &interval = m_liveIntervals[var];
            if (index < interval.begin)
            {
                interval.begin = index;
            }
            else if (index > interval.end)
            {
                interval.end = index;
            }
        }
    };
    
    for (Block::ptr_t block : depthOrderBlocks)
    {
        auto &defIn = DEF[block];
        auto &in = IN[block];

        for (int tac_index : block->tac_indices)
        {
            const auto &code = tac[tac_index];
            const std::string &def = getDef(code);
            const std::set<std::string> &use = getUse(code);

            for (const std::string &var : in)
            {
                if (defIn.find(var) == defIn.end())
                {
                    insertToIntervals(var, tac_index);
                }
            }

            if (def != "")
            {
                insertToIntervals(def, tac_index);
            }

            for (const std::string &use_var : use)
            {
                for (int i = tac_index; i >= block->tac_indices.front(); i --)
                {
                    const auto &code = tac[i];
                    if (getDef(code) == use_var)
                    {
                        break;
                    }
                    insertToIntervals(use_var, i);
                }
            }

        }
    }
}

std::string LiveVariableAnalyzer::getDef(const TACGenerator::Quaternion &code)
{
    switch (code.op)
    {
        case TACGenerator::Op_address:
        case TACGenerator::Op_ref:
        case TACGenerator::Op_not:
        case TACGenerator::Op_neg:
        case TACGenerator::Op_add:
        case TACGenerator::Op_sub:
        case TACGenerator::Op_mult:
        case TACGenerator::Op_div:
        case TACGenerator::Op_mod:
        case TACGenerator::Op_eq_eq:
        case TACGenerator::Op_not_eq:
        case TACGenerator::Op_greater:
        case TACGenerator::Op_greater_eq:
        case TACGenerator::Op_less:
        case TACGenerator::Op_less_eq:
        case TACGenerator::Op_assign:
        if (analyzable(code.result))
        {
            return code.result;
        }
    }
    return "";
}

std::set<std::string> LiveVariableAnalyzer::getUse(const TACGenerator::Quaternion &code)
{
    std::set<std::string> use;
    switch (code.op)
    {
        case TACGenerator::Op_param:
        case TACGenerator::Op_if:
        case TACGenerator::Op_assign:
        case TACGenerator::Op_address:
        case TACGenerator::Op_ref:
        case TACGenerator::Op_not:
        case TACGenerator::Op_neg:
        case TACGenerator::Op_add:
        case TACGenerator::Op_sub:
        case TACGenerator::Op_mult:
        case TACGenerator::Op_div:
        case TACGenerator::Op_mod:
        case TACGenerator::Op_eq_eq:
        case TACGenerator::Op_not_eq:
        case TACGenerator::Op_greater:
        case TACGenerator::Op_greater_eq:
        case TACGenerator::Op_less:
        case TACGenerator::Op_less_eq:
            if(analyzable(code.arg1))
            {
                use.insert(code.arg1);
            }
            if(analyzable(code.arg2))
            {
                use.insert(code.arg2);
            }
    }
    if (code.op == TACGenerator::Op_param && analyzable(code.result))
    {
        use.insert(code.result);
    }
    else if (code.op == TACGenerator::Op_assign && code.result[0] == '*')
    {
        const std::string &res = code.result.substr(1);
        if (analyzable(res))
        {
            use.insert(res);
        }
    }
    return use;
}

bool LiveVariableAnalyzer::analyzable(const std::string &var)
{
    return var.substr(0, 2) == "t^";
}

bool VarNameCompare::operator()(const std::string& v1, const std::string& v2) const
{
    if (v1.length() < v2.length() || v1.length() == v2.length() && v1 < v2)
    {
        return true;
    }
    return false;
}