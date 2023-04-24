#pragma once

#include "gmp.h"

class BigInt
{
private:
    mpz_t m_Val = {0};
private:
    template <typename Func, typename T>
    inline BigInt Operation(const Func& func, const T& op) const noexcept
    {
        BigInt tmp(*this);
        func(tmp.m_Val, tmp.m_Val, op);
        return tmp;
    }
public:
    inline BigInt(unsigned int value) noexcept
    {
        mpz_init(m_Val);
        mpz_set_ui(m_Val, value);
    }

    inline BigInt(const char* value) noexcept
    {
        mpz_init(m_Val);
        mpz_set_str(m_Val, value, 10);
    }

    inline BigInt(const mpz_t& value) noexcept
    {
        mpz_init(m_Val);
        mpz_set(m_Val, value);
    }

    inline BigInt(const BigInt& other) noexcept
    {
        mpz_init(m_Val);
        mpz_set(m_Val, other.m_Val);
    }

    inline BigInt& operator=(const BigInt& other) noexcept
    {
        mpz_set(m_Val, other.m_Val);
        return *this;
    }

    inline BigInt(BigInt&& other) noexcept
    {
        m_Val->_mp_d     = other.m_Val->_mp_d;
        m_Val->_mp_size  = other.m_Val->_mp_size;
        m_Val->_mp_alloc = other.m_Val->_mp_alloc;
        other.m_Val->_mp_d     = nullptr;
        other.m_Val->_mp_size  = 0;
        other.m_Val->_mp_alloc = 0;
    }

    inline BigInt& operator=(BigInt&& other) noexcept
    {
        m_Val->_mp_d     = other.m_Val->_mp_d;
        m_Val->_mp_size  = other.m_Val->_mp_size;
        m_Val->_mp_alloc = other.m_Val->_mp_alloc;
        other.m_Val->_mp_d     = nullptr;
        other.m_Val->_mp_size  = 0;
        other.m_Val->_mp_alloc = 0;
        return *this;
    }

    inline ~BigInt() noexcept
    {
        if(m_Val->_mp_d)
        {
            mpz_clear(m_Val);
            m_Val->_mp_d = nullptr;
        }
    }

    inline BigInt Pow      (unsigned long int exp) const noexcept { return Operation(mpz_pow_ui, exp);   }
    inline BigInt operator+(unsigned long int op)  const noexcept { return Operation(mpz_add_ui, op);    }
    inline BigInt operator*(unsigned long int op)  const noexcept { return Operation(mpz_mul_ui, op);    }
    inline BigInt operator*(const BigInt& op)      const noexcept { return Operation(mpz_mul, op.m_Val); }

    inline const mpz_t& MPZ() const noexcept { return m_Val; }
    inline void operator*=(const BigInt& op)     noexcept { mpz_mul(m_Val, m_Val, op.m_Val); }
    inline void operator*=(unsigned long int op) noexcept { mpz_mul_ui(m_Val, m_Val, op);    }
};