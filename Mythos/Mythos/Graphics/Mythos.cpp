#include "Mythos.h"

namespace Mythos
{
	Mythos::Mythos(void* window)
	{
		HWND hWnd = *(HWND*)window;

		RECT rect;
		GetClientRect(hWnd, &rect);

		HRESULT hr;

		D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(scDesc));

		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.BufferDesc.Width = rect.right - rect.left;
		scDesc.BufferDesc.Height = rect.bottom - rect.top;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.OutputWindow = hWnd;
		scDesc.SampleDesc.Count = 1;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.Windowed = true;

		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
			&dx11, 1, D3D11_SDK_VERSION, &scDesc, &m_SwapChain.GetSwapChain(), &m_Creator.GetCreator(), nullptr, &m_Context.GetContext());

		if (FAILED(hr)) {
			throw;
		}

		m_Viewport.Width = (FLOAT)scDesc.BufferDesc.Width;
		m_Viewport.Height = (FLOAT)scDesc.BufferDesc.Height;
		m_Viewport.TopLeftX = m_Viewport.TopLeftY = 0;
		m_Viewport.MinDepth = 0; m_Viewport.MaxDepth = 1;

		ID3D11Resource* backbuffer;

		hr = m_SwapChain.GetSwapChain()->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);

		if (FAILED(hr))
			throw;

		hr = m_Creator.GetCreator()->CreateRenderTargetView(backbuffer, nullptr, &m_RTV);

		if (FAILED(hr))
			throw;

		backbuffer->Release();
	}

	Mythos::~Mythos()
	{
		m_Creator.SafeRelease();
		m_Context.SafeRelease();
		m_SwapChain.SafeRelease();
		if (m_RTV) m_RTV->Release();
	}
}