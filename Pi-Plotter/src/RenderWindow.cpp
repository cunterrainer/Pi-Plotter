#ifdef WINDOWS
    #include <Windows.h>
    #undef max // windows macros
    #undef min // windows macros
#elif defined(LINUX)
    #include <locale>
    #include <sstream>
    #include "MsgBoxX11/MsgBoxX11.h"
#endif

#include "GLFW/glfw3.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_stdlib.h"
#include "ImPlot/implot.h"

#include "Log.h"
#include "Arial.h"
#include "pi-string.h"
#include "RenderWindow.h"


inline bool g_ShowFPS = false;
RenderWindow::RenderWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) noexcept
{
    glfwSetErrorCallback([](int error, const char* description){ Err << "[GLFW] " << error << ": " << description << Endl; });

    if (!glfwInit())
    {
        Err << "Failed to initialize GLFW" << Endl;
        return;
    }
    Log << "Initialized GLFW" << Endl;

    // Create window with graphics context
    m_Window = glfwCreateWindow(width, height, title, monitor, share);
    if (m_Window == NULL)
    {
        Err << "Failed to create window w: " << width << " h : " << height << " t : " << title << Endl;
        return;
    }
    Log << "Created window w: " << width << " h: " << height << " t: " << title << Endl;

    // center the window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_Window, (mode->width - width) / 2, (mode->height - height) / 2);

    glfwSetKeyCallback(m_Window, [](GLFWwindow*, int key, int, int action, int) { if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) g_ShowFPS = !g_ShowFPS; });
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);
    glClearColor(0.27f, 0.27f, 0.27f, 1.0f);
    if (!ImGuiInit()) return;
    ImGuiSetTheme();
    ImPlot::CreateContext();
    ImPlot::StyleColorsClassic();
    Log << "Initialized ImPlot" << Endl;
}


RenderWindow::~RenderWindow() noexcept
{
    // Cleanup
    ImPlot::DestroyContext();
    Log << "Shut down ImPlot" << Endl;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Log << "Shut down ImGui" << Endl;

    glfwDestroyWindow(m_Window);
    glfwTerminate();
    Log << "Shut down GLFW" << Endl;
}


bool RenderWindow::ImGuiInit(const char* iniFileName) const noexcept
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedTTF(sg_ArialCompressedData, sg_ArialCompressedSize, 19);
    io.IniFilename = iniFileName;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    if (!ImGui_ImplOpenGL3_Init("#version 130"))
    {
        Err << "Failed to initialize ImGui OpenGL3 implementation" << Endl;
        return false;
    }
    Log << "Initialized ImGui" << Endl;
    return true;
}


void RenderWindow::ImGuiStartFrame() const noexcept
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void RenderWindow::ImGuiRender() const noexcept
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void RenderWindow::ImGuiSetTheme() const noexcept
{
    /*
        Styles from this project:
        https://github.com/MohitSethi99/ArcGameEngine/blob/main/Arc-Editor/src/Utils/EditorTheme.h
    */
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.275f, 0.275f, 0.275f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.082f, 0.082f, 0.082f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.082f, 0.082f, 0.082f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.44f, 0.88f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.000f, 0.439f, 0.878f, 0.824f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.082f, 0.082f, 0.082f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.188f, 0.529f, 0.780f, 1.000f);


    ImGuiStyle* style = &ImGui::GetStyle();
    style->AntiAliasedFill = true;
    style->AntiAliasedLines = true;
    style->AntiAliasedLinesUseTex = true;

    style->Alpha = 1;
    style->TabMinWidthForCloseButton = 0.1f;
    style->CellPadding = ImVec2(8.0f, 4.0f);
    style->ItemSpacing = ImVec2(8.0f, 3.0f);
    style->ItemInnerSpacing = ImVec2(2.0f, 4.0f);
    style->TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style->IndentSpacing = 12;
    style->ScrollbarSize = 14;
    style->GrabMinSize = 10;

    style->WindowBorderSize = 0.0f;
    style->ChildBorderSize = 0.0f;
    style->PopupBorderSize = 1.5f;
    style->FrameBorderSize = 0.5f;
    style->TabBorderSize = 0.0f;

    style->WindowRounding = 0.0f;
    style->ChildRounding = 0.0f;
    style->FrameRounding = 2.0f;
    style->PopupRounding = 2.0f;
    style->ScrollbarRounding = 3.0f;
    style->GrabRounding = 2.0f;
    style->LogSliderDeadzone = 4.0f;
    style->TabRounding = 3.0f;

    style->WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style->SelectableTextAlign = ImVec2(0.0f, 0.0f);
    style->DisplaySafeAreaPadding = ImVec2(8.0f, 8.0f);
}


