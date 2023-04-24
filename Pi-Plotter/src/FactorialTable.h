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
public:
    inline FactorialTable(const FactorialTable&) = delete;
    inline FactorialTable(FactorialTable&&) = delete;
    inline FactorialTable& operator=(const FactorialTable&) = delete;
    inline FactorialTable&& operator=(FactorialTable&&) = delete;

    inline FactorialTable()
    {
        constexpr unsigned long limit = 100000;
        Log << "Creating factorial table up to " << limit << Endl;
        Profiler::Start();

        BigInt prev = 1;
        m_Table.reserve(limit);
        ProgressBarInit();
        for (unsigned long i = 1; i <= limit; ++i)
        {
            if(i % 1000 == 0)
                ProgressBar((float)i, (float)limit);
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
        return m_Table[index];
    }
};
inline const FactorialTable FactTable;