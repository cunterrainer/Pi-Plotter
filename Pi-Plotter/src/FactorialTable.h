#pragma once
#include <vector>
#include <string>
#include <fstream>

#include "gmp.h"

#include "Log.h"
#include "ProgressBar.h"
//#include "FactorialTableData.h"

class FactorialTable
{
private:
    std::vector<MP_INT> m_Table;
public:
    FactorialTable()
    {
        std::ifstream data("FactorialTableData");
        if (!data.is_open())
        {
            Err << "Failed to open [FactorialTableData] hence no factorial table was created" << Endl;
            return;
        }
    
        size_t i = 0;
        std::string line;
        std::getline(data, line);
        size_t lines = std::stoul(line);
    
        Log << "Creating factorial table from precomputed file" << Endl;
        ProgressBarInit();
        while (std::getline(data, line))
        {
            ProgressBar((float)i, (float)lines-1);
            MP_INT val;
            mpz_init2(&val, (mp_bitcnt_t)line.size());
            mpz_set_str(&val, line.c_str(), 62);
            m_Table.push_back(val);
            ++i;
        }
        Log << "Created factorial table with size: " << m_Table.size() - 1 << Endl;
    }

    //FactorialTable()
    //{
    //    Log << "Creating factorial table from precompiled binary" << Endl;
    //    ProgressBarInit();
    //    for (size_t i = 0; i < FactorialTableData.size(); ++i)
    //    {
    //        ProgressBar((float)i, (float)FactorialTableData.size() - 1);
    //        MP_INT val;
    //        mpz_init2(&val, (mp_bitcnt_t)FactorialTableData[i].size());
    //        mpz_set_str(&val, FactorialTableData[i].data(), 62);
    //        m_Table.push_back(val);
    //    }
    //    Log << "Created factorial table with size: " << m_Table.size() - 1 << Endl;
    //}

    inline MP_INT& operator[](size_t index)
    {
        return m_Table[index];
    }
};
inline FactorialTable FactTable;