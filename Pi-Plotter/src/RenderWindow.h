#pragma once
#include <array>
#include <utility>
#include <functional>

#include "GLFW/glfw3.h"
#include "ImGui/imgui.h"

#include "Plot.h"

class RenderWindow
{
private:
	static inline constexpr float SettingHeight = 43.f;
private:
	GLFWwindow* m_Window = nullptr;
	Plot<uint32_t> m_Archimedes{"Archimedes"};
	Plot<uint32_t> m_Chudnovsky{"Chudnovsky"};
	Plot<uint32_t> m_Newton{"Newton-Raphson"};
private:
	inline void Swap()       const noexcept { glfwSwapBuffers(m_Window);    }
	inline void Clear()      const noexcept { glClear(GL_COLOR_BUFFER_BIT); }
	inline void PollEvents() const noexcept { glfwPollEvents();             }
	inline void WaitEvents() const noexcept { glfwWaitEvents();             }

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
	inline ImVec2 Size() const noexcept { const ImGuiIO& io = ImGui::GetIO(); return { io.DisplaySize.x, io.DisplaySize.y }; }
	bool Show() noexcept;

	inline void ResetPlots()
	{
		m_Archimedes.Reset();
		m_Chudnovsky.Reset();
		m_Newton.Reset();
	}

	inline void Add(uint32_t x, uint32_t y, uint8_t identifier)
	{
		switch (identifier)
		{
		case 0:
			m_Archimedes.Add(x, y); break;
		case 1:
			m_Chudnovsky.Add(x, y); break;
		default:
			m_Newton.Add(x, y); break;
		}
	}
};

int MsgBoxError(const char* message);