#pragma once

#include "Core/Timer.h"
#include "Renderer/DX11Renderer.h"
#include "Window/Win32Window.h"
#include "UI/ImGuiManager.h"

#include <memory>
#include <string>

class App
{
public:
	App(int width, int height, const std::wstring& title);
	~App();

	void Run();

private:
	void Update(float dt);
	void Render();

	std::unique_ptr<Win32Window> m_window;
	std::unique_ptr<DX11Renderer> m_renderer;
	std::unique_ptr<ImGuiManager> m_ui;

	Timer m_timer;
	float m_aspectRatio;
};

