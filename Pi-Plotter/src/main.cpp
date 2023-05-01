#include <thread>
#include <limits>
#include <memory>
#include <cstdint>
#include <functional>

#include "mpfr.h"

#include "Pi.h"
#include "Log.h"
#include "Thread.h"
#include "RenderWindow.h"

template <typename Func, uint32_t Iterations, uint32_t iterMod>
inline void Calculate(RenderWindow* window, Func func, RenderWindow::PlotID identifier)
{
    Log.Printfln("Successfully started thread (id=%u [%s])", std::this_thread::get_id(), typeid(Func).name());
    static_assert(Iterations % iterMod == 0, "Iterations & iterMod must satisfy: 'Iterations % iterMod == 0'");
    static_assert(Iterations != 0 && iterMod != 0, "Iterations & iterMod must satisfy: 'Iterations > 0 & iterMod > 0'");
    uint32_t start = 0;
    window->Add(0, 0, identifier);
    for (uint32_t i = 1; i <= Iterations && !Thread::ThreadsShouldStop; ++i)
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


int main()
{
    RenderWindow window;
    std::unique_ptr<std::thread> archimedes;
    std::unique_ptr<std::thread> chudnovsky;
    std::unique_ptr<std::thread> newton;
    bool threadsActive = false;
    const std::function<void()> stopThreads = std::bind(Thread::StopThreads, std::cref(threadsActive), std::cref(archimedes), std::cref(chudnovsky), std::cref(newton));

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
                archimedes = Thread::Dispatch(Calculate<decltype(Pi::Algorithm::Archimedes), 1000000000, 100>, &window, Pi::Algorithm::Archimedes, RenderWindow::PlotID::Archimedes);
                chudnovsky = Thread::Dispatch(Calculate<decltype(Pi::Algorithm::Chudnovsky),     100000, 100>, &window, Pi::Algorithm::Chudnovsky, RenderWindow::PlotID::Chudnovsky);
                newton     = Thread::Dispatch(Calculate<decltype(Pi::Algorithm::Newton),             20,   1>, &window, Pi::Algorithm::Newton,     RenderWindow::PlotID::Newton);
                if (archimedes || chudnovsky || newton)
                    threadsActive = true;
            }
        }
        window.EndFrame();
    }

    stopThreads();
    mpfr_free_cache();
    return 0;
}