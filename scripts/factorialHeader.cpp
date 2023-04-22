#include <string>
#include <limits>
#include <cstdint>
#include <fstream>

#include "gmp.h"

#include "Profiler.h"
#include "ProgressBar.h"

uint32_t ParseCmd(int argc, char** argv)
{
    static constexpr uint32_t defaultIterations = 20001;
    if (argc == 1)
        return defaultIterations;

    char* endptr = nullptr;
    uint32_t result = std::strtoul(argv[1], &endptr, 10) + 1;
    if (*endptr != '\0')
    {
        std::cout << "Usage: " << argv[0] << " [limit=20000 max(" << std::numeric_limits<long>::max() << ")]" << std::endl;
        return 0;
    }
    return result;
}

int main(int argc, char** argv)
{
    Profiler::Start();
    uint32_t iterations = ParseCmd(argc, argv);
    if (iterations == 0) return 0;
    std::string contentHeader = "#pragma once\n#include <array>\n#include <string_view>\nextern const std::array<std::string_view, " + std::to_string(iterations) + "> FactorialTableData;";
    std::string contentCpp = "#include <FactorialTableData.h>\nconst std::array<std::string_view, " + std::to_string(iterations) + "> FactorialTableData = \n{{\n";
    mpz_t prev;
    mpz_init2(prev, 5);
    mpz_set_ui(prev, 1);

    ProgressBarInit();
    std::ofstream osCpp("../Pi-Plotter/src/FactorialTableData.cpp");
    for (uint32_t i = 1; i <= iterations; ++i)
    {
        ProgressBar((float)i-1, (float)iterations-1);
        char* str = mpz_get_str(NULL, 62, prev);
        std::string prevStr(str);
        std::free(str);

        if (prevStr.size() >= 16380)
        {
            contentCpp += '\t';
            uint32_t blocks = (uint32_t)((double)prevStr.size() / 16380.0);
            for (uint32_t k = 0; k < blocks; ++k)
            {
                contentCpp += '"' + prevStr.substr(k * 16380, 16380) + '"';
                if ((double)blocks != (double)prevStr.size() / 16380.0)
                {
                    contentCpp += '"' + prevStr.substr(blocks * 16380) + '"';
                }
            }
            contentCpp += ",\n";
        }
        else
            contentCpp += "\t\"" + prevStr + "\",\n";
        if (contentCpp.size() > 100000)
        {
            osCpp << contentCpp;
            contentCpp.clear();
        }
        mpz_mul_ui(prev, prev, i);
    }
    contentCpp += "}};";
    std::cout << "Writing to file..." << std::endl;
    std::ofstream osH("../Pi-Plotter/src/FactorialTableData.h");
    osH << contentHeader;
    osCpp << contentCpp;

    mpz_clear(prev);
    Profiler::End();
    std::cout << "Execution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)" << std::endl;
}