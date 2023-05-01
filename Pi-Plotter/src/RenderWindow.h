#pragma once
#include <array>
#include <utility>
#include <functional>

#include "GLFW/glfw3.h"
#include "ImGui/imgui.h"

#include "Plot.h"

class RenderWindow
{
public:
	enum PlotID { Archimedes, Chudnovsky, Newton };
private:
	static constexpr float SettingsHeight = 43.f;
private:
	GLFWwindow* m_Window = nullptr;
	std::array<Plot<uint32_t>, 3> m_Plots =
	{
		Plot<uint32_t>{"Archimedes"},
		Plot<uint32_t>{"Chudnovsky"},
		Plot<uint32_t>{"Newton-Raphson"}
	};
	int m_SelectedAlgorithm = 0;
private:
	inline void Swap()       const noexcept { glfwSwapBuffers(m_Window);    }
	inline void Clear()      const noexcept { glClear(GL_COLOR_BUFFER_BIT); }
	inline void PollEvents() const noexcept { glfwPollEvents();             }
	inline void WaitEvents() const noexcept { glfwWaitEvents();             }

	void RenderPlot() noexcept;
	bool ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
	void ImGuiSetTheme() const noexcept;
public:
	RenderWindow(int width = 1600, int height = 920, const char* title = "Pi Plotter", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) noexcept;
	~RenderWindow() noexcept;

	// loop
	inline bool IsOpen()     const noexcept { return !glfwWindowShouldClose(m_Window);      }
	inline void EndFrame()   const noexcept { Clear(); ImGuiRender(); PollEvents(); Swap(); }
	inline ImVec2 Size() const noexcept     { const ImGuiIO& io = ImGui::GetIO(); return { io.DisplaySize.x, io.DisplaySize.y }; }
	bool Show(bool started) noexcept;

	inline void ResetPlots()
	{
		m_Plots[PlotID::Archimedes].Reset();
		m_Plots[PlotID::Chudnovsky].Reset();
		m_Plots[PlotID::Newton].Reset();
	}

	inline void Add(uint32_t x, uint32_t y, PlotID identifier)
	{
		m_Plots[identifier].Add(x, y);
	}
};

int MsgBoxError(const char* message);