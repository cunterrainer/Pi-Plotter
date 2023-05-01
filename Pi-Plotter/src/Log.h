#pragma once
#include <string>
#include <utility>
#include <type_traits>
#include <system_error>

#ifdef WINDOWS
    #include <Windows.h>
    #undef max // windows macros
    #undef min // windows macros
    #define GetError() Logger::Error(GetLastError())
    using ColorType = WORD;
#elif defined(LINUX)
    #include <cerrno>
    #include <unistd.h>
    #define GetError() Logger::Error(errno)
    using ColorType = const char* const;
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
        static constexpr ColorType OutputColorWhite = 0b0111;
        static constexpr ColorType OutputColorLightRed = 0b1100;
    #elif defined(LINUX)
        static inline ColorType OutputColorWhite = "\033[0m";
        static inline ColorType OutputColorLightRed = "\033[1;31m";
    #endif
private:
    OutputStream& m_Os;
    const char* const m_LogInfo;
    mutable bool m_NewLine = true;
    mutable bool m_IsErr = false;
private:
    inline const char* SetOutputColor(ColorType color) const noexcept
    {
        #ifdef WINDOWS
            if (m_IsErr)
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        #elif defined(LINUX)
            if (m_IsErr && isatty(STDOUT_FILENO))
                return color;
        #endif
        return "";
    }
public:
    inline explicit Logger(const char* info, bool isErr, OutputStream& os) noexcept : m_Os(os), m_LogInfo(info), m_IsErr(isErr) {}

    // Warning: not thread safe use Print(), Println() or operator() for thread safety
    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        m_Os << *osmanip << SetOutputColor(OutputColorWhite);
        m_NewLine = true;
        return *this;
    }

    // Warning: not thread safe use Print(), Println() or operator() for thread safety
    template <class T>
    inline const Logger& operator<<(const T& msg) const noexcept
    {
        if (m_NewLine)
        {
            m_Os << SetOutputColor(OutputColorLightRed);
            m_Os << m_LogInfo;
            m_NewLine = false;
        }
        m_Os << msg;
        return *this;
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Print(Args&&... args) const noexcept
    {
        if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<OutputStream>>, ThreadSafe::Writer>)
        {
            std::scoped_lock lock{ThreadSafe::LockedWriter::Mutex};
            m_Os.NativeStream() << SetOutputColor(OutputColorLightRed) << m_LogInfo;
            ((m_Os.NativeStream() << std::forward<Args>(args)), ...) << SetOutputColor(OutputColorWhite);
        }
        else
        {
            m_Os << SetOutputColor(OutputColorLightRed) << m_LogInfo;
            ((m_Os << std::forward<Args>(args)), ...) << SetOutputColor(OutputColorWhite);
        }
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Printf(const char* format, Args&&... args) const noexcept
    {
        if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<OutputStream>>, ThreadSafe::Writer>)
        {
            std::scoped_lock lock{ ThreadSafe::LockedWriter::Mutex };
            m_Os.NativeStream() << SetOutputColor(OutputColorLightRed) << m_LogInfo << fmt::Format(format, std::forward<Args>(args)...) << SetOutputColor(OutputColorWhite);
        }
        else
            m_Os << SetOutputColor(OutputColorLightRed) << m_LogInfo << fmt::Format(format, std::forward<Args>(args)...) << SetOutputColor(OutputColorWhite);
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Printfln(const char* format, Args&&... args) const
    {
        if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<OutputStream>>, ThreadSafe::Writer>)
        {
            std::scoped_lock lock{ ThreadSafe::LockedWriter::Mutex };
            m_Os.NativeStream() << SetOutputColor(OutputColorLightRed) << m_LogInfo << fmt::Format(format, std::forward<Args>(args)...) << SetOutputColor(OutputColorWhite) << '\n';
        }
        else
            m_Os << SetOutputColor(OutputColorLightRed) << m_LogInfo << fmt::Format(format, std::forward<Args>(args)...) << SetOutputColor(OutputColorWhite) << '\n';
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Printf(const std::string& format, Args&&... args) const noexcept
    {
        Printf(format.c_str(), std::forward<Args>(args)...);
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Printfln(const std::string& format, Args&&... args) const
    {
        Printfln(format.c_str(), std::forward<Args>(args)...);
    }

    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void Println(Args&&... args) const noexcept
    {
        Print(std::forward<Args>(args)..., '\n');
    }
    
    // Info: thread safe (if operator<<() is thread safe for custom types)
    template <typename... Args>
    inline void operator()(Args&&... args) const noexcept
    {
        Print(std::forward<Args>(args)...);
    }
};
inline const Logger Log("[INFO] ", false, ThreadSafe::Stdout);
inline const Logger Err("[ERROR] ", true, ThreadSafe::Stderr);