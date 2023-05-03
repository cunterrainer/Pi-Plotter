#include <thread>
#include <limits>
#include <memory>
#include <cstdint>
#include <functional>

#include "mpfr.h"

#include "Pi.h"
#include "Log.h"
#include "Input.h"
#include "Thread.h"
#include "RenderWindow.h"

#define ARCHIMEDES_FUNC(iter, mod) Calculate<decltype(Pi::Algorithm::Archimedes)>, &window, Pi::Algorithm::Archimedes, iter, mod, RenderWindow::PlotID::Archimedes
#define CHUDNOVSKY_FUNC(iter, mod) Calculate<decltype(Pi::Algorithm::Chudnovsky)>, &window, Pi::Algorithm::Chudnovsky, iter, mod, RenderWindow::PlotID::Chudnovsky
#define NEWTON_FUNC(iter, mod)     Calculate<decltype(Pi::Algorithm::Newton)>,     &window, Pi::Algorithm::Newton, iter, mod,     RenderWindow::PlotID::Newton

template <typename Func>
inline void Calculate(RenderWindow* window, Func func, uint32_t iterations, uint32_t iterMod, RenderWindow::PlotID identifier)
{
    Log.Printfln("Successfully started thread id=%u (%u, %s)", std::this_thread::get_id(), identifier, Utility::PlotIDString(identifier));
    uint32_t start = 0;
    window->Add(0, 0, identifier);
    for (uint32_t i = 1; i <= iterations && !Thread::ThreadsShouldStop; ++i)
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


int main(int argc, const char** argv)
{
    const CmdSettings cs = ParseCmd(argc, argv);
    if(!ApplySettings(cs)) return 0;
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
                    archimedes = Thread::Dispatch(ARCHIMEDES_FUNC(cs.archimedesArgs[1], cs.archimedesArgs[2]));
                    break;
                case RenderWindow::Chudnovsky:
                    chudnovsky = Thread::Dispatch(CHUDNOVSKY_FUNC(cs.chudnovskyArgs[1], cs.chudnovskyArgs[2]));
                    break;
                case RenderWindow::Newton:
                    newton = Thread::Dispatch(NEWTON_FUNC(cs.newtonArgs[1], cs.newtonArgs[2]));
                    break;
                default:
                    archimedes = Thread::Dispatch(ARCHIMEDES_FUNC(cs.archimedesArgs[1], cs.archimedesArgs[2]));
                    chudnovsky = Thread::Dispatch(CHUDNOVSKY_FUNC(cs.chudnovskyArgs[1], cs.chudnovskyArgs[2]));
                    newton     = Thread::Dispatch(NEWTON_FUNC(cs.newtonArgs[1], cs.newtonArgs[2]));
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