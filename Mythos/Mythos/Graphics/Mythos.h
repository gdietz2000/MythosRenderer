#pragma once

#include <unordered_map>
#include "IMythosResource.h"
#include "MythosCreator.h"
#include "MythosContext.h"
#include "MythosSwapChain.h"

namespace Mythos
{
#define BIT(x) 1 << x

	enum MythosResourceTypes
	{
		MYTHOS_RESOURCE_VERTEX_BUFFER = 0,
		MYTHOS_RESOURCE_INDEX_BUFFER,
		MYTHOS_RESOURCE_CONSTANT_BUFFER,
		MYTHOS_RESOURCE_VERTEX_SHADER,
		MYTHOS_RESOURCE_HULL_SHADER,
		MYTHOS_RESOURCE_TESSELLATION_SHADER,
		MYTHOS_RESOURCE_DOMAIN_SHADER,
		MYTHOS_RESOURCE_GEOMETRY_SHADER,
		MYTHOS_RESOURCE_COMPUTE_SHADER,
		MYTHOS_RESOURCE_PIXEL_SHADER,
		MYTHOS_RESOURCE_INPUT_LAYOUT,
		MYTHOS_RESOURCE_COUNT
	};

	//This class will be the main accessor when it comes to rendering
	class Mythos 
	{
	public:
		//Mythos will be creating the resources that will be used for rendering.
		Mythos(void* window);
		~Mythos();
		//All of these are temporary
		inline ID3D11Device* GetCreator() { return m_Creator.GetCreator(); }
		inline ID3D11DeviceContext* GetContext() { return m_Context.GetContext(); }
		inline void Present() { m_SwapChain.GetSwapChain()->Present(0, 0); }
		inline ID3D11RenderTargetView* GetMainRenderTarget() { return m_RTV; }
		inline D3D11_VIEWPORT GetViewport() { return m_Viewport; }

		BOOL CreateVertexBuffer(void* data, unsigned int byteSize, const char* name);
		BOOL CreateIndexBuffer(void* data, unsigned int byteSize, const char* name);
		BOOL CreateConstantBuffer(void* data, unsigned int byteSize, const char* name);

		BOOL CreateVertexShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, const char* name);
		BOOL CreatePixelShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, const char* name);

		BOOL UpdateMythosResource(const char* name, void* data, unsigned int byteSize);

		//GetResource will return a resource based off of the id given.
		IMythosResource* GetResource(const char* name);
		ID3D10Blob* GetShaderBlob(const char* name);
	private:
		MythosCreator m_Creator;
		MythosContext m_Context;
		MythosSwapChain m_SwapChain;
		D3D11_VIEWPORT m_Viewport;
		ID3D11RenderTargetView* m_RTV;

		BOOL NameAvailable(const char* name);

		/*Create a system where resources are seperated based on resource type ie:
		Vertex Buffers
		Index Buffers
		Constant Buffers
		Texture2Ds
		Depth Buffers
		Render Target Views
		Vertex Shaders
		Pixel Shaders
		Geometry Shaders
		Compute Shaders
		Tesselation Shaders
		Shader Resource Views
		Rasterizer States
		Input Layouts
		*/

		//Assuming you need to find a vertex buffer with the name "VB" using the Get Resource function 
		//m_NamesToIndex will find the index the Vertex Buffers will be, which again can find the Resource
		std::unordered_map<const char*, unsigned int> m_NamesToIndex;
		std::vector<std::unordered_map<const char*, IMythosResource*>> m_Resources;
		std::unordered_map<const char*, ID3D10Blob*> m_ShaderBlobs;
	};
}