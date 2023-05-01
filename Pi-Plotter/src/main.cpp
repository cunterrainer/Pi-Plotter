#include <thread>
#include <atomic>
#include <limits>
#include <cstdint>
#include <functional>

#include "mpfr.h"

#include "Pi.h"
#include "Log.h"
#include "RenderWindow.h"

inline std::atomic_bool ThreadsShouldStop = false;
template <typename Func, uint32_t Iterations, uint32_t iterMod>
inline void Calculate(RenderWindow* window, Func func, uint8_t identifier)
{
    Log.Printfln("Successfully started thread (id=%u [%s])", std::this_thread::get_id(), typeid(Func).name());
    static_assert(Iterations % iterMod == 0, "Iterations & iterMod must satisfy: 'Iterations % iterMod == 0'");
    static_assert(Iterations != 0 && iterMod != 0, "Iterations & iterMod must satisfy: 'Iterations > 0 & iterMod > 0'");
    uint32_t start = 0;
    window->Add(0, 0, identifier);
    for (uint32_t i = 1; i <= Iterations && !ThreadsShouldStop; ++i)
    {
        const auto value = func(i);
        if (i % iterMod == 0 || i == 1)
        {
            start += Pi::Matches(value, start);
            window->Add(i, start, identifier);
        }
    }
    Log.Printfln("Successfully stopped thread (id=%u [%s])", std::this_thread::get_id(), typeid(Func).name());
}


inline void StopThreads(const bool& threadsActive, std::thread& archimedes, std::thread& chudnovsky, std::thread& newton)
{
    if (!threadsActive)
        return;
    Log << "Attempting to stop threads" << Endl;
    Profiler::Start();
    ThreadsShouldStop = true;
    archimedes.join();
    chudnovsky.join();
    newton.join();
    ThreadsShouldStop = false;
    Profiler::End();
    Log << "Successfully stoppen all threads\n";
    Profiler::Log(Profiler::Conversion::Seconds);
}


int main()
{
    RenderWindow window;
    std::thread archimedes;
    std::thread chudnovsky;
    std::thread newton;
    bool threadsActive = false;
    const std::function<void()> stopThreads = std::bind(StopThreads, std::cref(threadsActive), std::ref(archimedes), std::ref(chudnovsky), std::ref(newton));

    while (window.IsOpen())
    {
        if (window.Show(threadsActive))
        {
            if (threadsActive)
            {
                stopThreads();
                window.ResetPlots();
                Pi::Algorithm::Reset();
                threadsActive = false;
            }
            else
            {
                archimedes = std::thread(Calculate<decltype(Pi::Algorithm::Archimedes), 1000000000, 100>, &window, Pi::Algorithm::Archimedes, 0);
                chudnovsky = std::thread(Calculate<decltype(Pi::Algorithm::Chudnovsky), 100000, 100>, &window, Pi::Algorithm::Chudnovsky, 1);
                newton = std::thread(Calculate<decltype(Pi::Algorithm::Newton), 20, 1>, &window, Pi::Algorithm::Newton, 2);
                threadsActive = true;
            }
        }
        window.EndFrame();
    }

    stopThreads();
    mpfr_free_cache();
    return 0;
}