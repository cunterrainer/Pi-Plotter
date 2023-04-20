#pragma once
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/math/constants/constants.hpp"

#include "Trigonometry.h"

namespace Pi
{
#include "pi-million.h"
}

namespace Pi
{
    namespace bm  = boost::math;
    namespace bmp = boost::multiprecision;
    using Float = bmp::number<bmp::cpp_bin_float<10000>>;

    Float Chudnovsky(uint32_t i)
    {
        static Float prevNumDenom = 0;
        static Float longNumPow = 1; // -262537412640768000^0 = 1
        static Float sumNum = Float(426880 * bmp::sqrt(Float(10005)));
        static Float a(545140134);
        static Float b(13591409);

        --i;
        const Float num = Math::Factorial<Float>(6 * i) * (a * i + b);
        const Float denom = Math::Factorial<Float>(3 * i) * bmp::pow(Math::Factorial<Float>(i), 3) * longNumPow;
        prevNumDenom += num / denom;
        longNumPow *= Float(-262537412640768000);
        return sumNum / prevNumDenom;
    }

    Float Newton(uint32_t)
    {
        static Float prevSum = 3;
        prevSum = prevSum - Math::Tan<200>(prevSum);
        return prevSum;
    }

    Float Archimedes(uint32_t i)
    {
        static Float rad(180 * (bm::constants::pi<double>() / 180.0));
        return i * Math::Sin(rad / i);
    }

    uint32_t Matches(const Float& decNum, uint32_t)
    {
        uint32_t same = 0;
        std::string decStr = decNum.str().erase(0, 2);
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