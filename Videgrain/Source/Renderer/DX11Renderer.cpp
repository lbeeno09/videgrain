#include "DX11Renderer.h"

#include <d3dcompiler.h>

#include <stdexcept>

DX11Renderer::DX11Renderer(HWND hwnd)
{
    if(!CreateDeviceD3D(hwnd))
    {
        throw std::exception("ERROR: Init DX11 Failed");
    }

    RECT rect;
    GetClientRect(hwnd, &rect);
    Resize(rect.right - rect.left, rect.bottom - rect.top);
}

DX11Renderer::~DX11Renderer() { }

void DX11Renderer::BeginFrame()
{
    m_context->ClearState();
}

void DX11Renderer::EndFrame()
{
    // Present
    HRESULT hr = m_swapChain->Present(1, 0);   // Present with vsync
    m_swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

void DX11Renderer::Resize(UINT width, UINT height)
{
    if(width == 0 || height == 0)
    {
        return;
    }

    // Handle window being minimized or screen locked
    if(m_swapChainOccluded && m_swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
    {
        ::Sleep(10);
    }
    m_swapChainOccluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_mainRTV.Reset();
    m_context->Flush();

    HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(hr))
    {
        return;
    }

    CreateRenderTarget();

    D3D11_VIEWPORT vp = { 0, 0, (float)width, (float)height, 0.0f, 1.0f };
    m_context->RSSetViewports(1, &vp);
}

void DX11Renderer::BindMainWindowRenderTarget(const float clearColor[4])
{
    m_context->OMSetRenderTargets(1, m_mainRTV.GetAddressOf(), nullptr);
    m_context->ClearRenderTargetView(m_mainRTV.Get(), clearColor);
}

bool DX11Renderer::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    // This is a basic setup. Optimally could use e.g. DXGI_SWAP_EFFECT_FLIP_DISCARD and handle fullscreen mode differently. See #8979 for suggestions.
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, 
        m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel, m_context.GetAddressOf()
    );
    if(res != S_OK)
    {
        return false;
    }

    // Disable DXGI's default Alt+Enter fullscreen behavior.
    // - You are free to leave this enabled, but it will not work properly with multiple viewports.
    // - This must be done for all windows associated to the device. Our DX11 backend does this automatically for secondary viewports that it creates.
    Microsoft::WRL::ComPtr<IDXGIFactory> pSwapChainFactory;
    if(SUCCEEDED(m_swapChain->GetParent(IID_PPV_ARGS(pSwapChainFactory.GetAddressOf()))))
    {
        pSwapChainFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    }

    CreateRenderTarget();
    return true;
}

void DX11Renderer::CreateRenderTarget()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;

    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    if(FAILED(hr))
    {
        return;
    }

    hr = m_device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_mainRTV.GetAddressOf());
    if(FAILED(hr))
    {
        return;
    }
}
