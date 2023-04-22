#include <limits>
#include <cstdint>
#include <fstream>

#include "gmp.h"

#include "Profiler.h"
#include "ProgressBar.h"


uint32_t ParseCmd(int argc, char** argv)
{
    static constexpr uint32_t defaultIterations = 20000;
    if (argc == 1)
        return defaultIterations;

    char* endptr = nullptr;
    uint32_t result = std::strtol(argv[1], &endptr, 10);
    if (*endptr != '\0')
    {
        std::cout << "Usage: " << argv[0] << " [iterations=20000 max(" << std::numeric_limits<long>::max() << ")]" << std::endl;
        return 0;
    }
    return std::max(result, (uint32_t)1);
}

int main(int argc, char** argv)
{
    Profiler::Start();
    uint32_t iterations = ParseCmd(argc, argv);
    if (iterations == 0) return 0;
    uint32_t precision = 10;
    std::string content;
    mpz_t prev;
    mpz_init2(prev, precision);
    mpz_set_ui(prev, 1);

    ProgressBarInit();
    std::ofstream ofs("FactorialTableData");
    ofs << iterations << '\n';
    for (uint32_t i = 1; i <= iterations; ++i)
    {
        ProgressBar((float)i, (float)iterations);
        char* str = mpz_get_str(NULL, 10, prev);
        std::string prevStr(str);
        std::free(str);
        if (prevStr.size() == precision) // resize
        {
            mpz_t tmp;
            mpz_init2(tmp, precision);
            mpz_set(tmp, prev);

            precision *= 2;
            mpz_clear(prev);
            mpz_init2(prev, precision);
            mpz_set(prev, tmp);
            mpz_clear(tmp);
        }
        content += prevStr + '\n';

        if (content.size() > 100000)
        {
            ofs << content;
            content.clear();
        }
        mpz_mul_ui(prev, prev, i);
    }
    std::cout << "Writing to file..." << std::endl;
    ofs << content;

    mpz_clear(prev);
    Profiler::End();
    std::cout << "Execution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)" << std::endl;
}