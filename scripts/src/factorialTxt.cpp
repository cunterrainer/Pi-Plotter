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
    std::string content;
    mpz_t prev;
    mpz_init2(prev, 5);
    mpz_set_ui(prev, 1);

    ProgressBarInit();
    std::ofstream ofs("FactorialTableData");
    ofs << iterations << '\n';
    for (uint32_t i = 1; i <= iterations; ++i)
    {
        ProgressBar((float)i-1, (float)iterations-1);
        if (content.size() > 100000)
        {
            ofs << content;
            content.clear();
        }
        char* str = mpz_get_str(NULL, 62, prev);
        content += str;
        content += '\n';
        mpz_mul_ui(prev, prev, i);
        std::free(str);
    }
    std::cout << "Writing to file..." << std::endl;
    content.pop_back();
    ofs << content;

    mpz_clear(prev);
    Profiler::End();
    std::cout << "Execution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)" << std::endl;
}