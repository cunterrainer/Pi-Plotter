#pragma once
#include <string>
#include <string_view>

#include "mpfr.h"

#include "BigInt.h"

#define ROUNDING_MODE MPFR_RNDD

/*
    Precision doesn't represent the number of decimal places but rather
    the number of bits used to store the number. Thus you will have to multiply precision by
    about 4 (3.5 on average according to stack overflow). That's because in order to store on
    single digit (number 0-9) you'll need at max 4 bits (9 == 1111[bin]) and the number of
    bits is what you assign here
*/

template <size_t DecimalPlaces>
class BigFloat
{
    static_assert(DecimalPlaces != 0, "DecimalPlaces has to be greater than 0!");
private:
    static constexpr size_t Precision = (size_t)(3.5 * DecimalPlaces);
    static inline const std::string s_Format = "%." + std::to_string(Precision) + "RDf";
    static inline std::string s_Str = std::string(Precision + 2, 0);
private:
    mpfr_t m_Val;
private:
    template <typename Func>
    inline BigFloat& Operation(const Func& func) noexcept
    {
        func(m_Val, m_Val, ROUNDING_MODE);
        return *this;
    }

    template <typename Func, typename T>
    inline BigFloat Operation(const Func& func, const T& op) const noexcept
    {
        BigFloat tmp(*this);
        func(tmp.m_Val, tmp.m_Val, op, ROUNDING_MODE);
        return tmp;
    }

    template <typename Func, typename T>
    inline void Construct(const Func& func, const T& value)
    {
        mpfr_init2(m_Val, Precision);
        func(m_Val, value, ROUNDING_MODE);
    }
public:
    inline BigFloat(int value)             noexcept { Construct(mpfr_set_si, value);       }
    inline BigFloat(double value)          noexcept { Construct(mpfr_set_d,  value);       }
    inline BigFloat(const BigInt& value)   noexcept { Construct(mpfr_set_z,  value.MPZ()); }
    inline BigFloat(const BigFloat& other) noexcept { Construct(mpfr_set, other.m_Val);    } // copy

    inline BigFloat(const char* value) noexcept
    {
        mpfr_init2(m_Val, Precision);
        mpfr_set_str(m_Val, value, 10, ROUNDING_MODE);
    }

    inline BigFloat& operator=(const BigFloat& other) noexcept
    {
        mpfr_set(m_Val, other.m_Val, ROUNDING_MODE);
        return *this;
    }

    inline BigFloat(BigFloat&& other) noexcept
    {
        m_Val->_mpfr_d    = other.m_Val->_mpfr_d;
        m_Val->_mpfr_exp  = other.m_Val->_mpfr_exp;
        m_Val->_mpfr_prec = other.m_Val->_mpfr_prec;
        m_Val->_mpfr_sign = other.m_Val->_mpfr_sign;
        other.m_Val->_mpfr_d    = nullptr;
        other.m_Val->_mpfr_exp  = 0;
        other.m_Val->_mpfr_prec = 0;
        other.m_Val->_mpfr_sign = 0;
    }

    inline BigFloat& operator=(BigFloat&& other) noexcept
    {
        m_Val->_mpfr_d    = other.m_Val->_mpfr_d;
        m_Val->_mpfr_exp  = other.m_Val->_mpfr_exp;
        m_Val->_mpfr_prec = other.m_Val->_mpfr_prec;
        m_Val->_mpfr_sign = other.m_Val->_mpfr_sign;
        other.m_Val->_mpfr_d    = nullptr;
        other.m_Val->_mpfr_exp  = 0;
        other.m_Val->_mpfr_prec = 0;
        other.m_Val->_mpfr_sign = 0;
        return *this;
    }

    inline ~BigFloat() noexcept
    {
        if (m_Val->_mpfr_d)
        {
            mpfr_clear(m_Val);
            m_Val->_mpfr_d = nullptr;
        }
    }

    inline BigFloat& Sin()  noexcept { return Operation(mpfr_sin);  }
    inline BigFloat& Tan()  noexcept { return Operation(mpfr_tan);  }
    inline BigFloat& Sqrt() noexcept { return Operation(mpfr_sqrt); }

    inline BigFloat operator-(const BigFloat& op)   const noexcept { return Operation(mpfr_sub,    op.m_Val); }
    inline BigFloat operator*(const BigFloat& op)   const noexcept { return Operation(mpfr_mul,    op.m_Val); }
    inline BigFloat operator*(unsigned long int op) const noexcept { return Operation(mpfr_mul_ui, op);       }
    inline BigFloat operator/(const BigFloat& op)   const noexcept { return Operation(mpfr_div,    op.m_Val); }
    inline BigFloat operator/(const BigInt& op)     const noexcept { return Operation(mpfr_div_z,  op.MPZ()); }
    inline BigFloat operator/(unsigned long op)     const noexcept { return Operation(mpfr_div_ui, op);       }

    inline void operator+=(const BigFloat& op) noexcept { mpfr_add  (m_Val, m_Val, op.m_Val, ROUNDING_MODE); }
    inline void operator-=(const BigFloat& op) noexcept { mpfr_sub  (m_Val, m_Val, op.m_Val, ROUNDING_MODE); }

    inline std::string_view Str() const
    {
        mpfr_sprintf(s_Str.data(), s_Format.c_str(), m_Val);
        return { &s_Str.c_str()[2], Precision };
    }
};