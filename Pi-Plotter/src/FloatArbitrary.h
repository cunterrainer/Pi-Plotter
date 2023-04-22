#pragma once
#include <string>

#include "mpfr.h"

#define ROUNDING_MODE MPFR_RNDD

/*
    Precision doesn't represent the number of decimal places but rather
    the number of bits used to store the number. Thus you will have to multiply precision by
    about 4 (3.5 on average according to stack overflow). That's because in order to store on
    single digit (number 0-9) you'll need at max 4 bits (9 == 1111[bin]) and the number of
    bits is what you assign here
*/

template <size_t DecimalPlaces>
class FloatArbitrary
{
public:
    static_assert(DecimalPlaces != 0, "DecimalPlaces has to be greater than 0!");
private:
    static constexpr size_t Precision = (size_t)(3.5 * DecimalPlaces);
    mpfr_t m_Val;
public:
    inline ~FloatArbitrary()
    {
        mpfr_clear(m_Val);
    }

    inline FloatArbitrary(const FloatArbitrary& other)
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set(m_Val, other.m_Val, ROUNDING_MODE);
    }

    inline FloatArbitrary(int value)
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set_si(m_Val, value, ROUNDING_MODE);
    }

    inline FloatArbitrary(double value)
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set_d(m_Val, value, ROUNDING_MODE);
    }

    inline FloatArbitrary(const char* value)
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set_str(m_Val, value, 10, ROUNDING_MODE);
    }

    inline FloatArbitrary(const MP_INT& value)
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set_z(m_Val, &value, ROUNDING_MODE);
    }

    inline FloatArbitrary& operator=(const MP_INT& x)
    {
        mpfr_set_z(m_Val, &x, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator=(unsigned long x)
    {
        mpfr_set_ui(m_Val, x, ROUNDING_MODE);
        return *this;
    }

    inline void Assign(const char* value)
    {
        mpfr_set_str(m_Val, value, 10, ROUNDING_MODE);
    }

    inline FloatArbitrary& Factorial(unsigned long int op)
    {
        mpfr_fac_ui(m_Val, op, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& Pow(unsigned long int exp)
    {
        mpfr_pow_ui(m_Val, m_Val, exp, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& Sqrt()
    {
        mpfr_sqrt(m_Val, m_Val, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& Sin()
    {
        mpfr_sin(m_Val, m_Val, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& Tan()
    {
        mpfr_tan(m_Val, m_Val, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator*(unsigned long int op)
    {
        mpfr_mul_ui(m_Val, m_Val, op, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator*(const MP_INT& op)
    {
        mpfr_mul_z(m_Val, m_Val, &op, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator+(unsigned long int op)
    {
        mpfr_add_ui(m_Val, m_Val, op, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator-(const FloatArbitrary& other)
    {
        mpfr_sub(m_Val, m_Val, other.m_Val, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator*(const FloatArbitrary& other)
    {
        mpfr_mul(m_Val, m_Val, other.m_Val, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary operator/(unsigned long int op)
    {
        mpfr_div_ui(m_Val, m_Val, op, ROUNDING_MODE);
        return *this;
    }

    inline FloatArbitrary& operator/(const FloatArbitrary& other)
    {
        mpfr_div(m_Val, m_Val, other.m_Val, ROUNDING_MODE);
        return *this;
    }

    inline void operator+=(const FloatArbitrary& other)
    {
        mpfr_add(m_Val, m_Val, other.m_Val, ROUNDING_MODE);
    }

    inline void operator*=(const mpz_t& op)
    {
        mpfr_mul_z(m_Val, m_Val, op, ROUNDING_MODE);
    }

    inline void operator*=(const FloatArbitrary& other)
    {
        mpfr_mul(m_Val, m_Val, other.m_Val, ROUNDING_MODE);
    }

    inline void Print() const
    {
        mpfr_out_str(stdout, 10, 10, m_Val, ROUNDING_MODE);
        std::cout << std::endl;
    }

    inline std::string Str() const
    {
        char* abc = NULL;
        mpfr_exp_t i = 0;
        abc = mpfr_get_str(NULL, &i, 10, Precision, m_Val, ROUNDING_MODE);
        std::string ret(abc);
        mpfr_free_str(abc);
        return ret;
    }
};