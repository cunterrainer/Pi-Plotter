#pragma once
#include <vector>

#include "gmp.h"

#include "Log.h"
#include "BigInt.h"
#include "Profiler.h"
#include "ProgressBar.h"

class FactorialTable
{
private:
    mutable std::vector<BigInt> m_Table;
    mutable size_t m_Limit = 0;
public:
    inline FactorialTable() = default;
    inline FactorialTable(const FactorialTable&) = delete;
    inline FactorialTable(FactorialTable&&) = delete;
    inline FactorialTable& operator=(const FactorialTable&) = delete;
    inline FactorialTable&& operator=(FactorialTable&&) = delete;

    inline void Create(size_t limit) const
    {
        if (limit == 0) return;
        Log << "Creating factorial table up to " << limit << Endl;
        Profiler::Start();
        
        m_Limit = limit;
        BigInt prev = 1;
        m_Table.reserve(limit);
        ProgressBarInit();
        for (unsigned long i = 1; i <= limit; ++i)
        {
            if(i % 1000 == 0 || i == limit)
                ProgressBar((float)i, (float)limit);
            m_Table.emplace_back(prev);
            prev *= i;
        }
        
        Profiler::End();
        Log << "Created factorial table" << Endl;
        Profiler::Log(Profiler::Conversion::Seconds);
    }

    inline const BigInt& operator[](size_t index) const
    {
        if(index < m_Limit)
            return m_Table[index];
        static BigInt fac = 1;
        return fac.Factorial((unsigned long)index);
    }
};
inline const FactorialTable FactTable;