#include "Application.h"

Application::Application() : m_frameContext{},
                             m_pd3dSrvDescHeapAlloc{},
                             m_pd3dCommandQueue{},
                             m_mainRenderTargetResource{},
                             m_mainRenderTargetDescriptor{}
{
    m_frameIndex = 0;

    m_pd3dDevice = nullptr;
    m_pd3dRtvDescHeap = nullptr;
    m_pd3dSrvDescHeap = nullptr;
    m_pd3dCommandList = nullptr;
    m_fence = nullptr;
    m_fenceEvent = nullptr;
    m_fenceLastSignaledValue = 0;
    m_pSwapChain = nullptr;
    m_SwapChainOccluded = false;
    m_hSwapChainWaitableObject = nullptr;
}

Application::~Application()
{

}

void Application::Run()
{
    if(Init())
    {
        Loop();
    }
    Cleanup();
}

bool Application::Init()
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    m_wc = { sizeof(m_wc), CS_CLASSDC, StaticWndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Videgrain", nullptr };
    RegisterClassExW(&m_wc);
    m_hwnd = CreateWindowW(m_wc.lpszClassName, L"Videgrain", WS_OVERLAPPEDWINDOW, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, m_wc.hInstance, this);

    // Initialize Direct3D
    if(!CreateDeviceD3D(m_hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);

        return 1;
    }

    // Show Window
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    // Setup ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Style scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hwnd);

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = m_pd3dDevice;
    init_info.CommandQueue = m_pd3dCommandQueue;
    init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
    // SRV descriptors: callbacks
    init_info.SrvDescriptorHeap = m_pd3dSrvDescHeap;
    init_info.UserData = this;
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
        Application* app = static_cast<Application*>(info->UserData);

        app->m_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle);
    };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
        Application* app = static_cast<Application*>(info->UserData);

        app->m_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle);
    };
    ImGui_ImplDX12_Init(&init_info);

    // Load fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);
}

void Application::Loop()
{
    // Example state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main Loop
    bool done = false;
    while(!done)
    {
        // Poll, handle messages (inputs, window resice, etc...)
        // See WndProc() for dispatch event to Win32 backend
        MSG msg;
        while(PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if(msg.message == WM_QUIT)
            {
                done = true;
            }
        }
        if(done)
        {
            break;
        }

        // Handle window screen locked
        if((m_SwapChainOccluded && m_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) || IsIconic(m_hwnd))
        {
            Sleep(10);
            continue;
        }
        m_SwapChainOccluded = false;

        // Start ImGui Frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. show big demo window
        if(show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        // 2. show simple window
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo window", &show_demo_window);
            ImGui::Checkbox("Another window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            if(ImGui::Button("Button"))
            {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms / frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // 3. another window
        if(show_another_window)
        {
            ImGui::Begin("Another window", &show_another_window);

            ImGui::Text("Hello from another window!");
            if(ImGui::Button("Close me"))
            {
                show_another_window = false;
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
        m_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render ImGui Graphics
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        m_pd3dCommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
        m_pd3dCommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
        m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        m_pd3dCommandList->ResourceBarrier(1, &barrier);
        m_pd3dCommandList->Close();

        m_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_pd3dCommandList);

        // Present
        HRESULT hr = m_pSwapChain->Present(1, 0); // present w/ vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // present w/o vsync
        m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

        UINT64 fenceValue = m_fenceLastSignaledValue + 1;
        m_pd3dCommandQueue->Signal(m_fence, fenceValue);
        m_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    WaitForLastSubmittedFrame();
}

void Application::Cleanup()
{
    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(m_hwnd);
    UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
}


bool Application::CreateDeviceD3D(HWND hwnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = APP_NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = nullptr;
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
    {
        pdx12Debug->EnableDebugLayer();
    }
#endif

    // Create Device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if(D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_pd3dDevice)) != S_OK)
    {
        return false;
    }

    // [DEBUG] setup debug interface to break on any warning/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if(pdx12Debug != nullptr)
    {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        m_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = APP_NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if(m_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dRtvDescHeap)) != S_OK)
        {
            return false;
        }

        SIZE_T rtvDescriptorSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
        {
            m_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = APP_SRV_HEAP_SIZE;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if(m_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescHeap)) != S_OK)
        {
            return false;
        }

        m_pd3dSrvDescHeapAlloc.Create(m_pd3dDevice, m_pd3dSrvDescHeap);
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if(m_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pd3dCommandQueue)) != S_OK)
        {
            return false;
        }
    }

    for(UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
    {
        if(m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frameContext[i].CommandAllocator)) != S_OK)
        {
            return false;
        }
    }

    if(m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_pd3dCommandList)) != S_OK || m_pd3dCommandList->Close() != S_OK)
    {
        return false;
    }

    if(m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) != S_OK)
    {
        return false;
    }

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if(m_fenceEvent == nullptr)
    {
        return false;
    }

    {
        IDXGIFactory4* dxgiFactory = nullptr;
        IDXGISwapChain1* swapChain1 = nullptr;

        if(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
        {
            return false;
        }
        if(dxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, hwnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
        {
            return false;
        }
        if(swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)) != S_OK)
        {
            return false;
        }

        swapChain1->Release();
        dxgiFactory->Release();

        m_pSwapChain->SetMaximumFrameLatency(APP_NUM_BACK_BUFFERS);
        m_hSwapChainWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();

    return true;
}

