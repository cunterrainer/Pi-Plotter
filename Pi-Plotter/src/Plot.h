#pragma once
#include <limits>
#include <mutex>
#include <string>
#include <cstdlib>
#include <type_traits>
#include <string_view>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    static constexpr double YPercentageScalar = 0.05;
    static inline const char* XLabel = "Iterationen";
    static inline const char* YLabel = "Nachkommastellen";
private:
    std::vector<T> m_Xs;
    std::vector<T> m_Ys;
    std::mutex m_Mutex;
    const char* m_Name = nullptr;
    double m_YMax      = std::numeric_limits<double>::lowest();
    double m_YMin      = std::numeric_limits<double>::max();
    double m_GreatestY = std::numeric_limits<double>::lowest();
    double m_LowestY   = std::numeric_limits<double>::max();
private:
    inline void CalculateYRange()
    {
        double yOffset = -std::max(std::abs(m_GreatestY * YPercentageScalar), std::abs(m_LowestY * YPercentageScalar));
        if (yOffset == 0.0) yOffset = -1.0;
        m_YMax = m_GreatestY - yOffset;
        m_YMin = m_LowestY + yOffset;
    }
public:
    inline explicit Plot(const char* name) : m_Name(name) {}

    inline void Reset() noexcept
    {
        m_Xs.clear();
        m_Ys.clear();
    }

    inline void Add(T x, T y)
    {
        std::lock_guard lock(m_Mutex);
        m_Xs.push_back(x);
        m_Ys.push_back(y);
        m_GreatestY = std::max(m_GreatestY, static_cast<double>(y));
        m_LowestY = std::min(m_LowestY, static_cast<double>(y));
        CalculateYRange();
    }


    inline void Render(ImVec2 size, float xOffset, float yOffset)
    {
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos({ xOffset, yOffset });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin(m_Name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        if (ImPlot::BeginPlot(m_Name, {-1, -1}, ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxes(XLabel, YLabel, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);
            std::lock_guard lock(m_Mutex);
            ImPlot::SetupAxisLimits(ImAxis_Y1, m_YMin, m_YMax, ImPlotCond_Always);
            ImPlot::PlotLine("Pi", m_Xs.data(), m_Ys.data(), (int)m_Xs.size());
            ImPlot::EndPlot();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
};