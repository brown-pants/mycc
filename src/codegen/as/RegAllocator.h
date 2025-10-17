#ifndef __REGALLOCATOR_H__
#define __REGALLOCATOR_H__

#include "../3ac/TACGenerator.h"
#include <map>

class RegAllocator
{
public:
    static const size_t reg_count = 6;
    enum RegId
    {
        Reg0,
        Reg1,
        Reg2,
        Reg3,
        Reg4,
        Reg5,
        Nul
    };

    struct Interval
    {
        int begin;
        int end;
    };

    struct VarNameCompare
    {
        bool operator()(const std::string& v1, const std::string& v2) const;
    };

    RegAllocator(const std::vector<TACGenerator::Quaternion> &tac);
    void exec();

    inline std::string toReg64(RegId reg) const { return Reg64[reg]; }
    inline std::string toReg8(RegId reg) const { return Reg8[reg]; }

    RegId getOneReg();
    RegId getVarReg(const std::string &temp_var);
    void freeReg(RegId reg);
    std::vector<RegId> freeVarRegs(int idx);
    RegId getRegId(const std::string &regStr);

private:
    std::unordered_set<RegId> m_regPool
    {
        Reg0,
        Reg1,
        Reg2,
        Reg3,
        Reg4,
        Reg5
    };
    const std::string Reg64[reg_count] = 
    {
        "%rax",
        "%rbx",
        "%rcx",
        "%rdx",
        "%rsi",
        "%rdi",
    };
    const std::string Reg8[reg_count] = 
    {
        "%al",
        "%bl",
        "%cl",
        "%dl",
        "%sil",
        "%dil",
    };
    const std::vector<TACGenerator::Quaternion> &tac;
    std::map<std::string, Interval, VarNameCompare> m_activeIntervals;
    std::map<std::string, RegId> m_varRegs;
    std::map<int, std::vector<RegId>> m_regEnds;
    bool isTempVar(const std::string &var) const;
    bool hasTempVar(const std::string &var, std::string &temp_name) const;
};

#endif