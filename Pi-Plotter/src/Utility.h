#pragma once
#include "RenderWindow.h"

namespace Utility
{
    template <typename T>
    constexpr decltype(auto) Last(T&& t)
    {
        return std::forward<T>(t);
    }

    constexpr const char* PlotIDString(RenderWindow::PlotID id)
    {
        switch (id)
        {
        case RenderWindow::Archimedes: return "Archimedes";
        case RenderWindow::Chudnovsky: return "Chudnovsky";
        case RenderWindow::Newton: return "Newton-Raphson";
        default: return "";
        }
    }
}