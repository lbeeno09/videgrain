#pragma once

#include "Video/VideoTypes.h"

#include <d3d11.h>
#include <wrl/client.h>

class DX11Renderer
{
public:
	DX11Renderer(HWND hwnd);
	~DX11Renderer();

	void BeginFrame();
	void EndFrame();
	void Resize(UINT width, UINT height);

	void BindSceneRenderTarget(const float clearColor[4]);
	void BindMainWindowRenderTarget(const float clearColor[4]);
	void UpdateVideoConstants(const VideoConstantBuffer& data);
	void UpdateVideoTexture(const uint8_t* pData, UINT width, UINT height);

	ID3D11Device* GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
	IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }
	ID3D11ShaderResourceView* GetSceneSRV() const { return m_sceneSRV.Get(); }

private:
	bool CreateDeviceD3D(HWND hWnd);
	void CreateRenderTarget();
	void CreateSceneResource(UINT width, UINT height);

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_mainRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_sceneTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_sceneRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_sceneSRV;

	bool m_swapChainOccluded = false;
};