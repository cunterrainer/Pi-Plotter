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
    std::vector<BigInt> m_Table;
    size_t m_Limit = 100000;
private:
public:
    inline FactorialTable(const FactorialTable&) = delete;
    inline FactorialTable(FactorialTable&&) = delete;
    inline FactorialTable& operator=(const FactorialTable&) = delete;
    inline FactorialTable&& operator=(FactorialTable&&) = delete;

    inline FactorialTable()
    {
        Log << "Creating factorial table up to " << m_Limit << Endl;
        Profiler::Start();
        
        BigInt prev = 1;
        m_Table.reserve(m_Limit);
        ProgressBarInit();
        for (unsigned long i = 1; i <= m_Limit; ++i)
        {
            if(i % 1000 == 0)
                ProgressBar((float)i, (float)m_Limit);
            m_Table.emplace_back(prev);
            prev *= i;
        }
        
        Profiler::End();
        Log << "Created factorial table" << Endl;
        Log << "Execution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)" << Endl;
        Profiler::Reset();
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