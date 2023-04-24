#pragma once

#include "gmp.h"

class BigInt
{
private:
    mpz_t m_Val;
public:
    inline BigInt(unsigned int value) noexcept
    {
        mpz_init(m_Val);
        mpz_set_ui(m_Val, value);
    }

    inline BigInt(const char* str) noexcept
    {
        mpz_init(m_Val);
        mpz_set_str(m_Val, str, 10);
    }

    inline ~BigInt() noexcept
    {
        mpz_clear(m_Val);
    }

    inline const mpz_t& MPZ() const noexcept { return m_Val; }
    inline void operator*=(const BigInt& op) noexcept { mpz_mul(m_Val, m_Val, op.m_Val); }
};