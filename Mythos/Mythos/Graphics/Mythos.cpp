#include "Mythos.h"

#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")

#include <vector>

#include "Vector4.h"

#include "MythosBuffer.h"
#include "MythosVertexShader.h"
#include "MythosPixelShader.h"
#include "MythosTexture2D.h"
#include "MythosShaderResource.h"
#include "MythosRenderTarget.h"
#include "MythosDepthBuffer.h"
#include "MythosTextureSampler.h"
#include "MythosRasterizer.h"
#include "MythosInputLayout.h"
#include "DDSTextureLoader.h"

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

		for (int i = 0; i < MYTHOS_RESOURCE_COUNT; ++i)
		{
			m_Resources.push_back(std::unordered_map<const char*, IMythosResource*>());
		}

		m_RenderTargetClearColor = Math::Vector4(0);
		m_DepthBufferClearValue = 1.0f;
	}

	Mythos::~Mythos()
	{
		m_Creator.SafeRelease();
		m_Context.SafeRelease();
		m_SwapChain.SafeRelease();

		for (int i = 0; i < MYTHOS_RESOURCE_COUNT; ++i)
		{
			for (auto iter : m_Resources[i])
			{
				//Ensuring Memory is Deallocated Properly
				iter.second->SafeRelease();
				delete iter.second;
			}
		}

		for (auto blob : m_ShaderBlobs)
		{
			blob.second->Release();
		}

		//Clearing these containers
		m_NamesToIndex.clear();
		m_Resources.clear();
		m_ShaderBlobs.clear();
	}

	BOOL Mythos::CreateDefaultVertexBuffer(void* data, unsigned int byteSize, const char* name)
	{
		if (!NameAvailable(name))
			return FALSE;

		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = byteSize;
		vertexDesc.CPUAccessFlags = 0;
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (data != nullptr)
			subData.pSysMem = data;
		HRESULT hr;
		IMythosResource* vertexBuffer = new MythosBuffer();
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, &subData, (ID3D11Buffer**)&vertexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, NULL, (ID3D11Buffer**)&vertexBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_NamesToIndex.insert(std::make_pair(name, MYTHOS_RESOURCE_VERTEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_VERTEX_BUFFER].insert(std::make_pair(name, vertexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultIndexBuffer(void* data, unsigned int byteSize, const char* name)
	{
		if (!NameAvailable(name))
			return FALSE;

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
		IMythosResource* indexBuffer = new MythosBuffer();
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, &subData, (ID3D11Buffer**)&indexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, NULL, (ID3D11Buffer**)&indexBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_NamesToIndex.insert(std::make_pair(name, MYTHOS_RESOURCE_INDEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_INDEX_BUFFER].insert(std::make_pair(name, indexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultConstantBuffer(void* data, unsigned int byteSize, const char* name)
	{
		if (!NameAvailable(name))
			return FALSE;

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
		IMythosResource* constantBuffer = new MythosBuffer();
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, &subData, (ID3D11Buffer**)&constantBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, NULL, (ID3D11Buffer**)&constantBuffer->GetData());

		if (FAILED(hr))
			return FALSE;

		m_NamesToIndex.insert(std::make_pair(name, MYTHOS_RESOURCE_CONSTANT_BUFFER));
		m_Resources[MYTHOS_RESOURCE_CONSTANT_BUFFER].insert(std::make_pair(name, constantBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultDepthBuffer(const char* depthTextureName, const char* depthBufferName)
	{
		if (!NameAvailable(depthTextureName) || !NameAvailable(depthBufferName))
			return FALSE;

		D3D11_TEXTURE2D_DESC zBufferDesc;
		ZeroMemory(&zBufferDesc, sizeof(zBufferDesc));
		zBufferDesc.ArraySize = 1;
		zBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		zBufferDesc.Width = m_Viewport.Width;
		zBufferDesc.Height = m_Viewport.Height;
		zBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		zBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		zBufferDesc.SampleDesc.Count= 1;
		zBufferDesc.MipLevels = 1;

		IMythosResource* depthTexture = new MythosTexture2D();
		IMythosResource* depthBuffer = new MythosDepthBuffer();
		HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&zBufferDesc, nullptr, (ID3D11Texture2D**)&depthTexture->GetData());

		if (FAILED(hr)) {
			delete depthTexture;
			delete depthBuffer;
			return FALSE;
		}

		hr = m_Creator.GetCreator()->CreateDepthStencilView((ID3D11Resource*)depthTexture->GetData(), nullptr, (ID3D11DepthStencilView**)&depthBuffer->GetData());

		if (FAILED(hr))
		{
			delete depthTexture;
			delete depthBuffer;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(depthTextureName, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(depthTextureName, depthTexture));

		m_NamesToIndex.insert(std::make_pair(depthBufferName, MYTHOS_RESOURCE_DEPTH_BUFFER));
		m_Resources[MYTHOS_RESOURCE_DEPTH_BUFFER].insert(std::make_pair(depthBufferName, depthBuffer));

		return TRUE;
	}

	void Mythos::SetClearDepthBufferValue(float value)
	{
		m_DepthBufferClearValue = value;
	}

	void Mythos::ClearDepthBuffer(const char* depthBufferName)
	{
		IMythosResource* depthBuffer = GetResource(depthBufferName);
		if (depthBuffer)
			m_Context.GetContext()->ClearDepthStencilView((ID3D11DepthStencilView*)depthBuffer->GetData(), D3D11_CLEAR_DEPTH, m_DepthBufferClearValue, NULL);
	}

	BOOL Mythos::CreateVertexShader(const wchar_t* filePath, const char* entryPoint, const char* modelType, const char* name)
	{
		if (!NameAvailable(name))
			return FALSE;

		ID3D10Blob* errorBlob;
		ID3D10Blob* vertexBlob;

		HRESULT hr = D3DCompileFromFile(filePath, NULL, NULL, entryPoint, modelType, NULL, NULL, &vertexBlob, &errorBlob);
		if (FAILED(hr))
			return FALSE;

		IMythosResource* vertexShader = new MythosVertexShader();
		hr = m_Creator.GetCreator()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, (ID3D11VertexShader**)&vertexShader->GetData());
		if (FAILED(hr)) {
			delete vertexShader;
			return FALSE;
		}


		m_NamesToIndex.insert(std::make_pair(name, MYTHOS_RESOURCE_VERTEX_SHADER));
		m_Resources[MYTHOS_RESOURCE_VERTEX_SHADER].insert(std::make_pair(name, vertexShader));
		m_ShaderBlobs.insert(std::make_pair(name, vertexBlob));

		return TRUE;
	}

	BOOL Mythos::CreatePixelShader(const wchar_t* filePath, const char* entryPoint, const char* modelType, const char* name)
	{
		if (!NameAvailable(name))
			return FALSE;

		ID3D10Blob* errorBlob;
		ID3D10Blob* pixelBlob;
		HRESULT hr = D3DCompileFromFile(filePath, NULL, NULL, entryPoint, modelType, NULL, NULL, &pixelBlob, &errorBlob);
		if (FAILED(hr))
			return FALSE;

		IMythosResource* pixelShader = new MythosPixelShader();
		hr = m_Creator.GetCreator()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, (ID3D11PixelShader**)&pixelShader->GetData());
		if (FAILED(hr)) {
			delete pixelShader;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(name, MYTHOS_RESOURCE_PIXEL_SHADER));
		m_Resources[MYTHOS_RESOURCE_PIXEL_SHADER].insert(std::make_pair(name, pixelShader));
		m_ShaderBlobs.insert(std::make_pair(name, pixelBlob));

		return TRUE;
	}

	BOOL Mythos::CreateInputLayout(const MythosInputElement* elements, unsigned int numElements, const char* vertexShaderName, const char* inputLayoutName)
	{
		if (!NameAvailable(inputLayoutName))
			return FALSE;

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		int totalBytes = 0;
		for (int i = 0; i < numElements; ++i)
		{
			D3D11_INPUT_ELEMENT_DESC temp;
			int byteOffset = 0;
			temp.SemanticName = elements[i].m_SemanticName;
			temp.SemanticIndex = elements[i].m_SemanticIndex;
			switch (elements[i].m_Format)
			{
			case MythosFormat::MYTHOS_FORMAT_32_FLOAT1: { temp.Format = DXGI_FORMAT_R32_FLOAT; byteOffset = 4; break; }
			case MythosFormat::MYTHOS_FORMAT_32_FLOAT2: { temp.Format = DXGI_FORMAT_R32G32_FLOAT; byteOffset = 8; break; }
			case MythosFormat::MYTHOS_FORMAT_32_FLOAT3: { temp.Format = DXGI_FORMAT_R32G32B32_FLOAT; byteOffset = 12; break; }
			case MythosFormat::MYTHOS_FORMAT_32_FLOAT4: { temp.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; byteOffset = 16; break; }
			}
			temp.InputSlot = 0;
			temp.AlignedByteOffset = totalBytes;
			temp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			temp.InstanceDataStepRate = 0;
			totalBytes += byteOffset;
			inputElements.push_back(temp);
		}

		auto blob = GetShaderBlob(vertexShaderName);
		IMythosResource* inputLayout = new MythosInputLayout();

		HRESULT hr = m_Creator.GetCreator()->CreateInputLayout(inputElements.data(), numElements, blob->GetBufferPointer(), blob->GetBufferSize(), (ID3D11InputLayout**)&inputLayout->GetData());
		if (FAILED(hr)) {
			delete inputLayout;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(inputLayoutName, MYTHOS_RESOURCE_INPUT_LAYOUT));
		m_Resources[MYTHOS_RESOURCE_INPUT_LAYOUT].insert(std::make_pair(inputLayoutName, inputLayout));

		return TRUE;
	}

	BOOL Mythos::CreateMainRenderTarget(const char* renderTargetName)
	{
		if (!NameAvailable(renderTargetName))
			return FALSE;

		ID3D11Resource* backbuffer;
		HRESULT hr = m_SwapChain.GetSwapChain()->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);

		if (FAILED(hr))
			return FALSE;

		IMythosResource* renderTarget = new MythosRenderTarget();
		hr = m_Creator.GetCreator()->CreateRenderTargetView(backbuffer, nullptr, (ID3D11RenderTargetView**)&renderTarget->GetData());
		
		backbuffer->Release();

		if (FAILED(hr)) 
		{
			delete renderTarget;
			return FALSE;
		}
		
		m_NamesToIndex.insert(std::make_pair(renderTargetName, MYTHOS_RESOURCE_RENDER_TARGET));
		m_Resources[MYTHOS_RESOURCE_RENDER_TARGET].insert(std::make_pair(renderTargetName, renderTarget));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultRenderTarget(unsigned int width, unsigned int height, const char* textureName, const char* renderTargetName)
	{
		if (!NameAvailable(textureName) || !NameAvailable(renderTargetName))
			return FALSE;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
		ZeroMemory(&renderDesc, sizeof(renderDesc));

		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.Height = height;
		textureDesc.Width = width;
		textureDesc.MipLevels = 1;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;

		IMythosResource* texture = new MythosTexture2D();
		IMythosResource* renderTarget = new MythosRenderTarget();
		HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&texture->GetData());

		if (FAILED(hr)) {
			delete texture;
			delete renderTarget;
			return FALSE;
		}

		renderDesc.Format = textureDesc.Format;
		renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		hr = m_Creator.GetCreator()->CreateRenderTargetView((ID3D11Resource*)texture->GetData(), &renderDesc, (ID3D11RenderTargetView**)&renderTarget->GetData());

		if (FAILED(hr))
		{
			delete texture;
			delete renderTarget;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(textureName, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureName, texture));

		m_NamesToIndex.insert(std::make_pair(renderTargetName, MYTHOS_RESOURCE_RENDER_TARGET));
		m_Resources[MYTHOS_RESOURCE_RENDER_TARGET].insert(std::make_pair(renderTargetName, renderTarget));

		return TRUE;
	}

	void Mythos::SetClearRenderTargetColor(Math::Vector4 clearColor)
	{
		m_RenderTargetClearColor = clearColor;
	}

	void Mythos::ClearRenderTarget(const char* renderTargetName)
	{
		IMythosResource* renderTarget = GetResource(renderTargetName);
		if (renderTarget != nullptr)
			m_Context.GetContext()->ClearRenderTargetView((ID3D11RenderTargetView*)renderTarget->GetData(), m_RenderTargetClearColor.comp);
	}

	BOOL Mythos::CreateDefaultTexture2D(const wchar_t* filepath, const char* textureName)
	{
		if (!NameAvailable(textureName))
			return FALSE;

		IMythosResource* texture2D = new MythosTexture2D();
		HRESULT hr = DirectX::CreateDDSTextureFromFile(m_Creator.GetCreator(), filepath, (ID3D11Resource**)&texture2D->GetData(), nullptr);
		if (FAILED(hr))
		{
			delete texture2D;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(textureName, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureName, texture2D));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultShaderResource(const char* textureToBecomeResourceName, const char* shaderResourceName)
	{
		if (!NameAvailable(shaderResourceName))
			return FALSE;

		D3D11_TEXTURE2D_DESC textureDesc;

		IMythosResource* texture = GetResource(textureToBecomeResourceName);
		((ID3D11Texture2D*)texture->GetData())->GetDesc(&textureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
		ZeroMemory(&shaderDesc, sizeof(shaderDesc));
		shaderDesc.Format = textureDesc.Format;
		shaderDesc.Texture2D.MostDetailedMip = 0;
		shaderDesc.Texture2D.MipLevels = 1;
		shaderDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;

		IMythosResource* shaderResource = new MythosShaderResource();
		HRESULT hr = m_Creator.GetCreator()->CreateShaderResourceView((ID3D11Texture2D*)texture->GetData(), &shaderDesc, (ID3D11ShaderResourceView**)&shaderResource->GetData());

		if (FAILED(hr)) {
			delete shaderResource;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(shaderResourceName, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(shaderResourceName, shaderResource));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultTextureSampler(const char* samplerName)
	{
		if (!NameAvailable(samplerName))
			return FALSE;

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1;
		samplerDesc.BorderColor[1] = 1;
		samplerDesc.BorderColor[2] = 1;
		samplerDesc.BorderColor[3] = 1;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MaxLOD = 1;
		samplerDesc.MinLOD = 0;
		samplerDesc.MipLODBias = 0;

		IMythosResource* samplerState = new MythosTextureSampler();
		HRESULT hr = m_Creator.GetCreator()->CreateSamplerState(&samplerDesc, (ID3D11SamplerState**)&samplerState->GetData());
		if (FAILED(hr)) {
			delete samplerState;
			return FALSE;
		}

		m_NamesToIndex.insert(std::make_pair(samplerName, MYTHOS_RESOURCE_TEXTURE_SAMPLER));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_SAMPLER].insert(std::make_pair(samplerName, samplerState));

		return TRUE;
	}

	BOOL Mythos::CreateDefaultRasterizerState(const char* rasterizerStateName)
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;

		IMythosResource* rasterState = new MythosRasterizer();
		HRESULT hr = m_Creator.GetCreator()->CreateRasterizerState(&rasterDesc, (ID3D11RasterizerState**)&rasterState->GetData());

		if (FAILED(hr))
		{
			delete rasterState;
			return FALSE;
		}


		m_NamesToIndex.insert(std::make_pair(rasterizerStateName, MYTHOS_RESOURCE_RASTERIZER));
		m_Resources[MYTHOS_RESOURCE_RASTERIZER].insert(std::make_pair(rasterizerStateName, rasterState));

		return TRUE;
	}

	BOOL Mythos::UpdateMythosResource(const char* name, void* data, unsigned int byteSize)
	{
		IMythosResource* resource = m_Resources[m_NamesToIndex.find(name)->second].find(name)->second;

		if (!resource)
			return FALSE;

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		m_Context.GetContext()->Map((ID3D11Buffer*)resource->GetData(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, data, byteSize);
		m_Context.GetContext()->Unmap((ID3D11Buffer*)resource->GetData(), 0);

		return TRUE;
	}

	IMythosResource* Mythos::GetResource(const char* name)
	{
	
		auto num = m_NamesToIndex.find(name);
		if (num != m_NamesToIndex.end())
		{
			unsigned int arrayIndex = num->second;
			auto iter = m_Resources[arrayIndex].find(name);
			if (iter != m_Resources[arrayIndex].end())
			{
				return iter->second;
			}
		}

		return nullptr;
	}

	ID3D10Blob* Mythos::GetShaderBlob(const char* name)
	{
		auto iter = m_ShaderBlobs.find(name);
		if (iter != m_ShaderBlobs.end())
			return iter->second;


		return nullptr;
	}

	BOOL Mythos::NameAvailable(const char* name)
	{
		auto iter = m_NamesToIndex.find(name);
		if (iter == m_NamesToIndex.end())
			return TRUE;

		return FALSE;
	}
}