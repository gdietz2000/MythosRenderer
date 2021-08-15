#pragma once

#include <unordered_map>
#include "IMythosResource.h"
#include "MythosCreator.h"
#include "MythosContext.h"
#include "MythosSwapChain.h"

namespace Mythos
{
#define BIT(x) 1 << x

	enum MythosResourceTypes : unsigned long
	{
		NONE = 0,
		MYTHOS_RESOURCE_VERTEX_BUFFER = BIT(0),
		MYTHOS_RESOURCE_INDEX_BUFFER = BIT(1),
		MYTHOS_RESOURCE_CONSTANT_BUFFER = BIT(2),
		MYTHOS_RESOURCE_VERTEX_SHADER = BIT(3),
		MYTHOS_RESOURCE_HULL_SHADER = BIT(4),
		MYTHOS_RESOURCE_TESSELLATION_SHADER = BIT(5),
		MYTHOS_RESOURCE_DOMAIN_SHADER = BIT(6),
		MYTHOS_RESOURCE_GEOMETRY_SHADER = BIT(7),
		MYTHOS_RESOURCE_COMPUTE_SHADER = BIT(8),
		MYTHOS_RESOURCE_PIXEL_SHADER = BIT(9),
		MYTHOS_RESOURCE_INPUT_LAYOUT = BIT(10),
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

		BOOL CreateVertexBuffer(void* data, unsigned int byteSize);
		BOOL CreateIndexBuffer(void* data, unsigned int byteSize);
		BOOL CreateConstantBuffer(void* data, unsigned int byteSize);

		BOOL CreateVertexShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType);
		BOOL CreatePixelShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType);

		BOOL UpdateMythosResource(unsigned int id, void* data, unsigned int byteSize);

		//GetResource will return a resource based off of the id given.
		IMythosResource* GetResource(unsigned int id);
		ID3D10Blob* GetShaderBlob(unsigned int id);
	private:
		MythosCreator m_Creator;
		MythosContext m_Context;
		MythosSwapChain m_SwapChain;
		D3D11_VIEWPORT m_Viewport;
		ID3D11RenderTargetView* m_RTV;

		static unsigned int m_NextID;
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
		std::unordered_map<unsigned int, IMythosResource*> m_Resources;
		std::unordered_map<unsigned int, ID3D10Blob*> m_ShaderBlobs;
	};
}