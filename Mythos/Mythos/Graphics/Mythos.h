#pragma once

#include "Vector4.h"

#include <unordered_map>
#include "IMythosResource.h"
#include "MythosCreator.h"
#include "MythosContext.h"
#include "MythosSwapChain.h"
#include "MythosMesh.h"
#include "MythosDescriptors.h"

namespace Mythos
{

	enum MythosResourceTypes
	{
		MYTHOS_RESOURCE_VERTEX_BUFFER = 0,
		MYTHOS_RESOURCE_BATCH_VERTEX_BUFFER,
		MYTHOS_RESOURCE_INDEX_BUFFER,
		MYTHOS_RESOURCE_BATCH_INDEX_BUFFER,
		MYTHOS_RESOURCE_CONSTANT_BUFFER,
		MYTHOS_RESOURCE_DEPTH_BUFFER,
		MYTHOS_RESOURCE_VERTEX_SHADER,
		MYTHOS_RESOURCE_HULL_SHADER,
		MYTHOS_RESOURCE_TESSELLATION_SHADER,
		MYTHOS_RESOURCE_DOMAIN_SHADER,
		MYTHOS_RESOURCE_GEOMETRY_SHADER,
		MYTHOS_RESOURCE_COMPUTE_SHADER,
		MYTHOS_RESOURCE_PIXEL_SHADER,
		MYTHOS_RESOURCE_INPUT_LAYOUT,
		MYTHOS_RESOURCE_RENDER_TARGET,
		MYTHOS_RESOURCE_TEXTURE_2D,
		MYTHOS_RESOURCE_TEXTURE_3D,
		MYTHOS_RESOURCE_SHADER_RESOURCE,
		MYTHOS_RESOURCE_TEXTURE_SAMPLER,
		MYTHOS_RESOURCE_RASTERIZER,
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
		inline D3D11_VIEWPORT GetViewport() { return m_Viewport; }

		BOOL CreateVertexBuffer(void* data, unsigned int byteSize, const char* name);
		BOOL CreateVertexBuffer(MythosBufferDescriptor* descriptor, const char* name);

		BOOL CreateIndexBuffer(void* data, unsigned int byteSize, const char* name);
		BOOL CreateIndexBuffer(MythosBufferDescriptor* descriptor, const char* name);

		BOOL CreateModelBuffers(MythosModel* object, const char* vertexName, const char* indexName);

		BOOL CreateConstantBuffer(void* data, unsigned int byteSize, const char* name);
		BOOL CreateConstantBuffer(MythosBufferDescriptor* descriptor, const char* name);

		BOOL CreateDepthBuffer(const char* depthTextureName, const char* depthBufferName);

		void SetClearDepthBufferValue(float clearValue);
		void ClearDepthBuffer(const char* depthBufferName);

		BOOL CreateVertexShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, const char* name);
		BOOL CreatePixelShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, const char* name);

		BOOL CreateInputLayout(const MythosInputElement* elements, unsigned int numElements, const char* vertexShaderName, const char* inputLayoutName);

		BOOL CreateMainRenderTarget(const char* renderTargetName);

		void SetTopology(MythosTopology topology);

		BOOL CreateRenderTarget(unsigned int width, unsigned int height, const char* textureName, const char* renderTargetName);
		BOOL CreateRenderTarget(MythosTextureDescriptor* descriptor, const char* textureName, const char* renderTargetName);

		void SetClearRenderTargetColor(Math::Vector4 clearColor);
		void ClearRenderTarget(const char* renderTargetName);

		BOOL CreateTexture2D(const wchar_t* filepath, const char* textureName);
		BOOL CreateTexture2D(MythosTextureDescriptor* descriptor, IMythosResource* texture, const char* textureName);
		BOOL CreateTexture2D(MythosTextureDescriptor* descriptor, void* data, const char* textureName);

		BOOL CreateTextureCube(MythosTextureDescriptor* descriptor, IMythosResource** textures, const char* textureName);
		BOOL CreateTextureCube(MythosTextureDescriptor* descriptor, const char** namesOfTextures, const char* textureName);

		BOOL CreateShaderResource(const wchar_t* filepath, const char* shaderResourceName);
		BOOL CreateShaderResource(const char* textureToBecomeResourceName, const char* shaderResourceName);
		BOOL CreateShaderResource(IMythosResource* texture, const char* shaderResourceName);
		BOOL CreateShaderResourceCube(const char* cubeToBecomeResourceName, const char* shaderResourceName);

		BOOL CreateTexture2DAndShaderResource(const wchar_t* filepath, const char* textureName, const char* shaderResourceName);

		BOOL CreateTextureSampler(const char* samplerName);
		BOOL CreateTextureSampler(MythosSamplerDescriptor* descriptor, const char* samplerName);

		BOOL CreateRasterizerState(const char* rasterizerName);
		BOOL CreateRasterizerState(MythosRasterizerDescriptor* descriptor, const char* rasterizerName);

		BOOL UpdateMythosResource(const char* name, void* data, unsigned int byteSize);

		MythosModel* LoadMesh(const char* filepath);

		//PBR Specific Functions
		BOOL CreateSkyboxFromEquirectangularTexture(unsigned int width, unsigned int height, const wchar_t* equirectangularTextureFilepath, const char* textureCubeName);
		BOOL ConvoluteSkybox(unsigned int width, unsigned int height, const char* textureCubeName, const char* convolutedTextureCubeName);
		BOOL CreatePrefilteredEnvironment(unsigned int width, unsigned int height, const char* textureCubeName, const char* prefilteredTextureCubeName);
		BOOL CreateBRDFTexture(unsigned int width, unsigned int height, const char* brdfTextureName);

		BOOL GenerateMippedTextureOfDifferentColors(unsigned int width, unsigned int height, const char* mipTexture);

		BOOL CombineTexture2DsAsMips(unsigned int width, unsigned int height, IMythosResource** textures, int numMips, const char* combinedTextureName);

		//GetResource will return a resource based off of the id given.
		IMythosResource* GetResource(const char* name);
		ID3D10Blob* GetShaderBlob(const char* name);
	private:
		MythosCreator m_Creator;
		MythosContext m_Context;
		MythosSwapChain m_SwapChain;
		D3D11_VIEWPORT m_Viewport;

		//Helper Function

		BOOL NameAvailable(const char* name);
		UINT GetBindFlags(MythosBindFlags flag);
		UINT GetFormat(MythosFormat format);
		UINT GetFormatSize(MythosFormat format);

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

		Math::Vector4 m_RenderTargetClearColor;
		float m_DepthBufferClearValue;

		//Assuming you need to find a vertex buffer with the name "VB" using the Get Resource function 
		//m_NamesToIndex will find the index the Vertex Buffers will be, which again can find the Resource
		std::unordered_map<const char*, unsigned int> m_NamesToIndex;
		std::vector<std::unordered_map<const char*, IMythosResource*>> m_Resources;
		std::unordered_map<const char*, ID3D10Blob*> m_ShaderBlobs;

		//Storage for Models
		std::unordered_map<unsigned int, MythosMesh*> m_MythosMeshes;
	};
}