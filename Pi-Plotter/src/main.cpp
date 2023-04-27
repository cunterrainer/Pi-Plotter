#include <thread>
#include <atomic>
#include <limits>
#include <cstdint>

#include "mpfr.h"

#include "Pi.h"
#include "RenderWindow.h"

inline std::atomic_bool StopThreads = false;
template <typename Func, uint32_t Iterations, uint32_t iterMod>
inline void Calculate(RenderWindow* window, Func func, uint8_t identifier)
{
    static_assert(Iterations % iterMod == 0, "Iterations & iterMod must satisfy: 'Iterations % iterMod == 0'");
    static_assert(Iterations != 0 && iterMod != 0, "Iterations & iterMod must satisfy: 'Iterations > 0 & iterMod > 0'");
    uint32_t start = 0;
    window->Add(0, 0, identifier);
    for (uint32_t i = 1; i <= Iterations && !StopThreads; ++i)
    {
        const auto value = func(i);
        if (i % iterMod == 0 || i == 1)
        {
            start += Pi::Matches(value, start);
            window->Add(i, start, identifier);
        }
    }
}

int main()
{
    RenderWindow window;
    std::thread archimedes(Calculate<decltype(Pi::Archimedes), 1000000000, 100>, &window, Pi::Archimedes, 0);
    std::thread chudnovsky(Calculate<decltype(Pi::Chudnovsky), 100000, 100>, &window, Pi::Chudnovsky, 1);
    std::thread newton(Calculate<decltype(Pi::Newton), 20, 1>, &window, Pi::Newton, 2);

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