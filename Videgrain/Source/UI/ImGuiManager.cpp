#include "ImGuiManager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

ImGuiManager::ImGuiManager(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.ScaleAllSizes(main_scale);
    // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    style.FontScaleDpi = main_scale;
    // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleFonts = true;
    // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    io.ConfigDpiScaleViewports = true;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);
}

ImGuiManager::~ImGuiManager()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiManager::DrawLayout()
{
    ImGui::DockSpaceOverViewport();

    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Demo Window", nullptr, &m_showDemo);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if(m_showDemo)
    {
        ImGui::ShowDemoWindow(&m_showDemo);
    }

    ImGui::Begin("Details");
    ImGui::Text("Transform");
    ImGui::Separator();
    static float pos[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::DragFloat3("Position", pos);
    ImGui::End();

    ImGui::Begin("Content Drawer");
    ImGui::Text("Assets/Models/Textures...");
    ImGui::End();

    ImGui::Begin("Console");
    ImGui::Text("[Log] Engine Started...");
    ImGui::End();

    // viewport, handle 3d scene
    ImGui::Begin("Viewport");
    ImGui::Text("Video Preview");
    ImGui::End();
}
