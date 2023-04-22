#pragma once
#include <array>

#include "mpfr.h"

#include "FactorialTableData.h"

class FactorialTable
{
private:
    std::array<mpz_t, FactorialTableData.size()> m_Table;
public:
    FactorialTable()
    {
        for (size_t i = 0; i < m_Table.size(); ++i)
        {
            mpz_init2(m_Table[i], (mp_bitcnt_t)FactorialTableData[i].size());
            mpz_set_str(m_Table[i], FactorialTableData[i].data(), 10);
        }
    }

    inline mpz_t& operator[](size_t index)
    {
        return m_Table[index];
    }
};
inline FactorialTable FactTable;