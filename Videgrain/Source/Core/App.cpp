#include "App.h"

#include <tchar.h>
#include <iostream>
#include <stdexcept>

App::App(int width, int height, const std::wstring& title)
{
    m_window = std::make_unique<Win32Window>(width, height, title);
    m_renderer = std::make_unique<DX11Renderer>(m_window->GetHWND());
    m_ui = std::make_unique<ImGuiManager>(m_window->GetHWND(), m_renderer->GetDevice(), m_renderer->GetContext());

    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

App::~App() { }

void App::Run()
{
    // Main loop
    while(m_window->ProcessMessages())
    {
        if(m_window->WasResized())
        {
            m_renderer->Resize(m_window->GetWidth(), m_window->GetHeight());
            m_aspectRatio = static_cast<float>(m_window->GetWidth()) / static_cast<float>(m_window->GetHeight());
            m_window->ResetResizeFlag();
        }

        Render();
    }
}

void App::Update(float dt)
{
    float clearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
    m_renderer->BindMainWindowRenderTarget(clearColor);
}

void App::Render()
{
    m_renderer->BeginFrame();
    m_ui->BeginFrame();

    Update(m_timer.Tick());
    m_ui->DrawLayout();
    m_ui->EndFrame();
    m_renderer->EndFrame();
}
