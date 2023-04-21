#include <thread>
#include <atomic>
#include <limits>
#include <cstdint>
#include <iostream>

#include "Pi.h"
#include "Clang.h"
#include "RenderWindow.h"
#include "gmp.h"
#include "mpfr.h"

inline std::atomic_bool StopThreads = false;
template <typename Func>
inline void Calculate(RenderWindow* window, Func func, uint8_t identifier)
{
    uint32_t i = 1;
    while (i < std::numeric_limits<Pi::Float>::digits10 && !StopThreads)
    {
        window->Add(i, Pi::Measure(i, func), identifier);
        ++i;
    }
}


int main()
{
    mpfr_t t;
    mpfr_init2(t, 200);
    mpfr_set_ui(t, 10, MPFR_RNDD);
    mpfr_sin(t, t, MPFR_RNDD);
    mpfr_out_str(stdout, 10, 0, t, MPFR_RNDD);
    mpfr_clear(t);
    mpfr_free_cache();

    //std::cout << boost::math::sinc(3.14159265358979323846) << std::endl;
    std::cout << std::sin(3.14159265358979323846) << std::endl;
    std::cin.get();
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
    return 0;
}