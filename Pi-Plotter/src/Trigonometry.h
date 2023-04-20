#pragma once
#include "boost/multiprecision/cpp_bin_float.hpp"

namespace Math
{
    template <typename T>
    T Factorial(size_t n)
    {
        if (n <= 1)
            return 1;
        T result = 1;
        for (size_t i = 2; i < n + 1; ++i)
            result *= i;
        return result;
    }

    template <size_t Iterations = 10, typename T>
    T Sin(const T& x)
    {
        T res = 0;
        T term = x;
        T sign = 1;
        for (size_t i = 1; i <= Iterations; i++)
        {
            res += term;
            term *= -x * x / ((2 * i) * (2 * i + 1));
            sign *= -1;
        }
        return res;
    }

    template <size_t Iterations = 10, typename T>
    T Cos(const T& x) 
    {
        T res = 1;
        T term = 1;
        for (size_t i = 1; i <= Iterations; ++i)
        {
            term *= (-x * x) / ((2 * i - 1) * (2 * i));
            res += term;
        }
        return res;
    }

    template <size_t Iterations = 10, typename T>
    T Tan(const T& x)
    {
        return Sin<Iterations>(x) / Cos<Iterations>(x);
    }
}