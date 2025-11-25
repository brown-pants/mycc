#include "RegAllocator.h"
#include "../../debug/Debug.h"

RegAllocator::RegAllocator(const std::map<std::string, Interval, VarNameCompare> &liveIntervals) : m_liveIntervals(liveIntervals)
{

}

void RegAllocator::exec()
{
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
    for (auto iter : m_liveIntervals)
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
    Debug::PrintLiveIntervalsAndRegAlloc(m_liveIntervals, m_varRegs);
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