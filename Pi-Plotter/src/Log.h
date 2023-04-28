#pragma once
#include <string>
#include <system_error>

#ifdef WINDOWS
    #include <Windows.h>
    #undef max // windows macros
    #undef min // windows macros
    #define GetError() Logger::Error(GetLastError())
#elif defined(LINUX)
    #include <cerrno>
    #include <unistd.h>
    #define GetError() Logger::Error(errno)
#endif

#include "ThreadSafe.h"
#define Endl std::endl

template <typename OutputStream>
struct Logger
{
public:
    static std::string Error(int error)
    {
        return "Error: " + std::to_string(error) + " (" + std::system_category().message(static_cast<int>(error)) + ")";
    }
private:
    #ifdef WINDOWS
        static constexpr unsigned char OutputColorWhite = 0b0111;
        static constexpr unsigned char OutputColorLightRed = 0b1100;
    #endif
private:
    OutputStream& m_Os;
    const char* const m_LogInfo;
    mutable bool m_NewLine = true;
    mutable bool m_IsErr = false;
private:
    inline void SetOutputColor() const noexcept
    {
        #ifdef WINDOWS
            if(m_IsErr)
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), OutputColorLightRed);
        #elif defined(LINUX)
            if(m_IsErr && isatty(STDOUT_FILENO))
                m_Os << "\033[1;31m";
        #endif
    }

    inline void ResetOutputColor() const noexcept
    {
        #ifdef WINDOWS
            if(m_IsErr)
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), OutputColorWhite);
        #elif defined(LINUX)
            if(m_IsErr && isatty(STDOUT_FILENO))
                m_Os << "\033[0m";
        #endif
    }
public:
    inline explicit Logger(const char* info, bool isErr, OutputStream& os) noexcept : m_Os(os), m_LogInfo(info), m_IsErr(isErr) {}

    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        m_Os << *osmanip;
        ResetOutputColor();
        m_NewLine = true;
        return *this;
    }

    template <class T>
    inline const Logger& operator<<(const T& msg) const noexcept
    {
        if (m_NewLine)
        {
            SetOutputColor();
            m_Os << m_LogInfo;
            m_NewLine = false;
        }
        m_Os << msg;
        return *this;
    }
};
inline const Logger Log("[INFO] ", false, ThreadSafe::Stdout);
inline const Logger Err("[ERROR] ", true, ThreadSafe::Stderr);