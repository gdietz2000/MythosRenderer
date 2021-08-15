#include "Mythos.h"

#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")

#include "MythosBuffer.h"
#include "MythosVertexShader.h"

unsigned int Mythos::Mythos::m_NextID = 0;

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

		for (int i = 0; i < m_NextID; ++i) {

			auto iter = m_Resources.find(i);
			if (iter != m_Resources.end()) {
				iter->second->SafeRelease();
				delete iter->second;
				m_Resources.erase(i);
			}
		}
	}

	BOOL Mythos::CreateVertexBuffer(void* data, unsigned int byteSize)
	{
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = byteSize;
		vertexDesc.CPUAccessFlags = 0;
		vertexDesc.MiscFlags = 0;
		vertexDesc.StructureByteStride = 0;
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (data != nullptr)
			subData.pSysMem = data;
		HRESULT hr;
		IMythosResource* vertexBuffer = new MythosBuffer(m_NextID, data, byteSize, &m_Context);
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, &subData, (ID3D11Buffer**)&vertexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, NULL, (ID3D11Buffer**)&vertexBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_Resources.insert(std::make_pair(m_NextID++, vertexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateIndexBuffer(void* data, unsigned int byteSize)
	{
		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = byteSize;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		indexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (data != nullptr)
			subData.pSysMem = data;

		HRESULT hr;
		IMythosResource* indexBuffer = new MythosBuffer(m_NextID, data, byteSize, &m_Context);
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, &subData, (ID3D11Buffer**)&indexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, NULL, (ID3D11Buffer**)&indexBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_Resources.insert(std::make_pair(m_NextID++, &(*indexBuffer)));

		return TRUE;
	}

	BOOL Mythos::CreateConstantBuffer(void* data, unsigned int byteSize)
	{
		D3D11_BUFFER_DESC constantDesc;
		ZeroMemory(&constantDesc, sizeof(constantDesc));
		constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantDesc.ByteWidth = byteSize;
		constantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantDesc.MiscFlags = 0;
		constantDesc.StructureByteStride = 0;
		constantDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (data != nullptr)
			subData.pSysMem = data;

		HRESULT hr;
		IMythosResource* constantBuffer = new MythosBuffer(m_NextID, data, byteSize, &m_Context);
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, &subData, (ID3D11Buffer**)&constantBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, NULL, (ID3D11Buffer**)&constantBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_Resources.insert(std::make_pair(m_NextID++, constantBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateVertexShader(const wchar_t* filePath, const char* entryPoint, const char* modelType, ID3D10Blob*& vertexBlob)
	{
		ID3D10Blob* errorBlob;

		HRESULT hr = D3DCompileFromFile(filePath, NULL, NULL, entryPoint, modelType, NULL, NULL, &vertexBlob, &errorBlob);
		if (FAILED(hr))
			return FALSE;

		IMythosResource* vertexShader = new MythosVertexShader(m_NextID, filePath, entryPoint, modelType);
		hr = m_Creator.GetCreator()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, (ID3D11VertexShader**)&vertexShader->GetData());
		if (FAILED(hr)) {
			delete vertexShader;
			return FALSE;
		}

		m_Resources.insert(std::make_pair(m_NextID++, vertexShader));

		return TRUE;
	}

	BOOL Mythos::UpdateMythosResource(unsigned int id, void* data, unsigned int byteSize)
	{
		IMythosResource* resource = m_Resources.find(id)->second;

		if (!resource)
			return FALSE;

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		m_Context.GetContext()->Map((ID3D11Buffer*)resource->GetData(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, data, byteSize);
		m_Context.GetContext()->Unmap((ID3D11Buffer*)resource->GetData(), 0);

		return TRUE;
	}

	IMythosResource* Mythos::GetResource(unsigned int id)
	{
		auto iter = m_Resources.find(id);
		if (iter != m_Resources.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}