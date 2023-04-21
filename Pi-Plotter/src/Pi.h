#pragma once
#include <cmath>
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/math/constants/constants.hpp"
#include "mpfr.h"

#include "FloatArbitrary.h"

namespace Pi
{
#include "pi-million.h"
}

namespace Pi
{
    namespace bm  = boost::math;
    namespace bmp = boost::multiprecision;
    //using Float = bmp::number<bmp::cpp_bin_float<10000>>;
    using Float = FloatArbitrary<1500000>;

    mpz_t* Init()
    {
        static mpz_t longNum;
        mpz_init2(longNum, 20);
        mpz_set_str(longNum, "-262537412640768000", 10);
        return &longNum;
    }

    Float Chudnovsky(uint32_t i)
    {
        static Float prevNumDenom = 0;
        static Float longNumPow = 1; // -262537412640768000^0 = 1
        static mpz_t* longNum = Init();
        Float sumNum = Float(426880) * Float(10005).Sqrt();
        static Float a(545140134);
        static const unsigned long int b = 13591409;
        static Float c = 0;
        c = 0;
        a = 545140134;
        //c.Factorial(6*i);
        //a = a * i;
        //a = a + b;
        --i;
        //const Float num = Math::Factorial<Float>(6 * i) * (a * i + b);
        //Float num = c * a; // c*(a*i+b);
        Float num = c.Factorial(6 * i) * (a * i + b);
        //const Float denom = Math::Factorial<Float>(3 * i) * bmp::pow(Math::Factorial<Float>(i), 3) * longNumPow;
        Float denom = c.Factorial(3 * i);
        a.Factorial(i);
        a.Pow(3);
        denom *= a;
        denom *= longNumPow;
        prevNumDenom += num.operator/(denom);
        //longNumPow.Assign("-262537412640768000");
        //longNumPow.Pow(i + 1);
        longNumPow *= *longNum;
        return sumNum / prevNumDenom;
        //prevNumDenom += num / denom;
        //longNumPow *= Float(-262537412640768000);
        //return sumNum / prevNumDenom;
    }

    Float Newton(uint32_t)
    {
        static Float prevSum = 3;
        Float tan = prevSum;
        prevSum - tan.Tan();
        return prevSum;
    }
    
    Float Archimedes(uint32_t i)
    {
        static Float rad(180 * (bm::constants::pi<double>() / 180.0));
        Float a = rad;
        return (a / i).Sin() * i;
    }

    uint32_t Matches(const Float& decNum, uint32_t)
    {
        uint32_t same = 0;
        std::string decStr = decNum.Str().erase(0, 1);
        for (size_t i = 0; i < decStr.size(); ++i)
        {
            if (decStr[i] != PiMillionStr[i])
                break;
            ++same;
        }
        return same;
    }

    template <class Func>
    uint32_t Measure(uint32_t i, Func func)
    {
        static uint32_t start = 0;
        uint32_t same = Matches(func(i), start);
        start += same;
        return same;
    }
}