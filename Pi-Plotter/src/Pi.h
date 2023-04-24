#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <string_view>

#include "BigInt.h"
#include "BigFloat.h"
#include "pi-string.h"
#include "FactorialTable.h"


namespace Pi
{
    inline constexpr double PI = 3.14159265358979323846;
    using Float = BigFloat<150000>;
    inline const std::string_view PiStr = PiBillionStr;

    Float Chudnovsky(uint32_t i)
    {
        static constexpr unsigned long int b = 13591409;
        static const BigInt longNum = "-262537412640768000";
        static const BigInt a = 545140134;
        static const Float sumNum = Float(426880) * Float(10005).Sqrt();
        static BigInt longNumPow = 1; // -262537412640768000^0 = 1
        static Float prevNumDenom = 0;
    
        --i;
        const Float num = FactTable[6 * i] * (a * i + b);
        const BigInt denom = FactTable[i].Pow(3) * FactTable[3 * i] * longNumPow;
        prevNumDenom += num / denom;
        longNumPow *= longNum;
        return sumNum / prevNumDenom;
    }

    Float Newton(uint32_t)
    {
        static Float prevSum = 3;
        prevSum -= Float(prevSum).Tan();
        return prevSum;
    }
    
    Float Archimedes(uint32_t i)
    {
        static Float rad(180 * (PI / 180.0));
        Float a = rad;
        return (a / i).Sin() * i;
    }

    uint32_t Matches(const Float& decNum, uint32_t start)
    {
        const std::string decStr = decNum.Str().erase(0, 1);

        uint32_t i = start;
        while (i < decStr.size() && decStr[i] == PiStr[i]) ++i;
        return i - start;
    }
}