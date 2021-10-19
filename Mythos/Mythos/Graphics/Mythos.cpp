#include "Mythos.h"

#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")

#include <vector>
#include <fstream>

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

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

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

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
			m_Resources.push_back(std::unordered_map<MythosID, IMythosResource*>());
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

		for (auto light : m_Lights) 
		{
			delete light.second;
		}

		//Clearing these containers
		m_IDsToIndex.clear();
		m_Resources.clear();
		m_ShaderBlobs.clear();
		m_Lights.clear();
	}

	BOOL Mythos::CreateVertexBuffer(void* data, unsigned int byteSize, MythosID& id)
	{
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

		if (FAILED(hr)) {
			delete vertexBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_VERTEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_VERTEX_BUFFER].insert(std::make_pair(id, vertexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateVertexBuffer(MythosBufferDescriptor* desc, MythosID& id)
	{
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.ByteWidth = desc->byteSize;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			vertexDesc.CPUAccessFlags = 0;
			vertexDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			vertexDesc.CPUAccessFlags = 0;
			vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		vertexDesc.MiscFlags = 0;
		vertexDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (desc->data != nullptr)
			subData.pSysMem = desc->data;
		HRESULT hr;
		IMythosResource* vertexBuffer = new MythosBuffer();
		if (desc->data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, &subData, (ID3D11Buffer**)&vertexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&vertexDesc, NULL, (ID3D11Buffer**)&vertexBuffer->GetData());

		if (FAILED(hr)) {
			delete vertexBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_VERTEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_VERTEX_BUFFER].insert(std::make_pair(id, vertexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateIndexBuffer(void* data, unsigned int byteSize, MythosID& id)
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
		IMythosResource* indexBuffer = new MythosBuffer();
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, &subData, (ID3D11Buffer**)&indexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, NULL, (ID3D11Buffer**)&indexBuffer->GetData());

		if (FAILED(hr)) {
			delete indexBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_INDEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_INDEX_BUFFER].insert(std::make_pair(id, indexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateIndexBuffer(MythosBufferDescriptor* desc, MythosID& id)
	{
		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.ByteWidth = desc->byteSize;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			indexDesc.CPUAccessFlags = 0;
			indexDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			indexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			indexDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			indexDesc.CPUAccessFlags = 0;
			indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (desc->data != nullptr)
			subData.pSysMem = desc->data;
		HRESULT hr;
		IMythosResource* indexBuffer = new MythosBuffer();
		if (desc->data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, &subData, (ID3D11Buffer**)&indexBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&indexDesc, NULL, (ID3D11Buffer**)&indexBuffer->GetData());

		if (FAILED(hr)) {
			delete indexBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_INDEX_BUFFER));
		m_Resources[MYTHOS_RESOURCE_INDEX_BUFFER].insert(std::make_pair(id, indexBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateModelBuffers(MythosModel* object, MythosID& vertexID, MythosID& indexID)
	{
		std::vector<MythosVertex> vertices;
		std::vector<int> indices;

		for (int i = 0; i < object->m_Meshes.size(); ++i)
		{
			int vertCount = vertices.size();

			vertices.insert(vertices.end(), object->m_Meshes[i]->m_Vertices.begin(), object->m_Meshes[i]->m_Vertices.end());

			for (int j = 0; j < object->m_Meshes[i]->m_Indices.size(); ++j) {
				indices.push_back(object->m_Meshes[i]->m_Indices[j] + vertCount);
			}
		}

		MythosBufferDescriptor vertexDesc;
		vertexDesc.data = vertices.data();
		vertexDesc.byteSize = sizeof(MythosVertex) * vertices.size();
		vertexDesc.cpuAccess = MythosAccessability::MYTHOS_DEFAULT_ACCESS;
		BOOL success = CreateVertexBuffer(&vertexDesc, vertexID);
		if (!success)
			return FALSE;

		MythosBufferDescriptor indexDesc;
		indexDesc.data = indices.data();
		indexDesc.byteSize = sizeof(int) * indices.size();
		indexDesc.cpuAccess = MythosAccessability::MYTHOS_DEFAULT_ACCESS;
		success = CreateIndexBuffer(&indexDesc, indexID);
		if (!success)
			return FALSE;

		return TRUE;
	}

	BOOL Mythos::CreateConstantBuffer(void* data, unsigned int byteSize, MythosID& id)
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
		IMythosResource* constantBuffer = new MythosBuffer();
		if (data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, &subData, (ID3D11Buffer**)&constantBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, NULL, (ID3D11Buffer**)&constantBuffer->GetData());

		if (FAILED(hr)) {
			delete constantBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_CONSTANT_BUFFER));
		m_Resources[MYTHOS_RESOURCE_CONSTANT_BUFFER].insert(std::make_pair(id, constantBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateConstantBuffer(MythosBufferDescriptor* desc, MythosID& id)
	{
		D3D11_BUFFER_DESC constantDesc;
		ZeroMemory(&constantDesc, sizeof(constantDesc));
		constantDesc.ByteWidth = desc->byteSize;
		constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			constantDesc.CPUAccessFlags = 0;
			constantDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			constantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			constantDesc.CPUAccessFlags = 0;
			constantDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		constantDesc.MiscFlags = 0;
		constantDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		if (desc->data != nullptr)
			subData.pSysMem = desc->data;
		HRESULT hr;
		IMythosResource* constantBuffer = new MythosBuffer();
		if (desc->data != nullptr)
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, &subData, (ID3D11Buffer**)&constantBuffer->GetData());
		else
			hr = m_Creator.GetCreator()->CreateBuffer(&constantDesc, NULL, (ID3D11Buffer**)&constantBuffer->GetData());

		if (FAILED(hr)) {
			delete constantBuffer;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_CONSTANT_BUFFER));
		m_Resources[MYTHOS_RESOURCE_CONSTANT_BUFFER].insert(std::make_pair(id, constantBuffer));

		return TRUE;
	}

	BOOL Mythos::CreateDepthBuffer(MythosID& textureID, MythosID& bufferID)
	{
		D3D11_TEXTURE2D_DESC zBufferDesc;
		ZeroMemory(&zBufferDesc, sizeof(zBufferDesc));
		zBufferDesc.ArraySize = 1;
		zBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		zBufferDesc.Width = m_Viewport.Width;
		zBufferDesc.Height = m_Viewport.Height;
		zBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		zBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		zBufferDesc.SampleDesc.Count = 1;
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

		textureID = MythosID();
		bufferID = MythosID();

		m_IDsToIndex.insert(std::make_pair(textureID, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureID, depthTexture));

		m_IDsToIndex.insert(std::make_pair(bufferID, MYTHOS_RESOURCE_DEPTH_BUFFER));
		m_Resources[MYTHOS_RESOURCE_DEPTH_BUFFER].insert(std::make_pair(bufferID, depthBuffer));

		return TRUE;
	}

	void Mythos::SetClearDepthBufferValue(float value)
	{
		m_DepthBufferClearValue = value;
	}

	void Mythos::ClearDepthBuffer(MythosID& bufferID)
	{
		IMythosResource* depthBuffer = GetResource(bufferID);
		if (depthBuffer)
			m_Context.GetContext()->ClearDepthStencilView((ID3D11DepthStencilView*)depthBuffer->GetData(), D3D11_CLEAR_DEPTH, m_DepthBufferClearValue, NULL);
	}

	BOOL Mythos::CreateVertexShader(const wchar_t* filePath, const char* entryPoint, const char* modelType, MythosID& id)
	{
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

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_VERTEX_SHADER));
		m_Resources[MYTHOS_RESOURCE_VERTEX_SHADER].insert(std::make_pair(id, vertexShader));
		m_ShaderBlobs.insert(std::make_pair(id, vertexBlob));

		return TRUE;
	}

	BOOL Mythos::CreatePixelShader(const wchar_t* filePath, const char* entryPoint, const char* modelType, MythosID& id)
	{
		ID3D10Blob* errorBlob;
		ID3D10Blob* pixelBlob;
		HRESULT hr = D3DCompileFromFile(filePath, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, modelType, NULL, NULL, &pixelBlob, &errorBlob);
		if (FAILED(hr))
			return FALSE;

		IMythosResource* pixelShader = new MythosPixelShader();
		hr = m_Creator.GetCreator()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, (ID3D11PixelShader**)&pixelShader->GetData());
		if (FAILED(hr)) {
			delete pixelShader;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_PIXEL_SHADER));
		m_Resources[MYTHOS_RESOURCE_PIXEL_SHADER].insert(std::make_pair(id, pixelShader));
		m_ShaderBlobs.insert(std::make_pair(id, pixelBlob));

		return TRUE;
	}

	BOOL Mythos::CreateInputLayout(const MythosInputElement* elements, unsigned int numElements, MythosID& vertexShaderID, MythosID& inputLayoutID)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		int totalBytes = 0;
		for (int i = 0; i < numElements; ++i)
		{
			D3D11_INPUT_ELEMENT_DESC temp;
			int byteOffset = 0;
			temp.SemanticName = elements[i].SemanticName;
			temp.SemanticIndex = elements[i].SemanticIndex;
			temp.Format = (DXGI_FORMAT)GetFormat(elements[i].Format);
			byteOffset = GetFormatSize(elements[i].Format);
			temp.InputSlot = 0;
			temp.AlignedByteOffset = totalBytes;
			temp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			temp.InstanceDataStepRate = 0;
			totalBytes += byteOffset;
			inputElements.push_back(temp);
		}

		auto blob = GetShaderBlob(vertexShaderID);
		IMythosResource* inputLayout = new MythosInputLayout();

		HRESULT hr = m_Creator.GetCreator()->CreateInputLayout(inputElements.data(), numElements, blob->GetBufferPointer(), blob->GetBufferSize(), (ID3D11InputLayout**)&inputLayout->GetData());
		if (FAILED(hr)) {
			delete inputLayout;
			return FALSE;
		}

		inputLayoutID = MythosID();
		m_IDsToIndex.insert(std::make_pair(inputLayoutID, MYTHOS_RESOURCE_INPUT_LAYOUT));
		m_Resources[MYTHOS_RESOURCE_INPUT_LAYOUT].insert(std::make_pair(inputLayoutID, inputLayout));

		return TRUE;
	}

	BOOL Mythos::CreateMainRenderTarget(MythosID& id)
	{
		ID3D11Resource* backbuffer;
		HRESULT hr = m_SwapChain.GetSwapChain()->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);

		if (FAILED(hr))
			return FALSE;

		IMythosResource* renderTarget = new MythosRenderTarget();
		hr = m_Creator.GetCreator()->CreateRenderTargetView(backbuffer, nullptr, (ID3D11RenderTargetView**)&renderTarget->GetData());

		if (FAILED(hr))
		{
			delete renderTarget;
			return FALSE;
		}

		backbuffer->Release();

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_RENDER_TARGET));
		m_Resources[MYTHOS_RESOURCE_RENDER_TARGET].insert(std::make_pair(id, renderTarget));

		return TRUE;
	}

	void Mythos::SetTopology(MythosTopology topology)
	{
		if (topology == MYTHOS_LINE_LIST)
			m_Context.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		if (topology == MYTHOS_TRIANGLE_LIST)
			m_Context.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	BOOL Mythos::CreateRenderTarget(unsigned int width, unsigned int height, MythosID& textureID, MythosID& renderTargetID)
	{
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

		textureID = MythosID();
		renderTargetID = MythosID();
		m_IDsToIndex.insert(std::make_pair(textureID, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureID, texture));

		m_IDsToIndex.insert(std::make_pair(renderTargetID, MYTHOS_RESOURCE_RENDER_TARGET));
		m_Resources[MYTHOS_RESOURCE_RENDER_TARGET].insert(std::make_pair(renderTargetID, renderTarget));

		return TRUE;
	}

	BOOL Mythos::CreateRenderTarget(MythosTextureDescriptor* desc, MythosID& textureID, MythosID& renderTargetID)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
		ZeroMemory(&renderDesc, sizeof(renderDesc));

		if (desc->bindFlags & MYTHOS_BIND_DEPTH)
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		if (desc->bindFlags & MYTHOS_BIND_RENDER_TARGET)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		if (desc->bindFlags & MYTHOS_BIND_SHADER_RESOURCE)
			textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		textureDesc.Format = (DXGI_FORMAT)GetFormat(desc->format);

		textureDesc.ArraySize = 1;
		textureDesc.Height = desc->height;
		textureDesc.Width = desc->width;

		textureDesc.MipLevels = desc->mipLevels;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = desc->sampleCount;
		textureDesc.SampleDesc.Quality = desc->sampleQuality;

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

		textureID = MythosID();
		renderTargetID = MythosID();

		m_IDsToIndex.insert(std::make_pair(textureID, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureID, texture));

		m_IDsToIndex.insert(std::make_pair(renderTargetID, MYTHOS_RESOURCE_RENDER_TARGET));
		m_Resources[MYTHOS_RESOURCE_RENDER_TARGET].insert(std::make_pair(renderTargetID, renderTarget));

		return TRUE;
	}

	void Mythos::SetClearRenderTargetColor(Math::Vector4 clearColor)
	{
		m_RenderTargetClearColor = clearColor;
	}

	void Mythos::ClearRenderTarget(MythosID& id)
	{
		IMythosResource* renderTarget = GetResource(id);
		if (renderTarget != nullptr)
			m_Context.GetContext()->ClearRenderTargetView((ID3D11RenderTargetView*)renderTarget->GetData(), m_RenderTargetClearColor.comp);
	}

	BOOL Mythos::CreateTexture2D(const wchar_t* filepath, MythosID& id)
	{
		IMythosResource* texture2D = new MythosTexture2D();
		HRESULT hr = DirectX::CreateDDSTextureFromFile(m_Creator.GetCreator(), filepath, (ID3D11Resource**)&texture2D->GetData(), nullptr);
		if (FAILED(hr))
		{
			delete texture2D;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(id, texture2D));

		return TRUE;
	}

	BOOL Mythos::CreateTexture2D(MythosTextureDescriptor* descriptor, IMythosResource* texture, MythosID& id)
	{
		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(id, texture));

		return TRUE;
	}

	BOOL Mythos::CreateTexture2D(MythosTextureDescriptor* desc, void* data, MythosID& id)
	{
		IMythosResource* texture2D = new MythosTexture2D();
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = GetBindFlags((MythosBindFlags)desc->bindFlags);
		textureDesc.Format = (DXGI_FORMAT)GetFormat(desc->format);
		textureDesc.MipLevels = desc->mipLevels;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = desc->sampleCount;
		textureDesc.SampleDesc.Quality = desc->sampleQuality;
		textureDesc.Height = desc->height;
		textureDesc.Width = desc->width;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = data;

		HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&textureDesc, data ? &subData : nullptr, (ID3D11Texture2D**)&texture2D->GetData());
		if (FAILED(hr)) {
			delete texture2D;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(id, texture2D));

		return TRUE;
	}

	BOOL Mythos::CreateTextureCube(MythosTextureDescriptor* desc, IMythosResource** textures, MythosID& id)
	{
		//Creating the Cube Map Texture Descriptor
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Height = desc->height;
		textureDesc.Width = desc->width;

		textureDesc.ArraySize = 6;
		textureDesc.BindFlags = GetBindFlags((MythosBindFlags)desc->bindFlags);
		textureDesc.MipLevels = desc->mipLevels;
		textureDesc.Format = (DXGI_FORMAT)GetFormat(desc->format);
		textureDesc.SampleDesc.Count = desc->sampleCount;
		textureDesc.SampleDesc.Quality = desc->sampleQuality;

		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		//Putting the pixel Data into the subresource data
		D3D11_SUBRESOURCE_DATA subData[6];

		ID3D11Texture2D* copies[6];

		for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; ++cubeMapFaceIndex)
		{

			D3D11_TEXTURE2D_DESC desc;
			ID3D11Texture2D* texture = (ID3D11Texture2D*)textures[cubeMapFaceIndex]->GetData();
			texture->GetDesc(&desc);

			D3D11_TEXTURE2D_DESC desc2;
			desc2.Width = desc.Width;
			desc2.Height = desc.Height;
			desc2.ArraySize = desc.ArraySize;
			desc2.BindFlags = 0;
			desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc2.Format = desc.Format;
			desc2.MipLevels = desc.MipLevels;
			desc2.MiscFlags = desc.MiscFlags;
			desc2.SampleDesc = desc.SampleDesc;
			desc2.Usage = D3D11_USAGE_STAGING;

			HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&desc2, nullptr, &copies[cubeMapFaceIndex]);
			if (FAILED(hr))
				return -1;

			m_Context.GetContext()->CopyResource(copies[cubeMapFaceIndex], texture);
			D3D11_MAPPED_SUBRESOURCE mapInfo;

			m_Context.GetContext()->Map(copies[cubeMapFaceIndex], 0, D3D11_MAP_READ, 0, &mapInfo);

			subData[cubeMapFaceIndex].pSysMem = mapInfo.pData;
			subData[cubeMapFaceIndex].SysMemPitch = mapInfo.RowPitch;
			subData[cubeMapFaceIndex].SysMemSlicePitch = mapInfo.DepthPitch;

			m_Context.GetContext()->Unmap(copies[cubeMapFaceIndex], 0);
		}

		IMythosResource* textureCube = new MythosTexture2D();
		HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&textureDesc, &subData[0], (ID3D11Texture2D**)&textureCube->GetData());
		if (FAILED(hr))
		{
			delete textureCube;
			return FALSE;
		};

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_CUBE));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_CUBE].insert(std::make_pair(id, textureCube));

		for (int i = 0; i < 6; i++) {
			copies[i]->Release();
		}

		return TRUE;

	}

	BOOL Mythos::CreateTextureCube(MythosTextureDescriptor* desc, MythosID* listOfTextureIDs, MythosID& id)
	{
		//Creating the Cube Map Texture Descriptor
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Height = desc->height;
		textureDesc.Width = desc->width;

		textureDesc.ArraySize = 6;
		textureDesc.BindFlags = GetBindFlags((MythosBindFlags)desc->bindFlags);
		textureDesc.MipLevels = 1;
		textureDesc.Format = (DXGI_FORMAT)GetFormat(desc->format);
		textureDesc.SampleDesc.Count = desc->sampleCount;
		textureDesc.SampleDesc.Quality = desc->sampleQuality;

		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


		switch (desc->cpuAccess)
		{
		case MYTHOS_DEFAULT_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
		case MYTHOS_DYNAMIC_ACCESS: {
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.Usage = D3D11_USAGE_DYNAMIC;
			break;
		}
		case MYTHOS_IMMUTABLE_ACCESS: {
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
		}

		//Putting the pixel Data into the subresource data
		D3D11_SUBRESOURCE_DATA subData[6];

		ID3D11Texture2D* copies[6];

		for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; ++cubeMapFaceIndex)
		{

			D3D11_TEXTURE2D_DESC desc;
			ID3D11Texture2D* texture = (ID3D11Texture2D*)GetResource(listOfTextureIDs[cubeMapFaceIndex])->GetData();
			texture->GetDesc(&desc);

			D3D11_TEXTURE2D_DESC desc2;
			desc2.Width = desc.Width;
			desc2.Height = desc.Height;
			desc2.ArraySize = desc.ArraySize;
			desc2.BindFlags = 0;
			desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc2.Format = desc.Format;
			desc2.MipLevels = desc.MipLevels;
			desc2.MiscFlags = 0;
			desc2.SampleDesc = desc.SampleDesc;
			desc2.Usage = D3D11_USAGE_STAGING;

			HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&desc2, nullptr, &copies[cubeMapFaceIndex]);
			if (FAILED(hr))
				return -1;

			m_Context.GetContext()->CopyResource(copies[cubeMapFaceIndex], texture);
			D3D11_MAPPED_SUBRESOURCE mapInfo;

			m_Context.GetContext()->Map(copies[cubeMapFaceIndex], 0, D3D11_MAP_READ, 0, &mapInfo);

			subData[cubeMapFaceIndex].pSysMem = mapInfo.pData;
			subData[cubeMapFaceIndex].SysMemPitch = mapInfo.RowPitch;
			subData[cubeMapFaceIndex].SysMemSlicePitch = 0;

			m_Context.GetContext()->Unmap(copies[cubeMapFaceIndex], 0);
		}

		IMythosResource* textureCube = new MythosTexture2D();
		HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&textureDesc, &subData[0], (ID3D11Texture2D**)&textureCube->GetData());
		if (FAILED(hr))
		{
			delete textureCube;
			return FALSE;
		};

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_CUBE));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_CUBE].insert(std::make_pair(id, textureCube));

		for (int i = 0; i < 6; i++) {
			copies[i]->Release();
		}

		return TRUE;

	}

	BOOL Mythos::CreateShaderResource(const wchar_t* filepath, MythosID& id)
	{
		IMythosResource* resource = new MythosShaderResource();
		HRESULT hr = DirectX::CreateDDSTextureFromFile(GetCreator(), filepath, nullptr, (ID3D11ShaderResourceView**)&resource->GetData());

		if (FAILED(hr))
		{
			delete resource;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(id, resource));

		return TRUE;
	}

	BOOL Mythos::CreateShaderResource(MythosID& textureToBecomeResourceID, MythosID& id)
	{
		D3D11_TEXTURE2D_DESC textureDesc;

		IMythosResource* texture = GetResource(textureToBecomeResourceID);
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

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(id, shaderResource));

		return TRUE;
	}

	BOOL Mythos::CreateShaderResource(IMythosResource* texture, MythosID& id)
	{
		D3D11_TEXTURE2D_DESC textureDesc;

		ID3D11Texture2D* tex = (ID3D11Texture2D*)texture->GetData();
		tex->GetDesc(&textureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
		ZeroMemory(&shaderDesc, sizeof(shaderDesc));
		shaderDesc.Format = textureDesc.Format;
		shaderDesc.TextureCube.MostDetailedMip = 0;
		shaderDesc.TextureCube.MipLevels = textureDesc.MipLevels;
		shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		IMythosResource* shaderResource = new MythosShaderResource();
		HRESULT hr = m_Creator.GetCreator()->CreateShaderResourceView(tex, &shaderDesc, (ID3D11ShaderResourceView**)&shaderResource->GetData());

		if (FAILED(hr)) {
			delete shaderResource;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(id, shaderResource));

		return TRUE;
	}

	BOOL Mythos::CreateShaderResourceCube(MythosID& cubeToBecomeResourceID, MythosID& id)
	{
		D3D11_TEXTURE2D_DESC textureDesc;

		IMythosResource* texture = GetResource(cubeToBecomeResourceID);
		((ID3D11Texture2D*)texture->GetData())->GetDesc(&textureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
		ZeroMemory(&shaderDesc, sizeof(shaderDesc));
		shaderDesc.Format = textureDesc.Format;
		shaderDesc.TextureCube.MostDetailedMip = 0;
		shaderDesc.TextureCube.MipLevels = textureDesc.MipLevels;
		shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

		IMythosResource* shaderResource = new MythosShaderResource();
		HRESULT hr = m_Creator.GetCreator()->CreateShaderResourceView((ID3D11Texture2D*)texture->GetData(), &shaderDesc, (ID3D11ShaderResourceView**)&shaderResource->GetData());

		if (FAILED(hr)) {
			delete shaderResource;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(id, shaderResource));

		return TRUE;
	}

	BOOL Mythos::CreateTexture2DAndShaderResource(const wchar_t* filepath, MythosID& textureID, MythosID& resourceID)
	{
		IMythosResource* texture = new MythosTexture2D();
		IMythosResource* resource = new MythosShaderResource();
		HRESULT hr = DirectX::CreateDDSTextureFromFile(m_Creator.GetCreator(), filepath, (ID3D11Resource**)&texture->GetData(), (ID3D11ShaderResourceView**)&resource->GetData());
		if (FAILED(hr)) {
			delete texture;
			delete resource;
			return FALSE;
		}

		textureID = MythosID();
		resourceID = MythosID();

		m_IDsToIndex.insert(std::make_pair(textureID, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(textureID, texture));

		m_IDsToIndex.insert(std::make_pair(resourceID, MYTHOS_RESOURCE_SHADER_RESOURCE));
		m_Resources[MYTHOS_RESOURCE_SHADER_RESOURCE].insert(std::make_pair(resourceID, resource));


		return TRUE;
	}

	BOOL Mythos::CreateTextureSampler(MythosID& id)
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1;
		samplerDesc.BorderColor[1] = 1;
		samplerDesc.BorderColor[2] = 1;
		samplerDesc.BorderColor[3] = 1;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_SAMPLER));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_SAMPLER].insert(std::make_pair(id, samplerState));

		return TRUE;
	}

	BOOL Mythos::CreateTextureSampler(MythosSamplerDescriptor* desc, MythosID& id)
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		D3D11_TEXTURE_ADDRESS_MODE addressMode;

		switch (desc->AddressMode)
		{
		case MYTHOS_TEXTURE_ADDRESS_BORDER: addressMode = D3D11_TEXTURE_ADDRESS_BORDER; break;
		case MYTHOS_TEXTURE_ADDRESS_CLAMP: addressMode = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case MYTHOS_TEXTURE_ADDRESS_MIRROR: addressMode = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		default: addressMode = D3D11_TEXTURE_ADDRESS_WRAP; break;
		}

		samplerDesc.AddressU = addressMode;
		samplerDesc.AddressV = addressMode;
		samplerDesc.AddressW = addressMode;
		samplerDesc.BorderColor[0] = desc->BorderColor.x;
		samplerDesc.BorderColor[1] = desc->BorderColor.y;
		samplerDesc.BorderColor[2] = desc->BorderColor.z;
		samplerDesc.BorderColor[3] = desc->BorderColor.w;
		samplerDesc.MaxAnisotropy = desc->MaxAnisotropy;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MinLOD = desc->MinLOD;
		samplerDesc.MaxLOD = desc->MaxLOD;
		samplerDesc.MaxAnisotropy = desc->MaxAnisotropy;
		samplerDesc.MipLODBias = desc->MipLODBias;

		IMythosResource* samplerState = new MythosTextureSampler();
		HRESULT hr = m_Creator.GetCreator()->CreateSamplerState(&samplerDesc, (ID3D11SamplerState**)&samplerState->GetData());
		if (FAILED(hr)) {
			delete samplerState;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_SAMPLER));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_SAMPLER].insert(std::make_pair(id, samplerState));

		return TRUE;
	}

	BOOL Mythos::CreateRasterizerState(MythosID& id)
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


		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_RASTERIZER));
		m_Resources[MYTHOS_RESOURCE_RASTERIZER].insert(std::make_pair(id, rasterState));

		return TRUE;
	}

	BOOL Mythos::CreateRasterizerState(MythosRasterizerDescriptor* desc, MythosID& id)
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.AntialiasedLineEnable = desc->AntialiasLineEnabled;

		switch (desc->CullMode)
		{
		case MYTHOS_CULL_NONE: rasterDesc.CullMode = D3D11_CULL_NONE; break;
		case MYTHOS_CULL_FRONT: rasterDesc.CullMode = D3D11_CULL_FRONT; break;
		case MYTHOS_CULL_BACK: rasterDesc.CullMode = D3D11_CULL_BACK; break;
		}

		switch (desc->FillMode)
		{
		case MYTHOS_FILL_WIREFRAME: rasterDesc.FillMode = D3D11_FILL_WIREFRAME; break;
		case MYTHOS_FILL_SOLID: rasterDesc.FillMode = D3D11_FILL_SOLID; break;
		}

		rasterDesc.DepthBias = desc->DepthBias;
		rasterDesc.DepthBiasClamp = desc->DepthBiasClamp;
		rasterDesc.DepthClipEnable = desc->DepthClipEnable;
		rasterDesc.FrontCounterClockwise = desc->FrontCounterClockwise;
		rasterDesc.MultisampleEnable = desc->MultiSampleEnable;
		rasterDesc.ScissorEnable = desc->ScissorEnable;
		rasterDesc.SlopeScaledDepthBias = desc->SlopeScaledDepthBias;

		IMythosResource* rasterState = new MythosRasterizer();
		HRESULT hr = m_Creator.GetCreator()->CreateRasterizerState(&rasterDesc, (ID3D11RasterizerState**)&rasterState->GetData());

		if (FAILED(hr))
		{
			delete rasterState;
			return FALSE;
		}

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_RASTERIZER));
		m_Resources[MYTHOS_RESOURCE_RASTERIZER].insert(std::make_pair(id, rasterState));

		return TRUE;
	}

	BOOL Mythos::UpdateMythosResource(MythosID& id, void* data, unsigned int byteSize)
	{
		IMythosResource* resource = m_Resources[m_IDsToIndex.find(id)->second].find(id)->second;

		if (!resource)
			return FALSE;

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		m_Context.GetContext()->Map((ID3D11Buffer*)resource->GetData(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, data, byteSize);
		m_Context.GetContext()->Unmap((ID3D11Buffer*)resource->GetData(), 0);

		return TRUE;
	}

	MythosModel* Mythos::LoadMesh(const char* filepath)
	{
		MythosModel* importedModel = new MythosModel();

		std::fstream file{ filepath, std::ios_base::in | std::ios_base::binary };

		if (!file.is_open())
			return 0;

		uint32_t modelCount;
		file.read((char*)&modelCount, sizeof(uint32_t));

		importedModel->m_Meshes.resize(modelCount);

		unsigned int totalNumIndices = 0;

		for (int i = 0; i < modelCount; ++i)
		{
			std::vector<MythosVertex> m_Vertices;
			std::vector<int> m_Indices;

			uint32_t player_index_count;
			file.read((char*)&player_index_count, sizeof(uint32_t));

			m_Indices.resize(player_index_count);

			file.read((char*)m_Indices.data(), sizeof(uint32_t) * player_index_count);

			uint32_t player_vertex_count;
			file.read((char*)&player_vertex_count, sizeof(uint32_t));

			m_Vertices.resize(player_vertex_count);

			file.read((char*)m_Vertices.data(), sizeof(MythosVertex) * player_vertex_count);

			MythosMesh* mesh = new MythosMesh();

			mesh->m_Indices = m_Indices;
			mesh->m_Vertices = m_Vertices;

			for (MythosVertex& vertex : mesh->m_Vertices)
			{
				vertex.uv.y = (1.0f - vertex.uv.y);
			}

			importedModel->m_Meshes[i] = mesh;

			totalNumIndices += m_Indices.size();
		}

		file.close();

		importedModel->m_TotalNumIndices = totalNumIndices;
		return importedModel;
	}

	BOOL Mythos::CreateDirectionalLight(Math::Vector3 direction, Math::Vector3 color, float intensity, MythosID& id)
	{
		MythosLight* light = new MythosLight(direction, color, intensity);
		if (!light)
			return FALSE;

		id = MythosID();
		m_Lights.insert(std::make_pair(id, light));
		return TRUE;
	}

	BOOL Mythos::CreatePointLight(Math::Vector3 position, Math::Vector3 color, float intensity, float radius, MythosID& id)
	{
		MythosLight* light = new MythosLight(position, color, intensity, radius);
		if (!light)
			return FALSE;

		id = MythosID();
		m_Lights.insert(std::make_pair(id, light));
		return TRUE;
	}

	BOOL Mythos::CreateSpotLight(Math::Vector3 position, Math::Vector3 direction, Math::Vector3 color, float intensity, float radians, float innerConeRadians, float outerConeRadians, MythosID& id)
	{
		MythosLight* light = new MythosLight(position, direction, color, intensity, cos(radians), cos(innerConeRadians), cos(outerConeRadians));
		if (!light)
			return FALSE;

		id = MythosID();
		m_Lights.insert(std::make_pair(id, light));
		return TRUE;
	}

	BOOL Mythos::CreateSkyboxFromEquirectangularTexture(unsigned int width, unsigned int height, const wchar_t* equirectangularTextureFilepath, MythosID& id)
	{
		//=========================================================================
		//Must Create a Skybox=====================================================
		//=========================================================================

		struct WVP
		{
			Math::Matrix4 World;
			Math::Matrix4 View;
			Math::Matrix4 Projection;
		}MyMatrices;

		Math::Vector3 cubeVerts[8] =
		{
			{1,1,-1},
			{1,-1,-1},
			{1,1,1,},
			{1,-1,1},
			{-1,1,-1},
			{-1,-1,-1},
			{-1,1,1},
			{-1,-1,1}
		};

		unsigned int indices[36] =
		{
			4,2,0,
			2,7,3,
			6,5,7,
			1,7,5,
			0,3,1,
			4,1,5,
			4,6,2,
			2,6,7,
			6,4,5,
			1,3,7,
			0,2,3,
			4,0,1
		};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11Buffer* constantBuffer = nullptr;

		D3D11_BUFFER_DESC vDesc, iDesc, cDesc;
		ZeroMemory(&vDesc, sizeof(vDesc));
		vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vDesc.ByteWidth = sizeof(Math::Vector3) * 8;
		vDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&iDesc, sizeof(iDesc));
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.ByteWidth = sizeof(unsigned int) * 36;
		iDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&cDesc, sizeof(cDesc));
		cDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cDesc.ByteWidth = sizeof(WVP);
		cDesc.Usage = D3D11_USAGE_DYNAMIC;
		cDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = cubeVerts;

		HRESULT hr = GetCreator()->CreateBuffer(&vDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return FALSE;

		subData.pSysMem = indices;

		hr = GetCreator()->CreateBuffer(&iDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return FALSE;

		hr = GetCreator()->CreateBuffer(&cDesc, nullptr, &constantBuffer);
		if (FAILED(hr))
			return FALSE;

		//=========================================================================
		//Create Input Layout======================================================
		//=========================================================================

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		ID3D11InputLayout* inputLayout;

		MythosID vertexShaderID, pixelShaderID;
		BOOL success = CreateVertexShader(L"Assets/Shaders/DiffuseIBLVertexShader.hlsl", "main", "vs_4_0", vertexShaderID);
		if (!success)
			return FALSE;
		success = CreatePixelShader(L"Assets/Shaders/EquirectangularToCubeMap.hlsl", "main", "ps_4_0", pixelShaderID);
		if (!success)
			return FALSE;
		ID3D10Blob* blob = GetShaderBlob(vertexShaderID);
		GetCreator()->CreateInputLayout(layout, 1, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
		GetContext()->IASetInputLayout(inputLayout);

		//=========================================================================
		//Render Targets and Camera Projections====================================
		//=========================================================================

		IMythosResource* textures[6] = {
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};

		ID3D11RenderTargetView* rtvs[6] = {
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};

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

		renderDesc.Format = textureDesc.Format;
		renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		for (int i = 0; i < 6; ++i)
		{
			textures[i] = new MythosTexture2D();
			hr = GetCreator()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&textures[i]->GetData());
			if (FAILED(hr)) {
				return FALSE;
			}


			hr = GetCreator()->CreateRenderTargetView((ID3D11Texture2D*)textures[i]->GetData(), &renderDesc, &rtvs[i]);
			if (FAILED(hr)) {
				return FALSE;
			}
		}

		Math::Matrix4 captureProjection = Math::Matrix4::PerspectiveFovLH(Math::radians(90.0f), 1.0f, 0.1f, 10.0);
		Math::Matrix4 captureViews[] = {
			Math::Matrix4::LookAtLH({0,0,0}, {1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {-1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,1.0,0.0}, {0,0,-1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,-1.0,0.0}, {0,0,1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,1.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,-1.0}, {0,1,0})
		};

		//=========================================================================
		//Create the Equirectangular Texture=======================================
		//=========================================================================

		MythosID iblResourceID;
		success = CreateShaderResource(equirectangularTextureFilepath, iblResourceID);
		if (!success)
			return -1;

		//=========================================================================
		//Rendering the Skybox out based on width and height specified
		//=========================================================================

		MythosID samplerStateID;
		MythosSamplerDescriptor samplerDesc;
		samplerDesc.AddressMode = MYTHOS_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor = Math::Vector4(1, 1, 1, 1);
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MipLODBias = 0.0f;
		success = CreateTextureSampler(&samplerDesc, samplerStateID);
		if (!success)
			return -1;

		UINT strides[] = { sizeof(Math::Vector3) };
		UINT offset[] = { 0 };

		D3D11_VIEWPORT textureViewport;
		textureViewport.Width = width;
		textureViewport.Height = height;
		textureViewport.TopLeftX = textureViewport.TopLeftY = 0;
		textureViewport.MinDepth = 0; textureViewport.MaxDepth = 1;

		GetContext()->RSSetViewports(1, &textureViewport);

		GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offset);
		GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		GetContext()->VSSetShader((ID3D11VertexShader*)GetResource(vertexShaderID)->GetData(), nullptr, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
		GetContext()->PSSetShader((ID3D11PixelShader*)GetResource(pixelShaderID)->GetData(), nullptr, 0);
		GetContext()->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&GetResource(iblResourceID)->GetData());
		GetContext()->PSSetSamplers(0, 1, (ID3D11SamplerState**)&GetResource(samplerStateID)->GetData());

		MyMatrices.World = Math::Matrix4::Scale(1, 1, -1);
		MyMatrices.Projection = captureProjection;

		for (int i = 0; i < 6; ++i)
		{
			MyMatrices.View = captureViews[i];
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			m_Context.GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			memcpy(gpuBuffer.pData, &MyMatrices, sizeof(WVP));
			m_Context.GetContext()->Unmap(constantBuffer, 0);

			GetContext()->OMSetRenderTargets(1, &rtvs[i], nullptr);

			GetContext()->DrawIndexed(36, 0, 0);
			Present();
		}


		//=========================================================================
		//Converting the rendered targets into a cube map
		//=========================================================================

		MythosTextureDescriptor cubeDesc;
		cubeDesc.bindFlags = MYTHOS_BIND_RENDER_TARGET | MYTHOS_BIND_SHADER_RESOURCE;
		cubeDesc.cpuAccess = MYTHOS_DEFAULT_ACCESS;
		cubeDesc.format = MYTHOS_FORMAT_32_FLOAT4;
		cubeDesc.width = width;
		cubeDesc.height = height;
		cubeDesc.mipLevels = 1;
		cubeDesc.sampleCount = 1;
		cubeDesc.sampleQuality = 0;

		MythosID tempID = MythosID();
		success = CreateTextureCube(&cubeDesc, textures, tempID);
		if (!success)
		{
			return FALSE;
		}

		success = CreateShaderResourceCube(tempID, id);
		if (!success) {
			return FALSE;
		}

		//=========================================================================
		//Clean up upon completion
		//=========================================================================

		vertexBuffer->Release();
		indexBuffer->Release();
		constantBuffer->Release();

		inputLayout->Release();

		for (int i = 0; i < 6; ++i)
		{
			textures[i]->SafeRelease();
			delete textures[i];
			rtvs[i]->Release();
		}

		DeleteResource(vertexShaderID);
		DeleteResource(pixelShaderID);
		DeleteResource(tempID);
		DeleteResource(samplerStateID);
		DeleteShaderBlob(vertexShaderID);

		return TRUE;
	}

	BOOL Mythos::ConvoluteSkybox(unsigned int width, unsigned int height, MythosID& textureToConvoluteID, MythosID& convolutedID)
	{
		//=========================================================================
		//Must Create a Skybox=====================================================
		//=========================================================================

		struct WVP
		{
			Math::Matrix4 World;
			Math::Matrix4 View;
			Math::Matrix4 Projection;
		}MyMatrices;

		Math::Vector3 cubeVerts[8] =
		{
			{1,1,-1},
			{1,-1,-1},
			{1,1,1,},
			{1,-1,1},
			{-1,1,-1},
			{-1,-1,-1},
			{-1,1,1},
			{-1,-1,1}
		};

		unsigned int indices[36] =
		{
			4,2,0,
			2,7,3,
			6,5,7,
			1,7,5,
			0,3,1,
			4,1,5,
			4,6,2,
			2,6,7,
			6,4,5,
			1,3,7,
			0,2,3,
			4,0,1
		};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11Buffer* constantBuffer = nullptr;

		D3D11_BUFFER_DESC vDesc, iDesc, cDesc;
		ZeroMemory(&vDesc, sizeof(vDesc));
		vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vDesc.ByteWidth = sizeof(Math::Vector3) * 8;
		vDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&iDesc, sizeof(iDesc));
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.ByteWidth = sizeof(unsigned int) * 36;
		iDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&cDesc, sizeof(cDesc));
		cDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cDesc.ByteWidth = sizeof(WVP);
		cDesc.Usage = D3D11_USAGE_DYNAMIC;
		cDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = cubeVerts;

		HRESULT hr = GetCreator()->CreateBuffer(&vDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return FALSE;

		subData.pSysMem = indices;

		hr = GetCreator()->CreateBuffer(&iDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return FALSE;

		hr = GetCreator()->CreateBuffer(&cDesc, nullptr, &constantBuffer);
		if (FAILED(hr))
			return FALSE;

		//=========================================================================
		//Render Targets and Camera Projections====================================
		//=========================================================================

		IMythosResource* textures[] = {
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
		};

		ID3D11RenderTargetView* rtvs[] = {
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
		};

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

		renderDesc.Format = textureDesc.Format;
		renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		for (int i = 0; i < 6; ++i)
		{
			textures[i] = new MythosTexture2D();
			HRESULT hr = GetCreator()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&textures[i]->GetData());
			if (FAILED(hr)) {
				return FALSE;
			}


			hr = GetCreator()->CreateRenderTargetView((ID3D11Texture2D*)textures[i]->GetData(), &renderDesc, &rtvs[i]);
			if (FAILED(hr)) {
				return FALSE;
			}
		}

		//=================================
		//Input Layout
		//=================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		ID3D11InputLayout* inputLayout;

		MythosID vertexShaderID, pixelShaderID;
		BOOL success = CreateVertexShader(L"Assets/Shaders/DiffuseIBLVertexShader.hlsl", "main", "vs_4_0", vertexShaderID);
		if (!success)
			return FALSE;
		success = CreatePixelShader(L"Assets/Shaders/IrradiencePreCompute.hlsl", "main", "ps_4_0", pixelShaderID);
		if (!success)
			return FALSE;
		ID3D10Blob* blob = GetShaderBlob(vertexShaderID);
		GetCreator()->CreateInputLayout(layout, 1, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
		GetContext()->IASetInputLayout(inputLayout);

		Math::Matrix4 captureProjection = Math::Matrix4::PerspectiveFovLH(Math::radians(90.0f), 1.0f, 0.1f, 10.0);
		Math::Matrix4 captureViews[] = {
			Math::Matrix4::LookAtLH({0,0,0}, {1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {-1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,1.0,0.0}, {0,0,-1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,-1.0,0.0}, {0,0,1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,1.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,-1.0}, {0,1,0})
		};

		MythosID samplerStateID;
		MythosSamplerDescriptor samplerDesc;
		samplerDesc.AddressMode = MYTHOS_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor = Math::Vector4(1, 1, 1, 1);
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MipLODBias = 0.0f;
		success = CreateTextureSampler(&samplerDesc, samplerStateID);
		if (!success)
			return -1;

		//=========================================================================
		//Rendering the Skybox out based on width and height specified
		//=========================================================================

		UINT strides[] = { sizeof(Math::Vector3) };
		UINT offset[] = { 0 };

		D3D11_VIEWPORT textureViewport;
		textureViewport.Width = width;
		textureViewport.Height = height;
		textureViewport.TopLeftX = textureViewport.TopLeftY = 0;
		textureViewport.MinDepth = 0; textureViewport.MaxDepth = 1;

		GetContext()->RSSetViewports(1, &textureViewport);

		GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offset);
		GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		GetContext()->VSSetShader((ID3D11VertexShader*)GetResource(vertexShaderID)->GetData(), nullptr, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
		GetContext()->PSSetShader((ID3D11PixelShader*)GetResource(pixelShaderID)->GetData(), nullptr, 0);
		GetContext()->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&GetResource(textureToConvoluteID)->GetData());
		GetContext()->PSSetSamplers(0, 1, (ID3D11SamplerState**)&GetResource(samplerStateID)->GetData());

		MyMatrices.World = Math::Matrix4::Scale(1, 1, -1);
		MyMatrices.Projection = captureProjection;

		for (int i = 0; i < 6; ++i)
		{
			MyMatrices.View = captureViews[i];
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			m_Context.GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			memcpy(gpuBuffer.pData, &MyMatrices, sizeof(WVP));
			m_Context.GetContext()->Unmap(constantBuffer, 0);

			GetContext()->OMSetRenderTargets(1, &rtvs[i], nullptr);

			GetContext()->DrawIndexed(36, 0, 0);
		}

		Present();

		MythosTextureDescriptor cubeDesc;
		cubeDesc.bindFlags = MYTHOS_BIND_RENDER_TARGET | MYTHOS_BIND_SHADER_RESOURCE;
		cubeDesc.cpuAccess = MYTHOS_DEFAULT_ACCESS;
		cubeDesc.format = MYTHOS_FORMAT_32_FLOAT4;
		cubeDesc.width = width;
		cubeDesc.height = height;
		cubeDesc.mipLevels = 1;
		cubeDesc.sampleCount = 1;
		cubeDesc.sampleQuality = 0;

		MythosID tempID;
		success = CreateTextureCube(&cubeDesc, textures, tempID);
		if (!success)
		{
			return FALSE;
		}

		success = CreateShaderResourceCube(tempID, convolutedID);
		if (!success) {
			return FALSE;
		}

		vertexBuffer->Release();
		indexBuffer->Release();
		constantBuffer->Release();

		inputLayout->Release();

		for (int i = 0; i < 6; ++i)
		{
			textures[i]->SafeRelease();
			delete textures[i];
			rtvs[i]->Release();
		}

		DeleteResource(vertexShaderID);
		DeleteResource(pixelShaderID);
		DeleteResource(tempID);
		DeleteResource(samplerStateID);
		DeleteShaderBlob(vertexShaderID);

		return TRUE;
	}

	BOOL Mythos::CreatePrefilteredEnvironment(unsigned int width, unsigned int height, MythosID& skyboxID, MythosID& prefilteredEnvironmentID)
	{
		//=========================================================================
		//Must Create a Skybox=====================================================
		//=========================================================================

		struct WVP
		{
			Math::Matrix4 World;
			Math::Matrix4 View;
			Math::Matrix4 Projection;
		}MyMatrices;

		Math::Vector3 cubeVerts[8] =
		{
			{1,1,-1},
			{1,-1,-1},
			{1,1,1,},
			{1,-1,1},
			{-1,1,-1},
			{-1,-1,-1},
			{-1,1,1},
			{-1,-1,1}
		};

		unsigned int indices[36] =
		{
			4,2,0,
			2,7,3,
			6,5,7,
			1,7,5,
			0,3,1,
			4,1,5,
			4,6,2,
			2,6,7,
			6,4,5,
			1,3,7,
			0,2,3,
			4,0,1
		};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11Buffer* vertexConstantBuffer = nullptr;
		ID3D11Buffer* pixelConstantBuffer = nullptr;

		D3D11_BUFFER_DESC vDesc, iDesc, cDesc;
		ZeroMemory(&vDesc, sizeof(vDesc));
		vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vDesc.ByteWidth = sizeof(Math::Vector3) * 8;
		vDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&iDesc, sizeof(iDesc));
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.ByteWidth = sizeof(unsigned int) * 36;
		iDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&cDesc, sizeof(cDesc));
		cDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cDesc.ByteWidth = sizeof(WVP);
		cDesc.Usage = D3D11_USAGE_DYNAMIC;
		cDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = cubeVerts;

		HRESULT hr = GetCreator()->CreateBuffer(&vDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return FALSE;

		subData.pSysMem = indices;

		hr = GetCreator()->CreateBuffer(&iDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return FALSE;

		hr = GetCreator()->CreateBuffer(&cDesc, nullptr, &vertexConstantBuffer);
		if (FAILED(hr))
			return FALSE;

		cDesc.ByteWidth = sizeof(Math::Vector4);

		hr = GetCreator()->CreateBuffer(&cDesc, nullptr, &pixelConstantBuffer);
		if (FAILED(hr))
			return FALSE;

		//=========================================================================
		//Render Targets and Camera Projections====================================
		//=========================================================================

		int maxMipLevels = 5;

		IMythosResource** textures = new IMythosResource * [6 * maxMipLevels];
		ID3D11RenderTargetView** rtvs = new ID3D11RenderTargetView * [6 * maxMipLevels];

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
		textureDesc.MipLevels = maxMipLevels;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;

		renderDesc.Format = textureDesc.Format;
		renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		for (int i = 0; i < 6 * maxMipLevels; ++i)
		{
			textures[i] = new MythosTexture2D();
			HRESULT hr = GetCreator()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&textures[i]->GetData());
			if (FAILED(hr)) {
				return FALSE;
			}


			hr = GetCreator()->CreateRenderTargetView((ID3D11Texture2D*)textures[i]->GetData(), &renderDesc, &rtvs[i]);
			if (FAILED(hr)) {
				return FALSE;
			}
		}

		Math::Matrix4 captureProjection = Math::Matrix4::PerspectiveFovLH(Math::radians(90.0f), 1.0f, 0.1f, 10.0);
		Math::Matrix4 captureViews[] = {
			Math::Matrix4::LookAtLH({0,0,0}, {1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {-1.0,0.0,0.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,1.0,0.0}, {0,0,-1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,-1.0,0.0}, {0,0,1}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,1.0}, {0,1,0}),
			Math::Matrix4::LookAtLH({0,0,0}, {0.0,0.0,-1.0}, {0,1,0})
		};

		//=================================
		//Input Layout
		//=================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		ID3D11InputLayout* inputLayout;

		MythosID vertexShaderID, pixelShaderID;
		BOOL success = CreateVertexShader(L"Assets/Shaders/DiffuseIBLVertexShader.hlsl", "main", "vs_4_0", vertexShaderID);
		if (!success)
			return FALSE;
		success = CreatePixelShader(L"Assets/Shaders/PrefilterEnvironment.hlsl", "main", "ps_4_0", pixelShaderID);
		if (!success)
			return FALSE;
		ID3D10Blob* blob = GetShaderBlob(vertexShaderID);
		GetCreator()->CreateInputLayout(layout, 1, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
		GetContext()->IASetInputLayout(inputLayout);

		MythosID samplerStateID;
		MythosSamplerDescriptor samplerDesc;
		samplerDesc.AddressMode = MYTHOS_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor = Math::Vector4(1, 1, 1, 1);
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MipLODBias = 0.0f;
		success = CreateTextureSampler(&samplerDesc, samplerStateID);
		if (!success)
			return -1;

		//=========================================================================
		//Rendering the Skybox out based on width and height specified
		//=========================================================================

		UINT strides[] = { sizeof(Math::Vector3) };
		UINT offset[] = { 0 };

		GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offset);
		GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		GetContext()->VSSetShader((ID3D11VertexShader*)GetResource(vertexShaderID)->GetData(), nullptr, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &vertexConstantBuffer);
		GetContext()->PSSetShader((ID3D11PixelShader*)GetResource(pixelShaderID)->GetData(), nullptr, 0);
		GetContext()->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&GetResource(skyboxID)->GetData());
		GetContext()->PSSetSamplers(0, 1, (ID3D11SamplerState**)&GetResource(samplerStateID)->GetData());

		MyMatrices.World = Math::Matrix4::Scale(1, 1, -1);
		MyMatrices.Projection = captureProjection;

		GetContext()->PSSetConstantBuffers(0, 1, &pixelConstantBuffer);

		int index = 0;
		for (int i = 0; i < 6; ++i)
		{
			MyMatrices.View = captureViews[i];
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			m_Context.GetContext()->Map(vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			memcpy(gpuBuffer.pData, &MyMatrices, sizeof(WVP));
			m_Context.GetContext()->Unmap(vertexConstantBuffer, 0);
			for (int mip = 0; mip < maxMipLevels; ++mip)
			{
				D3D11_VIEWPORT textureViewport;
				textureViewport.Width = width * pow(0.5, mip);
				textureViewport.Height = height * pow(0.5, mip);
				textureViewport.TopLeftX = textureViewport.TopLeftY = 0;
				textureViewport.MinDepth = 0; textureViewport.MaxDepth = 1;

				GetContext()->RSSetViewports(1, &textureViewport);

				float roughness = mip / (float)maxMipLevels;
				D3D11_MAPPED_SUBRESOURCE gpuBuffer;
				GetContext()->Map(pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
				memcpy(gpuBuffer.pData, &Math::Vector4(roughness), sizeof(Math::Vector4));
				GetContext()->Unmap(pixelConstantBuffer, 0);

				GetContext()->OMSetRenderTargets(1, &rtvs[index], nullptr);

				GetContext()->DrawIndexed(36, 0, 0);
				index++;
			}
		}

		Present();

		MythosID tempID;
		success = CombineTextureCubeMapAsMips(width, height, textures, maxMipLevels, tempID);
		if (!success)
			return FALSE;

		success = CreateShaderResourceCube(tempID, prefilteredEnvironmentID);
		if (!success) {
			return FALSE;
		}

		vertexBuffer->Release();
		indexBuffer->Release();
		vertexConstantBuffer->Release();
		pixelConstantBuffer->Release();

		inputLayout->Release();

		for (int i = 0; i < 6 * maxMipLevels; ++i)
		{
			textures[i]->SafeRelease();
			delete textures[i];
			rtvs[i]->Release();
		}

		DeleteResource(vertexShaderID);
		DeleteResource(pixelShaderID);
		DeleteResource(tempID);
		DeleteResource(samplerStateID);
		DeleteShaderBlob(vertexShaderID);

		delete[] textures;
		delete[] rtvs;

		return TRUE;
	}

	BOOL Mythos::CreateBRDFTexture(unsigned int width, unsigned int height, MythosID& brdfID)
	{
		//=========================================================================
		//Create a Plane===========================================================
		//=========================================================================

		struct WVP
		{
			Math::Matrix4 World;
			Math::Matrix4 View;
			Math::Matrix4 Projection;
		}MyMatrices;

		struct PlaneVertex
		{
			Math::Vector3 pos;
			Math::Vector2 tex;
		};

		PlaneVertex vertices[] =
		{
			{{1,1,1}, {1,0}},
			{{-1,1,1}, {0,0}},
			{{1,-1,1}, {1,1}},
			{{-1,-1,1}, {0,1}},
		};

		unsigned int indices[] = {
			0,2,1,
			2,3,1
		};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11Buffer* constantBuffer = nullptr;

		D3D11_BUFFER_DESC vDesc, iDesc, cDesc;
		ZeroMemory(&vDesc, sizeof(vDesc));
		vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vDesc.ByteWidth = sizeof(PlaneVertex) * 4;
		vDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&iDesc, sizeof(iDesc));
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.ByteWidth = sizeof(unsigned int) * 6;
		iDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&cDesc, sizeof(cDesc));
		cDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cDesc.ByteWidth = sizeof(WVP);
		cDesc.Usage = D3D11_USAGE_DYNAMIC;
		cDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = vertices;

		HRESULT hr = GetCreator()->CreateBuffer(&vDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return FALSE;

		subData.pSysMem = indices;

		hr = GetCreator()->CreateBuffer(&iDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return FALSE;

		hr = GetCreator()->CreateBuffer(&cDesc, nullptr, &constantBuffer);
		if (FAILED(hr))
			return FALSE;

		//=========================================================================
		//Create Input Layout======================================================
		//=========================================================================

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		ID3D11InputLayout* inputLayout;

		MythosID vertexShaderID, pixelShaderID;
		BOOL success = CreateVertexShader(L"Assets/Shaders/TextureVertexShader.hlsl", "main", "vs_4_0", vertexShaderID);
		if (!success)
			return FALSE;
		success = CreatePixelShader(L"Assets/Shaders/BRDF_LUT.hlsl", "main", "ps_4_0", pixelShaderID);
		if (!success)
			return FALSE;
		ID3D10Blob* blob = GetShaderBlob(vertexShaderID);
		GetCreator()->CreateInputLayout(layout, 2, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
		GetContext()->IASetInputLayout(inputLayout);

		//=========================================================================
		//Render Targets and Camera Projections====================================
		//=========================================================================

		IMythosResource* texture = nullptr;
		ID3D11RenderTargetView* rtv = nullptr;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
		ZeroMemory(&renderDesc, sizeof(renderDesc));

		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
		textureDesc.Height = height;
		textureDesc.Width = width;
		textureDesc.MipLevels = 1;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;

		renderDesc.Format = textureDesc.Format;
		renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		texture = new MythosTexture2D();
		hr = GetCreator()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&texture->GetData());
		if (FAILED(hr)) {
			return FALSE;
		}

		hr = GetCreator()->CreateRenderTargetView((ID3D11Texture2D*)texture->GetData(), &renderDesc, &rtv);
		if (FAILED(hr)) {
			return FALSE;
		}

		Math::Matrix4 captureProjection = Math::Matrix4::PerspectiveFovLH(Math::radians(90.0f), 1.0f, 0.1f, 10.0);
		Math::Matrix4 captureViews = Math::Matrix4::LookAtLH({ 0,0,0 }, { 0.0,0.0,1.0 }, { 0,1,0 });

		//=========================================================================
		//Rendering the Skybox out based on width and height specified
		//=========================================================================

		UINT strides[] = { sizeof(PlaneVertex) };
		UINT offset[] = { 0 };

		D3D11_VIEWPORT textureViewport;
		textureViewport.Width = width;
		textureViewport.Height = height;
		textureViewport.TopLeftX = textureViewport.TopLeftY = 0;
		textureViewport.MinDepth = 0; textureViewport.MaxDepth = 1;

		GetContext()->RSSetViewports(1, &textureViewport);

		GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offset);
		GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		GetContext()->VSSetShader((ID3D11VertexShader*)GetResource(vertexShaderID)->GetData(), nullptr, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
		GetContext()->PSSetShader((ID3D11PixelShader*)GetResource(pixelShaderID)->GetData(), nullptr, 0);

		//MyMatrices.World = Math::Matrix4::Scale(1, 1, -1);
		MyMatrices.World = Math::Matrix4::Identity;
		MyMatrices.View = captureViews;
		MyMatrices.Projection = captureProjection;

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, &MyMatrices, sizeof(WVP));
		GetContext()->Unmap(constantBuffer, 0);

		//GetContext()->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&GetResource("mainRenderTarget")->GetData(), nullptr);
		GetContext()->OMSetRenderTargets(1, &rtv, nullptr);

		GetContext()->DrawIndexed(6, 0, 0);
		Present();

		//=========================================================================
		//Converting the rendered targets into shader resource
		//=========================================================================

		success = CreateShaderResource(texture, brdfID);
		if (!success) {
			return FALSE;
		}

		//=========================================================================
		//Clean up upon completion
		//=========================================================================

		vertexBuffer->Release();
		indexBuffer->Release();
		constantBuffer->Release();

		inputLayout->Release();

		texture->SafeRelease();
		delete texture;
		rtv->Release();

		DeleteResource(vertexShaderID);
		DeleteResource(pixelShaderID);
		DeleteShaderBlob(vertexShaderID);

		return TRUE;
	}


	BOOL Mythos::CombineTexture2DsAsMips(unsigned int width, unsigned int height, IMythosResource** textures, int numMips, MythosID& id)
	{
		IMythosResource* combinedTexture = CombineAsMips2D(width, height, textures, numMips);
		if (!combinedTexture)
			return FALSE;

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_2D));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_2D].insert(std::make_pair(id, combinedTexture));

		return TRUE;
	}

	BOOL Mythos::CombineTextureCubeMapAsMips(unsigned int width, unsigned int height, IMythosResource** textures, int numMips, MythosID& id)
	{
		IMythosResource* combinedTexture = CombineAsMipsCube(width, height, textures, numMips);
		if (!combinedTexture)
			return FALSE;

		id = MythosID();
		m_IDsToIndex.insert(std::make_pair(id, MYTHOS_RESOURCE_TEXTURE_CUBE));
		m_Resources[MYTHOS_RESOURCE_TEXTURE_CUBE].insert(std::make_pair(id, combinedTexture));

		return TRUE;
	}

	IMythosResource* Mythos::GetResource(MythosID& id)
	{

		auto num = m_IDsToIndex.find(id);
		if (num != m_IDsToIndex.end())
		{
			unsigned int arrayIndex = num->second;
			auto iter = m_Resources[arrayIndex].find(id);
			if (iter != m_Resources[arrayIndex].end())
			{
				return iter->second;
			}
		}

		return nullptr;
	}

	void Mythos::DeleteResource(MythosID& id)
	{
		auto num = m_IDsToIndex.find(id);
		if (num != m_IDsToIndex.end())
		{
			unsigned int arrayIndex = num->second;
			auto iter = m_Resources[arrayIndex].find(id);
			if (iter != m_Resources[arrayIndex].end())
			{
				iter->second->SafeRelease();
				delete iter->second;
				m_Resources[arrayIndex].erase(id);
				m_IDsToIndex.erase(id);
			}
		}
	}

	ID3D10Blob* Mythos::GetShaderBlob(MythosID& id)
	{
		auto iter = m_ShaderBlobs.find(id);
		if (iter != m_ShaderBlobs.end())
			return iter->second;

		return nullptr;
	}

	void Mythos::DeleteShaderBlob(MythosID& id)
	{
		auto iter = m_ShaderBlobs.find(id);
		if (iter != m_ShaderBlobs.end())
		{
			iter->second->Release();
			m_ShaderBlobs.erase(id);
		}
	}

	MythosLight* Mythos::GetLight(MythosID& id)
	{
		auto iter = m_Lights.find(id);
		if (iter != m_Lights.end())
			return iter->second;

		return nullptr;
	}

	void Mythos::DeleteLight(MythosID& id)
	{
		auto iter = m_Lights.find(id);
		if (iter != m_Lights.end())
		{
			delete iter->second;
			m_Lights.erase(id);
		}
	}

	UINT Mythos::GetBindFlags(MythosBindFlags flags)
	{
		unsigned int binds = 0;
		if (flags & MYTHOS_BIND_RENDER_TARGET)
		{
			binds |= D3D11_BIND_RENDER_TARGET;
		}

		if (flags & MYTHOS_BIND_SHADER_RESOURCE) {
			binds |= D3D11_BIND_SHADER_RESOURCE;
		}

		return binds;
	}

	UINT Mythos::GetFormat(MythosFormat format)
	{
		unsigned int formatType = 0;
		switch (format)
		{
		case MYTHOS_FORMAT_32_FLOAT1:
			formatType = DXGI_FORMAT_R32_FLOAT;
			break;
		case MYTHOS_FORMAT_32_FLOAT2:
			formatType = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case MYTHOS_FORMAT_32_FLOAT3:
			formatType = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case MYTHOS_FORMAT_32_FLOAT4:
			formatType = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case MYTHOS_FORMAT_32_SINT1:
			formatType = DXGI_FORMAT_R32_SINT;
			break;
		case MYTHOS_FORMAT_32_SINT2:
			formatType = DXGI_FORMAT_R32G32_SINT;
			break;
		case MYTHOS_FORMAT_32_SINT3:
			formatType = DXGI_FORMAT_R32G32B32_SINT;
			break;
		case MYTHOS_FORMAT_32_SINT4:
			formatType = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		case MYTHOS_FORMAT_32_UINT1:
			formatType = DXGI_FORMAT_R32_UINT;
			break;
		case MYTHOS_FORMAT_32_UINT2:
			formatType = DXGI_FORMAT_R32G32_UINT;
			break;
		case MYTHOS_FORMAT_32_UINT3:
			formatType = DXGI_FORMAT_R32G32B32_UINT;
			break;
		case MYTHOS_FORMAT_32_UINT4:
			formatType = DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN1:
			formatType = DXGI_FORMAT_R32_TYPELESS;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN2:
			formatType = DXGI_FORMAT_R32G32_TYPELESS;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN3:
			formatType = DXGI_FORMAT_R32G32B32_TYPELESS;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN4:
			formatType = DXGI_FORMAT_R32G32B32A32_TYPELESS;
			break;
		case MYTHOS_FORMAT_32_DEPTH:
			formatType = DXGI_FORMAT_D32_FLOAT;
			break;
		case MYTHOS_FORMAT_8_FLOAT4:
			formatType = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		return formatType;
	}

	UINT Mythos::GetFormatSize(MythosFormat format)
	{
		unsigned int formatSize = 0;
		switch (format)
		{
		case MYTHOS_FORMAT_32_FLOAT1:
			formatSize = sizeof(float) * 1;
			break;
		case MYTHOS_FORMAT_32_FLOAT2:
			formatSize = sizeof(float) * 2;
			break;
		case MYTHOS_FORMAT_32_FLOAT3:
			formatSize = sizeof(float) * 3;
			break;
		case MYTHOS_FORMAT_32_FLOAT4:
			formatSize = sizeof(float) * 4;
			break;
		case MYTHOS_FORMAT_32_SINT1:
			formatSize = sizeof(int) * 1;
			break;
		case MYTHOS_FORMAT_32_SINT2:
			formatSize = sizeof(int) * 2;
			break;
		case MYTHOS_FORMAT_32_SINT3:
			formatSize = sizeof(int) * 3;
			break;
		case MYTHOS_FORMAT_32_SINT4:
			formatSize = sizeof(int) * 4;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN1:
			formatSize = 4;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN2:
			formatSize = 8;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN3:
			formatSize = 12;
			break;
		case MYTHOS_FORMAT_32_UNKNOWN4:
			formatSize = 16;
			break;
		case MYTHOS_FORMAT_32_DEPTH:
			formatSize = 4;
			break;
		case MYTHOS_FORMAT_8_FLOAT4:
			formatSize = sizeof(char) * 4;
		}
		return formatSize;
	}
	IMythosResource* Mythos::CombineAsMips2D(unsigned int width, unsigned int height, IMythosResource** textures, int numMips)
	{
		D3D11_SUBRESOURCE_DATA* subData = new D3D11_SUBRESOURCE_DATA[numMips];
		ID3D11Texture2D** copies = new ID3D11Texture2D * [numMips];

		for (int mipLevel = 0; mipLevel < numMips; ++mipLevel)
		{
			D3D11_TEXTURE2D_DESC desc;
			ID3D11Texture2D* texture = (ID3D11Texture2D*)textures[mipLevel]->GetData();
			texture->GetDesc(&desc);

			D3D11_TEXTURE2D_DESC desc2;
			memcpy(&desc2, &desc, sizeof(desc));
			desc2.BindFlags = 0;
			desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc2.Usage = D3D11_USAGE_STAGING;

			HRESULT hr = GetCreator()->CreateTexture2D(&desc2, nullptr, &copies[mipLevel]);
			if (FAILED(hr))
				return FALSE;

			GetContext()->CopyResource(copies[mipLevel], texture);

			D3D11_MAPPED_SUBRESOURCE mapInfo;
			m_Context.GetContext()->Map(copies[mipLevel], 0, D3D11_MAP_READ, 0, &mapInfo);

			subData[mipLevel].pSysMem = mapInfo.pData;
			subData[mipLevel].SysMemPitch = mapInfo.RowPitch;
			subData[mipLevel].SysMemSlicePitch = mapInfo.DepthPitch;

			m_Context.GetContext()->Unmap(copies[mipLevel], 0);
		}

		D3D11_TEXTURE2D_DESC newDesc;
		ZeroMemory(&newDesc, sizeof(newDesc));
		newDesc.ArraySize = 1;
		newDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		newDesc.MipLevels = numMips;
		newDesc.MiscFlags = 0;
		newDesc.CPUAccessFlags = 0;
		newDesc.Usage = D3D11_USAGE_DEFAULT;
		newDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		newDesc.Width = width;
		newDesc.Height = height;
		newDesc.SampleDesc.Count = 1;

		IMythosResource* combinedTexture = new MythosTexture2D();
		HRESULT hr = GetCreator()->CreateTexture2D(&newDesc, &subData[0], (ID3D11Texture2D**)&combinedTexture->GetData());

		//CLEAN UP=========================
		for (int i = 0; i < numMips; ++i) {
			copies[i]->Release();
		}
		delete[] subData;
		delete[] copies;
		//CLEAN UP=========================

		if (FAILED(hr)) {
			delete combinedTexture;
			return nullptr;
		}

		return combinedTexture;
	}

	IMythosResource* Mythos::CombineAsMipsCube(unsigned int width, unsigned int height, IMythosResource** textures, int numMips)
	{
		D3D11_SUBRESOURCE_DATA* subData = new D3D11_SUBRESOURCE_DATA[6 * numMips];
		ID3D11Texture2D** copies = new ID3D11Texture2D * [6 * numMips];

		for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6 * numMips; ++cubeMapFaceIndex)
		{
			D3D11_TEXTURE2D_DESC desc;
			ID3D11Texture2D* texture = (ID3D11Texture2D*)textures[cubeMapFaceIndex]->GetData();
			texture->GetDesc(&desc);

			D3D11_TEXTURE2D_DESC desc2;
			desc2.Width = desc.Width;
			desc2.Height = desc.Height;
			desc2.ArraySize = desc.ArraySize;
			desc2.BindFlags = 0;
			desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc2.Format = desc.Format;
			desc2.MipLevels = desc.MipLevels;
			desc2.MiscFlags = desc.MiscFlags;
			desc2.SampleDesc = desc.SampleDesc;
			desc2.Usage = D3D11_USAGE_STAGING;

			HRESULT hr = m_Creator.GetCreator()->CreateTexture2D(&desc2, nullptr, &copies[cubeMapFaceIndex]);
			if (FAILED(hr))
				return nullptr;

			m_Context.GetContext()->CopyResource(copies[cubeMapFaceIndex], texture);
			D3D11_MAPPED_SUBRESOURCE mapInfo;

			m_Context.GetContext()->Map(copies[cubeMapFaceIndex], 0, D3D11_MAP_READ, 0, &mapInfo);

			subData[cubeMapFaceIndex].pSysMem = mapInfo.pData;
			subData[cubeMapFaceIndex].SysMemPitch = mapInfo.RowPitch;
			subData[cubeMapFaceIndex].SysMemSlicePitch = mapInfo.DepthPitch;

			m_Context.GetContext()->Unmap(copies[cubeMapFaceIndex], 0);
		}

		D3D11_TEXTURE2D_DESC newDesc;
		ZeroMemory(&newDesc, sizeof(newDesc));
		newDesc.ArraySize = 6;
		newDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		newDesc.MipLevels = numMips;
		newDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		newDesc.CPUAccessFlags = 0;
		newDesc.Usage = D3D11_USAGE_DEFAULT;
		newDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		newDesc.Width = width;
		newDesc.Height = height;
		newDesc.SampleDesc.Count = 1;

		IMythosResource* combinedTexture = new MythosTexture2D();
		HRESULT hr = GetCreator()->CreateTexture2D(&newDesc, &subData[0], (ID3D11Texture2D**)&combinedTexture->GetData());

		//CLEAN UP=========================
		for (int i = 0; i < 6 * numMips; ++i) {
			copies[i]->Release();
		}
		delete[] subData;
		delete[] copies;
		//CLEAN UP=========================

		if (FAILED(hr)) {
			delete combinedTexture;
			return nullptr;
		}

		return combinedTexture;
	}
}