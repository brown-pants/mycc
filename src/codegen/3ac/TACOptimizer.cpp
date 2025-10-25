#include "TACOptimizer.h"

TACOptimizer::TACOptimizer()
{
    
}

void TACOptimizer::opt(std::vector<TACGenerator::Quaternion> &tac)
{
    optGotoLabel(tac);
}

void TACOptimizer::optGotoLabel(std::vector<TACGenerator::Quaternion> &tac)
{
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