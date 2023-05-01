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

#define ARCHIMEDES_FUNC Calculate<decltype(Pi::Algorithm::Archimedes), 1000000000, 100>, &window, Pi::Algorithm::Archimedes, RenderWindow::PlotID::Archimedes
#define CHUDNOVSKY_FUNC Calculate<decltype(Pi::Algorithm::Chudnovsky),     100000, 100>, &window, Pi::Algorithm::Chudnovsky, RenderWindow::PlotID::Chudnovsky
#define NEWTON_FUNC     Calculate<decltype(Pi::Algorithm::Newton),             20,   1>, &window, Pi::Algorithm::Newton,     RenderWindow::PlotID::Newton

template <typename Func, uint32_t Iterations, uint32_t iterMod>
inline void Calculate(RenderWindow* window, Func func, RenderWindow::PlotID identifier)
{
    Log.Printfln("Successfully started thread id=%u (%u, %s)", std::this_thread::get_id(), identifier, Utility::PlotIDString(identifier));
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
    Log.Printfln("Successfully stopped thread id=%u (%u, %s)", std::this_thread::get_id(), identifier, Utility::PlotIDString(identifier));
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
                switch (window.SelectedAlgorithm())
                {
                case RenderWindow::Archimedes:
                    archimedes = Thread::Dispatch(ARCHIMEDES_FUNC);
                    break;
                case RenderWindow::Chudnovsky:
                    chudnovsky = Thread::Dispatch(CHUDNOVSKY_FUNC);
                    break;
                case RenderWindow::Newton:
                    newton = Thread::Dispatch(NEWTON_FUNC);
                    break;
                default:
                    archimedes = Thread::Dispatch(ARCHIMEDES_FUNC);
                    chudnovsky = Thread::Dispatch(CHUDNOVSKY_FUNC);
                    newton     = Thread::Dispatch(NEWTON_FUNC);
                    break;
                }
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