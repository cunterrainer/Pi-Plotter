#pragma once
#include <memory>
#include <atomic>
#include <thread>
#include <exception>
#include <system_error>

#include "Log.h"
#include "Utility.h"
#include "Profiler.h"
#include "RenderWindow.h"

namespace Thread
{
    inline void Join(const std::unique_ptr<std::thread>& thread) noexcept
    {
        if (thread)
            thread->join();
    }

    inline std::atomic_bool ThreadsShouldStop = false;
    inline void StopThreads(const bool& threadsActive, const std::unique_ptr<std::thread>& archimedes, const std::unique_ptr<std::thread>& chudnovsky, const std::unique_ptr<std::thread>& newton)
    {
        if (!threadsActive)
            return;
        Log << "Attempting to stop threads" << Endl;
        Profiler::Start();
        ThreadsShouldStop = true;
        Join(archimedes);
        Join(chudnovsky);
        Join(newton);
        ThreadsShouldStop = false;
        Profiler::End();
        Log << "Successfully stoppen all threads" << Endl;
        Profiler::Log(Profiler::Conversion::Seconds);
    }

    template <typename Func, typename... Args>
    inline std::unique_ptr<std::thread> Dispatch(Func&& func, Args&& ...args)
    {
        static std::string errorMsg;
        const uint32_t identifier = (Utility::Last(std::forward<Args>(args)), ...);
        const char* const algorithm = Utility::PlotIDString(static_cast<RenderWindow::PlotID>(identifier));

        try
        {
            return std::make_unique<std::thread>(std::thread(func, std::forward<Args>(args)...));
        }
        catch (const std::system_error& e)
        {
            errorMsg = fmt::Format("System error on thread creation with code: %d what: %s (%u, %s)", e.code(), e.what(), identifier, algorithm);
        }
        catch (const std::exception& e)
        {
            errorMsg = fmt::Format("Exception on thread creation what: %s (%u, %s)", e.what(), identifier, algorithm);
        }
        catch (...)
        {
            errorMsg = fmt::Format("Unhandled exception on thread creation (%u, %s)", identifier, algorithm);
        }
        Log.Println(errorMsg);
        MsgBoxError(errorMsg.c_str());
        return nullptr;
    }
}