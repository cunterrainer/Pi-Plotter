#pragma once
#include <vector>

#include "gmp.h"

#include "Log.h"
#include "Profiler.h"
#include "ProgressBar.h"

class FactorialTable
{
private:
    std::vector<MP_INT> m_Table;
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
        MP_INT prev;
        mpz_init(&prev);
        mpz_set_ui(&prev, 1);
    
        m_Table.reserve(limit);
        ProgressBarInit();
        for (unsigned long i = 1; i <= limit; ++i)
        {
            if(i % 1000 == 0)
                ProgressBar((float)i, (float)limit);

            MP_INT tmp;
            mpz_init2(&tmp, 28);
            mpz_set(&tmp, &prev);
            m_Table.push_back(tmp);

            mpz_mul_ui(&prev, &prev, i);
        }
        mpz_clear(&prev);
        Profiler::End();
        Log << "Created factorial table" << Endl;
        Log << "Execution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)" << Endl;
        Profiler::Reset();
    }

    inline ~FactorialTable()
    {
        for (MP_INT& mp : m_Table)
            mpz_clear(&mp);
    }

    inline const MP_INT& operator[](size_t index) const
    {
        return m_Table[index];
    }
};
inline const FactorialTable FactTable;