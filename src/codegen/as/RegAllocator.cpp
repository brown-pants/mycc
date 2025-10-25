#include "RegAllocator.h"
#include "../../debug/Debug.h"
#include <set>

RegAllocator::RegAllocator(const std::vector<TACGenerator::Quaternion> &tac)
    : tac(tac)
{

}

void RegAllocator::exec()
{
    // get active intervals
    std::string temp_name;
    for (int idx = 0; idx < tac.size(); idx ++)
    {
        const TACGenerator::Quaternion &code = tac[idx];
        switch(code.op)
        {
        case TACGenerator::Op_param:
            if (hasTempVar(code.result, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            break;
        case TACGenerator::Op_if:
            if (hasTempVar(code.arg1, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            break;
        case TACGenerator::Op_assign:
        case TACGenerator::Op_address:
        case TACGenerator::Op_ref:
        case TACGenerator::Op_not:
        case TACGenerator::Op_neg:
            if (isTempVar(code.result))
            {
                m_activeIntervals.insert(std::pair<std::string, Interval>(code.result, { idx, idx }));
            }
            else if (hasTempVar(code.result, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            if (hasTempVar(code.arg1, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            break;
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
            if (isTempVar(code.result))
            {
                m_activeIntervals.insert(std::pair<std::string, Interval>(code.result, { idx, idx }));
            }
            else if (hasTempVar(code.result, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            if (hasTempVar(code.arg1, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            if (hasTempVar(code.arg2, temp_name))
            {
                m_activeIntervals[temp_name].end = idx;
            }
            break;
        }
    }

    // alloc register
    auto insertToRegEnds = [this](int end, RegId reg) {
        if (m_regEnds.find(end) == m_regEnds.end())
        {
            m_regEnds[end] = std::vector<RegAllocator::RegId>({ reg });
        }
        else
        {
            m_regEnds[end].push_back(reg);
        }
    };

    std::multimap<int, RegId> regMap;    // map <end, reg>
    for (auto iter : m_activeIntervals)
    {
        std::string var = iter.first;
        const Interval interval = iter.second;
        if (regMap.empty())
        {
            regMap.insert(std::pair<int, RegId>(interval.end, Reg0));
            m_varRegs.insert(std::pair<std::string, RegId>(var, Reg0));
        }
        else
        {
            bool sign = false;
            std::set<RegId> occupiedRegs;
            for(auto it = regMap.rbegin(); it != regMap.rend(); it ++)
            {
                RegId reg = it->second;
                if (interval.begin > it->first)
                {
                    if (occupiedRegs.find(reg) == occupiedRegs.end())
                    {
                        regMap.insert(std::pair<int, RegId>(interval.end, reg));
                        m_varRegs.insert(std::pair<std::string, RegId>(var, reg));
                        sign = true;
                        insertToRegEnds(interval.end, reg);
                        break;
                    }
                }
                else
                {
                    occupiedRegs.insert(reg);
                }
            }
            if (!sign)
            {
                RegId reg = *occupiedRegs.rbegin();
                if (reg + 1 < reg_count)
                {
                    RegId new_reg = RegId(reg + 1);
                    regMap.insert(std::pair<int, RegId>(interval.end, new_reg));
                    m_varRegs.insert(std::pair<std::string, RegId>(var, new_reg));
                    insertToRegEnds(interval.end, new_reg);
                }
            }
        }
    }
    Debug::PrintActiveIntervalsAndRegAlloc(m_activeIntervals, m_varRegs);
}

RegAllocator::RegId RegAllocator::getOneReg()
{
    if (m_regPool.empty())
    {
        return Nul;
    }
    RegId reg = *m_regPool.begin();
    m_regPool.erase(m_regPool.begin());
    return reg;
}

RegAllocator::RegId RegAllocator::getVarReg(const std::string &temp_var)
{
    auto iter = m_varRegs.find(temp_var);
    if (iter != m_varRegs.end())
    {
        m_regPool.erase(iter->second);
        return iter->second;
    }
    return Nul;
}

void RegAllocator::freeReg(RegId reg)
{
    m_regPool.insert(reg);
}

bool RegAllocator::isFreeReg(RegId reg)
{
    return m_regPool.find(reg) != m_regPool.end();
}

std::vector<RegAllocator::RegId> RegAllocator::freeVarRegs(int idx)
{
    auto iter = m_regEnds.find(idx);
    if (iter != m_regEnds.end())
    {
        for (auto reg : iter->second)
        {
            freeReg(reg);
        }
        return iter->second;
    }
    return std::vector<RegAllocator::RegId>();
}

RegAllocator::RegId RegAllocator::getRegId(const std::string &regStr)
{
    for (int i = 0; i < reg_count; i ++)
    {
        if (Reg64[i] == regStr || Reg8[i] == regStr)
        {
            return RegId(i);
        }
    }
    return Nul;
}

bool RegAllocator::isInvalidVar(const std::string &var)
{
    auto iter = m_activeIntervals.find(var);
    return (iter != m_activeIntervals.end() && iter->second.begin == iter->second.end);
}

bool RegAllocator::isTempVar(const std::string &var) const 
{ 
    return var.substr(0, 2) == "t^"; 
}

bool RegAllocator::hasTempVar(const std::string &var, std::string &temp_name) const
{
    if (var[0] == '*')
    {
        temp_name = var.substr(1);
        return isTempVar(temp_name);
    }
    temp_name = var;
    return isTempVar(var);
}

bool RegAllocator::VarNameCompare::operator()(const std::string& v1, const std::string& v2) const
{
    if (v1.length() < v2.length() || v1.length() == v2.length() && v1 < v2)
    {
        return true;
    }
    return false;
}