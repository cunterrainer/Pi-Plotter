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
    inline const std::string_view PiStr = PiBillionStr;

    struct Algorithm
    {
    public:
        using FloatA = BigFloat<100>;
        using FloatC = BigFloat<150000>;
        using FloatN = BigFloat<100000>;
        static_assert(!std::disjunction_v<std::is_same<FloatA, FloatC>, std::is_same<FloatA, FloatN>, std::is_same<FloatC, FloatN>>, "FloatA & FloatC & FloatN have to be different types (different 'DecimalPlaces' parameters)");
    private:
        static inline BigInt ChudnovskyLongNumPow = 1; // -262537412640768000^0 = 1
        static inline FloatC ChudnovskyPrevNumDenom = 0;
        static inline FloatN NewtonPrevSum = 3;
    public:
        static inline void Reset()
        {
            ChudnovskyLongNumPow = 1;
            ChudnovskyPrevNumDenom = 0;
            NewtonPrevSum = 3;
        }

        static inline FloatC Chudnovsky(uint32_t i)
        {
            static constexpr unsigned long int b = 13591409;
            static const BigInt longNum = "-262537412640768000";
            static const BigInt a = 545140134;
            static const FloatC sumNum = FloatC(426880) * FloatC(10005).Sqrt();

            --i;
            const FloatC num = FactTable[6 * i] * (a * i + b);
            const BigInt denom = FactTable[i].Pow(3) * FactTable[3 * i] * ChudnovskyLongNumPow;
            ChudnovskyPrevNumDenom += num / denom;
            ChudnovskyLongNumPow *= longNum;
            return (sumNum / ChudnovskyPrevNumDenom);
        }

        static inline FloatN Newton(uint32_t)
        {
            NewtonPrevSum -= FloatN(NewtonPrevSum).Tan();
            return NewtonPrevSum;
        }

        static inline FloatA Archimedes(uint32_t i)
        {
            static FloatA rad(180 * (PI / 180.0));
            FloatA a = rad;
            return (a / i).Sin() * i;
        }
    };

    template <typename FloatVariant>
    uint32_t Matches(const FloatVariant& decNum, uint32_t start)
    {
        const std::string_view decStr = decNum.Str();
        uint32_t i = start;
        while (i < decStr.size() && decStr[i] == PiStr[i]) ++i;
        return i - start;
    }
}