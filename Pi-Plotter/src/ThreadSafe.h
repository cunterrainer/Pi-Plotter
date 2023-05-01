#pragma once
#include <mutex>
#include <cstdio>
#include <utility>
#include <ostream>
#include <iostream>

namespace fmt
{
    template <typename... Args>
    inline std::string Format(const char* format, Args&&... args) noexcept
    {
        size_t size = (size_t)std::snprintf(NULL, 0, format, std::forward<Args>(args)...) + 1; // Extra space for '\0'
        std::string res(size, 0);
        std::snprintf(res.data(), size, format, std::forward<Args>(args)...);
        res.pop_back(); // '\0'
        return res;
    }

    template <typename... Args>
    inline std::string Format(const std::string& format, Args&&... args) noexcept
    {
        return Format(format.c_str(), std::forward<Args>(args)...);
    }
}

namespace ThreadSafe
{
    struct LockedWriter final
    {
    public:
        static inline std::mutex Mutex;
    private:
        const std::scoped_lock<std::mutex> m_Lock{Mutex};
        std::ostream& m_Os;
    public:
        inline LockedWriter() noexcept = delete;
        inline LockedWriter(LockedWriter&&) noexcept = delete;
        inline LockedWriter(const LockedWriter&) noexcept = delete;
        inline LockedWriter& operator=(LockedWriter&&) const noexcept = delete;
        inline LockedWriter& operator=(const LockedWriter&) const noexcept = delete;
        inline explicit LockedWriter(std::ostream& os) noexcept : m_Os(os) {}

        template <typename T>
        inline explicit LockedWriter(std::ostream& os, const T& msg) noexcept : LockedWriter(os)
        { 
            m_Os << msg;
        }

        // Modifing the stream is thread safe
        inline std::ostream& NativeStream() const noexcept
        {
            return m_Os;
        }

        inline const LockedWriter& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
        {
            m_Os << *osmanip;
            return *this;
        }

        template <typename T>
        inline const LockedWriter& operator<<(const T& mess) const noexcept
        {
            m_Os << mess;
            return *this;
        }

        template <typename... Args>
        inline const LockedWriter& Print(Args&&... args) const noexcept
        {
            return ((*this << std::forward<Args>(args)), ...);
        }

        template <typename... Args>
        inline void Printf(const char* format, Args&&... args) const
        {
            m_Os << fmt::Format(format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Printfln(const char* format, Args&&... args) const
        {
            m_Os << fmt::Format(format, std::forward<Args>(args)...) << '\n';
        }

        template <typename... Args>
        inline const LockedWriter& Println(Args&&... args) const noexcept
        {
            return Print(std::forward<Args>(args)...) << '\n';
        }

        template <typename... Args>
        inline const LockedWriter& operator()(Args&&... args) const noexcept
        {
            return Print(std::forward<Args>(args)...);
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

        // Warning: modifing the native stream is not thread safe,
        // use 'operator<<().NativeStream()' to achieve thread safety
        inline std::ostream& NativeStream() const noexcept
        {
            return m_Os;
        }

        inline LockedWriter operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
        {
            return LockedWriter(m_Os, osmanip); // Has to be constructed in place of the caller from C++17 on
        }

        template <typename T>
        inline LockedWriter operator<<(const T& msg) const noexcept
        {
            return LockedWriter(m_Os, msg); // Has to be constructed in place of the caller from C++17 on
        }

        template <typename... Args>
        inline void Print(Args&&... args) const noexcept
        {
            LockedWriter(m_Os).Print(std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Printf(const char* format, Args&&... args) const noexcept
        {
            LockedWriter(m_Os).Printf(format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Printfln(const char* format, Args&&... args) const
        {
            LockedWriter(m_Os).Printfln(format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Printf(const std::string& format, Args&&... args) const noexcept
        {
            LockedWriter(m_Os).Printf(format.c_str(), std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Printfln(const std::string& format, Args&&... args) const
        {
            LockedWriter(m_Os).Printfln(format.c_str(), std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void Println(Args&&... args) const noexcept
        {
            LockedWriter(m_Os).Println(std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void operator()(Args&&... args) const noexcept
        {
            LockedWriter(m_Os).operator()(std::forward<Args>(args)...);
        }
    };
    inline const Writer Stdout(std::cout);
    inline const Writer Stderr(std::cerr);
}