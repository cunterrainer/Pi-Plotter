#include <cstdint>

#include "Pi.h"
#include "Clang.h"
#include "RenderWindow.h"

int main()
{
    RenderWindow window;
    uint32_t a = 1;
    uint32_t c = 1;
    uint32_t n = 1;

    while (window.IsOpen())
    {
        window.Show();
        if (a < 100)
        {
            window.AddArchimedes(a, Pi::Measure(a, Pi::Archimedes));
            window.AddChudnovsky(c, Pi::Measure(c, Pi::Chudnovsky));
            window.AddNewton(n, Pi::Measure(n, Pi::Newton)); // little bit too accurate
        }
        window.EndFrame();
        ++a;
        ++c;
        ++n;
    }
    return 0;
}