void Application::CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if(m_pSwapChain)
    {
        m_pSwapChain->SetFullscreenState(false, nullptr);
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }
    if(m_hSwapChainWaitableObject)
    {
        CloseHandle(m_hSwapChainWaitableObject);
    }
    for(UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
    {
        if(m_frameContext[i].CommandAllocator)
        {
            m_frameContext[i].CommandAllocator->Release();
            m_frameContext[i].CommandAllocator = nullptr;
        }
    }
    if(m_pd3dCommandQueue)
    {
        m_pd3dCommandQueue->Release();
        m_pd3dCommandQueue = nullptr;
    }
    if(m_pd3dCommandList)
    {
        m_pd3dCommandList->Release();
        m_pd3dCommandList = nullptr;
    }
    if(m_pd3dRtvDescHeap)
    {
        m_pd3dRtvDescHeap->Release();
        m_pd3dRtvDescHeap = nullptr;
    }
    if(m_pd3dSrvDescHeap)
    {
        m_pd3dSrvDescHeap->Release();
        m_pd3dSrvDescHeap = nullptr;
    }
    if(m_fence)
    {
        m_fence->Release();
        m_fence = nullptr;
    }
    if(m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }
    if(m_pd3dDevice)
    {
        m_pd3dDevice->Release();
        m_pd3dDevice = nullptr;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = nullptr;
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void Application::CreateRenderTarget()
{
    for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = nullptr;
        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_mainRenderTargetDescriptor[i]);
        m_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void Application::CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
    {
        if(m_mainRenderTargetResource[i])
        {
            m_mainRenderTargetResource[i]->Release();
            m_mainRenderTargetResource[i] = nullptr;
        }
    }
}

void Application::WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &m_frameContext[m_frameIndex % APP_NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if(fenceValue == 0)
    {
        return;
    }

    frameCtx->FenceValue = 0;
    if(m_fence->GetCompletedValue() >= fenceValue)
    {
        return;
    }

    m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
    WaitForSingleObject(m_fenceEvent, INFINITE);
}

Application::FrameContext* Application::WaitForNextFrameResources()
{
    UINT nextFrameIndex = m_frameIndex + 1;
    m_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { m_hSwapChainWaitableObject, nullptr };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &m_frameContext[nextFrameIndex % APP_NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if(fenceValue != 0)
    {
        frameCtx->FenceValue = 0;
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        waitableObjects[1] = m_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// forward decl message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI Application::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    Application* pApp = nullptr;
    if(msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pApp = reinterpret_cast<Application*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
    }
    else
    {
        pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if(pApp)
    {
        return pApp->HandleMessage(hWnd, msg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LRESULT WINAPI Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_SIZE:
            if(m_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
            {
                WaitForLastSubmittedFrame();
                CleanupRenderTarget();

                HRESULT result = m_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
                assert(SUCCEEDED(result) && "Failed to resize swapchain");

                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if((wParam & 0xfff0) == SC_KEYMENU)
            {
                return 0;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}