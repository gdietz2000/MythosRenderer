#pragma once

#include <unordered_map>
#include "IMythosResource.h"
#include "MythosCreator.h"
#include "MythosContext.h"
#include "MythosSwapChain.h"

namespace Mythos
{
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
		/*Will eventually contain a map of maps for:
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