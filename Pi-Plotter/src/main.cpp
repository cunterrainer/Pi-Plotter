#include <iostream>
#include <chrono>

#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "boost/math/special_functions/factorials.hpp"

#include "pi-million.h"
using namespace boost::multiprecision;

//using int2048_t = number<cpp_int_backend<4096*128, 4096*128, signed_magnitude, unchecked, void> >;
using float1000 = number<cpp_bin_float<15000>>;
using int2048_t = float1000;
int2048_t factorial(unsigned int n)
{
    if (n <= 1)
        return 1;
    int2048_t result = 1;
    for (unsigned int i = 2; i < n + 1; ++i)
        result *= i;
    return result;
}

float1000 calc(int iterations)
{
    float1000 sm = 0;
    int2048_t a(545140134);
    int2048_t b(13591409);
    int2048_t longNum(-262537412640768000);

    for (int i = 0; i < iterations; ++i)
    {
        std::cout << "Iteration: " << i << '\r';
        int2048_t c = a * i + b;
        //a *= i;
        //a += 13591409;
        float1000 num = (factorial(6 * i) * c);
        float1000 denom = (factorial(3 * i) * pow(factorial(i), 3)) * (pow(longNum, i));
        sm += (num / denom);
        //std::cout << num << "\n" << denom << std::endl;
        //num = factorial(6*i)*Decimal((545140134*i+13591409))
        //denom = factorial(3*i)*pow(factorial(i), 3)*pow(Decimal(-262537412640768000), i)
        //sm += num/denom
    }
    std::cout << "\n";
    //std::cout << (float1000(426880*boost::multiprecision::sqrt(float1000(10005)))/sm).str() << std::endl;
    return float1000(426880 * boost::multiprecision::sqrt(float1000(10005))) / sm;
    //return Decimal(426880)*Decimal(10005).sqrt()/sm
}

uint32_t Matches(const float1000& decNum, uint32_t )
{
    uint32_t same = 0;
    std::string decStr = decNum.str().erase(0, 2);
    //std::cout << decStr << std::endl;
    for (size_t i = 0; i < decStr.size(); ++i)
    {
        if (decStr[i] != PiMillionStr[i])
            break;
        ++same;
    }
    return same;
}

int main()
{
    auto start = std::chrono::steady_clock::now();
    std::cout << Matches(calc(1000), 0) << std::endl;
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0 << " sec(s)" << std::endl;
    //// Operations at fixed precision and full numeric_limits support:
    //cpp_bin_float_100 b = 2;
    //std::cout << std::numeric_limits<cpp_bin_float_100>::digits << std::endl;
    //std::cout << std::numeric_limits<cpp_bin_float_100>::digits10 << std::endl;
 //
    //// We can use any C++ std lib function, lets print all the digits as well:
    //std::cout << std::setprecision(std::numeric_limits<cpp_bin_float_100>::max_digits10)
    //   << log(b) << std::endl; // print log(2)
 //
    //// We can also use any function from Boost.Math:
    //std::cout << boost::math::tgamma(b) << std::endl;
    //// These even work when the argument is an expression template:
    //std::cout << boost::math::tgamma(b * b) << std::endl;
 //
    //// And since we have an extended exponent range we can generate some really large
    //// numbers here (4.0238726007709377354370243e+2564):
    //std::cout << boost::math::tgamma(cpp_bin_float_100(1000)) << std::endl;
    std::cin.get();
    return 0;
}