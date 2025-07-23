#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>

#include <tchar.h>
#include <iostream>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

class Application
{
public:
    Application();
    ~Application();

    void Run();
private:
    bool Init();
    void Loop();
    void Cleanup();

    // Dx12 + ImGui Related
public:
    // config for example app
    static const int APP_NUM_FRAMES_IN_FLIGHT = 2;
    static const int APP_NUM_BACK_BUFFERS = 2;
    static const int APP_SRV_HEAP_SIZE = 64;

    static LRESULT WINAPI StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:
    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator;
        UINT64                  FenceValue;
    };

    struct ExampleDescriptorHeapAllocator
    {
        ID3D12DescriptorHeap* Heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
        UINT                        HeapHandleIncrement;
        ImVector<int>               FreeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
        {
            IM_ASSERT(Heap == nullptr && FreeIndices.empty());

            Heap = heap;
            D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
            HeapType = desc.Type;
            HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
            HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
            HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
            FreeIndices.reserve((int)desc.NumDescriptors);
            for(int n = desc.NumDescriptors; n > 0; n--)
            {
                FreeIndices.push_back(n - 1);
            }
        }

        void Destroy()
        {
            Heap = nullptr;
            FreeIndices.clear();
        }

        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
        {
            IM_ASSERT(FreeIndices.Size > 0);

            int idx = FreeIndices.back();
            FreeIndices.pop_back();
            out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
            out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
        }

        void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
        {
            int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
            int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);

            IM_ASSERT(cpu_idx == gpu_idx);

            FreeIndices.push_back(cpu_idx);
        }
    };

    bool CreateDeviceD3D(HWND hwnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();
    FrameContext* WaitForNextFrameResources();
    LRESULT WINAPI HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


    // Data
    FrameContext m_frameContext[APP_NUM_FRAMES_IN_FLIGHT];
    UINT m_frameIndex;

    ID3D12Device* m_pd3dDevice;
    ID3D12DescriptorHeap* m_pd3dRtvDescHeap;
    ID3D12DescriptorHeap* m_pd3dSrvDescHeap;
    ExampleDescriptorHeapAllocator m_pd3dSrvDescHeapAlloc;
    ID3D12CommandQueue* m_pd3dCommandQueue;
    ID3D12GraphicsCommandList* m_pd3dCommandList;
    ID3D12Fence* m_fence;
    HANDLE m_fenceEvent;
    UINT64 m_fenceLastSignaledValue;
    IDXGISwapChain3* m_pSwapChain;
    bool m_SwapChainOccluded;
    HANDLE m_hSwapChainWaitableObject;
    ID3D12Resource* m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
    D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS];

    WNDCLASSEX m_wc;
    HWND m_hwnd;
};