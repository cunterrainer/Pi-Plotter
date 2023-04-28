#pragma once
#include <mutex>
#include <ostream>
#include <iostream>

namespace ThreadSafe
{
    struct LockedWriter final
    {
    private:
        static inline std::mutex Mutex;
        std::ostream& m_Os;
    public:
        inline LockedWriter() noexcept = delete;
        inline LockedWriter(LockedWriter&&) noexcept = delete;
        inline LockedWriter(const LockedWriter&) noexcept = delete;
        inline LockedWriter& operator=(LockedWriter&&) const noexcept = delete;
        inline LockedWriter& operator=(const LockedWriter&) const noexcept = delete;
        inline ~LockedWriter() noexcept { Mutex.unlock(); }

        template <typename T>
        inline explicit LockedWriter(std::ostream& os, const T& msg) noexcept : m_Os(os) 
        { 
            Mutex.lock();
            m_Os << msg;
        }

        inline const LockedWriter& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
        {
            m_Os << *osmanip;
            return *this;
        }

        template <class T>
        inline const LockedWriter& operator<<(const T& mess) const noexcept
        {
            m_Os << mess;
            return *this;
        }
    };

    struct Writer final
    {
    private:
        std::ostream& m_Os;
    public:
        inline Writer(Writer&&) noexcept = delete;
        inline Writer(const Writer&) noexcept = delete;
        inline Writer& operator=(Writer&&) const noexcept = delete;
        inline Writer& operator=(const Writer&) const noexcept = delete;
        inline explicit Writer(std::ostream& os) noexcept : m_Os(os) {}

        inline LockedWriter operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
        {
            return LockedWriter(m_Os, osmanip);
        }

        template <typename T>
        inline LockedWriter operator<<(const T& mess) const noexcept
        {
            return LockedWriter(m_Os, mess);
        }
    };
    inline const Writer Stdout(std::cout);
    inline const Writer Stderr(std::cerr);
}