void RenderWindow::RenderText(const ImVec2& plotSize) noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImGui::SetNextWindowPos({ plotSize.x, SettingsHeight });
    ImGui::SetNextWindowSize(plotSize);
    ImGui::Begin("##PiOutput", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    const size_t digits = m_Plots[m_SelectedAlgorithm].Decimals();
    std::string pi = std::string(PiBillionStr, digits+2);

    size_t off = 0;
    for (size_t i = 0; i < pi.size(); ++i)
    {
        if (ImGui::CalcTextSize(pi.substr(off, i-off).c_str()).x > plotSize.x)
        {
            pi.insert(i-1, 1, '\n');
            off = i;
        }
    }

    ImGui::TextUnformatted(fmt::Format("[%zu] 3.%s", digits, pi.substr(0, pi.size()-2).c_str()).c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.f);
    ImGui::End();
    ImGui::PopStyleVar();
}


void RenderWindow::RenderPlot() noexcept
{
    const ImVec2 size = Size();
    ImVec2 plotSize{ size.x / 2.f, size.y - SettingsHeight };

    switch (m_SelectedAlgorithm)
    {
    case PlotID::Archimedes:
    case PlotID::Chudnovsky:
    case PlotID::Newton:
        m_Plots[m_SelectedAlgorithm].Render(plotSize, 0, SettingsHeight);
        RenderText(plotSize);
        return;
    default:
        plotSize.x = size.x / 3.f;
        m_Plots[PlotID::Archimedes].Render(plotSize, 0, SettingsHeight);
        m_Plots[PlotID::Chudnovsky].Render(plotSize, plotSize.x, SettingsHeight);
        m_Plots[PlotID::Newton].Render(plotSize, plotSize.x * 2.f, SettingsHeight);
    }
}


bool RenderWindow::Show(bool started) noexcept
{
    ImGuiStartFrame();
    if (ImGui::GetIO().DisplaySize.x <= 0 || ImGui::GetIO().DisplaySize.y <= 0) return false;

    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize({ Size().x, SettingsHeight });
    ImGui::Begin("##Setup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
   
    const bool ret = ImGui::Button(started ? "Stop" : "Start", {150.f, 0.f});
    ImGui::SameLine();
    ImGui::SetNextItemWidth(170);
    ImGui::Combo("##Algorithm", &m_SelectedAlgorithm, "Archimedes\0Chudnovsky\0Newton-Raphson\0Alle\0");
    if (g_ShowFPS)
    {
        ImGui::SameLine(Size().x - 250);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

    RenderPlot();
    return ret;
}


#ifdef WINDOWS
    int MsgBoxError(const char* message) { return MessageBoxA(NULL, message, "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL); }
#elif defined(LINUX)
    inline std::wstring WidenString(const char* str)
    {
        const size_t length = strlen(str);
        std::wstring result;
        result.resize(length);

        const auto& facet = std::use_facet<std::ctype<wchar_t>>(std::wostringstream().getloc());
        std::transform(str, str + length, result.begin(), [&facet](const char ch)
        {
            return facet.widen(ch);
        });
        return result;
    }

    int MsgBoxError(const char* message)
    {
        Button btn;
        btn.label = L"Ok";
        btn.result = 1;
        return Messagebox("Error", WidenString(message).c_str(), &btn, 1);
    }
#endif