#include "Win32Window.h"

#include "imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Win32Window::Win32Window(int width, int height, const std::wstring& title) : m_className(L"VidegrainWindowClass")
{
	m_hInst = GetModuleHandle(nullptr);

    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, m_hInst, nullptr, nullptr, nullptr, nullptr, m_className.c_str(), nullptr};
    ::RegisterClassExW(&wc);

    m_hwnd = ::CreateWindowW(m_className.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, (int)(width * main_scale), (int)(height * main_scale), nullptr, nullptr, m_hInst, this);

    // Show the window
    ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_hwnd);
}

Win32Window::~Win32Window()
{
    ::DestroyWindow(m_hwnd);
    ::UnregisterClassW(m_className.c_str(), m_hInst);
}

bool Win32Window::ProcessMessages()
{
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32 backend.
    MSG msg;
    while(::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
        {
            return false;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return true;
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    Win32Window* pWindow = nullptr;
    if(msg == WM_NCCREATE)
    {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pWindow = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);

        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
    }
    else
    {
        pWindow = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    if(pWindow)
    {
        return pWindow->HandleProc(hWnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Win32Window::HandleProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_SIZE:
        if(wParam == SIZE_MINIMIZED)
        {
            return 0;
        }
        m_width = (UINT)LOWORD(lParam); // Queue resize
        m_height = (UINT)HIWORD(lParam);
        m_resizeRequested = true;
        return 0;
    case WM_SYSCOMMAND:
        // Disable ALT application menu
        if((wParam & 0xfff0) == SC_KEYMENU)
        {
            return 0;
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
