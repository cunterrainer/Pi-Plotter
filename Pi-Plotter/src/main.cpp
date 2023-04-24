#include <thread>
#include <atomic>
#include <limits>
#include <cstdint>

#include "mpfr.h"

#include "Pi.h"
#include "Profiler.h"
#include "RenderWindow.h"

inline std::atomic_bool StopThreads = false;
template <typename Func>
inline void Calculate(RenderWindow* window, Func func, uint8_t identifier)
{
    uint32_t i = 1;
    uint32_t start = 0;
    while (i <= 1000 && !StopThreads)
    {
        start += Pi::Matches(func(i), start);
        window->Add(i, start, identifier);
        ++i;
    }
}

int main()
{
    RenderWindow window;
    std::thread archimedes(Calculate<decltype(Pi::Archimedes)>, &window, Pi::Archimedes, 0);
    std::thread chudnovsky(Calculate<decltype(Pi::Chudnovsky)>, &window, Pi::Chudnovsky, 1);
    std::thread newton(Calculate<decltype(Pi::Newton)>, &window, Pi::Newton, 2);

    while (window.IsOpen())
    {
        window.Show();
        window.EndFrame();
    }

    StopThreads = true;
    archimedes.join();
    chudnovsky.join();
    newton.join();
    mpfr_free_cache();
    return 0;
}