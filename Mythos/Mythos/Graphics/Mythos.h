#pragma once

#include "Vector4.h"

#include <unordered_map>
#include "IMythosResource.h"
#include "MythosCreator.h"
#include "MythosContext.h"
#include "MythosSwapChain.h"
#include "MythosMesh.h"
#include "MythosDescriptors.h"
#include "MythosLight.h"
#include "MythosMaterial.h"

#include "MythosID.h"

namespace Mythos
{
	//Resource Types for hashing
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
		MYTHOS_RESOURCE_TEXTURE_CUBE,
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
		
		inline ID3D11Device* GetCreator() { return m_Creator.GetCreator(); }
		inline ID3D11DeviceContext* GetContext() { return m_Context.GetContext(); }
		inline void Present() { m_SwapChain.GetSwapChain()->Present(0, 0); }
		inline D3D11_VIEWPORT GetViewport() { return m_Viewport; }

		BOOL CreateVertexBuffer(void* data, unsigned int byteSize, MythosID& id);
		BOOL CreateVertexBuffer(MythosBufferDescriptor* descriptor, MythosID& id);

		BOOL CreateIndexBuffer(void* data, unsigned int byteSize, MythosID& id);
		BOOL CreateIndexBuffer(MythosBufferDescriptor* descriptor, MythosID& id);

		BOOL CreateModelBuffers(MythosModel* object, MythosID& vertexID, MythosID& indexID);

		BOOL CreateConstantBuffer(void* data, unsigned int byteSize, MythosID& id);
		BOOL CreateConstantBuffer(MythosBufferDescriptor* descriptor, MythosID& id);

		BOOL CreateDepthBuffer(MythosID& textureID, MythosID& bufferID);

		void SetClearDepthBufferValue(float clearValue);
		void ClearDepthBuffer(MythosID& bufferID);

		BOOL CreateVertexShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, MythosID& id);
		BOOL CreatePixelShader(const wchar_t* shaderFilePath, const char* shaderEntryPoint, const char* shaderModelType, MythosID& id);

		BOOL CreateInputLayout(const MythosInputElement* elements, unsigned int numElements, MythosID& vertexShaderID, MythosID& inputLayoutID);

		BOOL CreateMainRenderTarget(MythosID& mainRenderTargetID);

		void SetTopology(MythosTopology topology);

		BOOL CreateRenderTarget(unsigned int width, unsigned int height, MythosID& textureID, MythosID& renderTargetID);
		BOOL CreateRenderTarget(MythosTextureDescriptor* descriptor, MythosID& textureID, MythosID& renderTargetID);

		void SetClearRenderTargetColor(Math::Vector4 clearColor);
		void ClearRenderTarget(MythosID& renderTargetID);

		BOOL CreateTexture2D(const wchar_t* filepath, MythosID& id);
		BOOL CreateTexture2D(MythosTextureDescriptor* descriptor, IMythosResource* texture, MythosID& id);
		BOOL CreateTexture2D(MythosTextureDescriptor* descriptor, void* data, MythosID& id);

		BOOL CreateTextureCube(MythosTextureDescriptor* descriptor, IMythosResource** textures, MythosID& id);
		BOOL CreateTextureCube(MythosTextureDescriptor* descriptor, MythosID* listOfTextureIDs, MythosID& id);

		BOOL CreateShaderResource(const wchar_t* filepath, MythosID& id);
		BOOL CreateShaderResource(MythosID& textureToBecomeResourceID, MythosID& id);
		BOOL CreateShaderResource(IMythosResource* texture, MythosID& id);
		BOOL CreateShaderResourceCube(MythosID& cubeToBecomeResourceID, MythosID& id);

		BOOL CreateTexture2DAndShaderResource(const wchar_t* filepath, MythosID& textureId, MythosID& resourceId);

		BOOL CreateTextureSampler(MythosID& id);
		BOOL CreateTextureSampler(MythosSamplerDescriptor* descriptor, MythosID& id);

		BOOL CreateRasterizerState(MythosID& id);
		BOOL CreateRasterizerState(MythosRasterizerDescriptor* descriptor, MythosID& id);

		BOOL UpdateMythosResource(MythosID& id, void* data, unsigned int byteSize);

		MythosModel* LoadMesh(const char* filepath);

		BOOL CreateDirectionalLight(Math::Vector3 direction, Math::Vector3 color, float intensity, MythosID& id);
		BOOL CreatePointLight(Math::Vector3 position, Math::Vector3 color, float intensity, float radius, MythosID& id);
		BOOL CreateSpotLight(Math::Vector3 position, Math::Vector3 direction, Math::Vector3 color, float intensity, float radians, float innerConeRadians, float outerConeRadians, MythosID& id);

		//Setters
		void SetMaterial(UINT startSlot, MythosMaterial& mat);

		//PBR Specific Functions
		BOOL CreateSkyboxFromEquirectangularTexture(unsigned int width, unsigned int height, const wchar_t* equirectangularTextureFilepath, MythosID& id);
		BOOL ConvoluteSkybox(unsigned int width, unsigned int height, MythosID& textureToConvoluteID, MythosID& convolutedID);
		BOOL CreatePrefilteredEnvironment(unsigned int width, unsigned int height, MythosID& skyboxID, MythosID& prefilteredEnvironment);
		BOOL CreateBRDFTexture(unsigned int width, unsigned int height, MythosID& brdfID);

		BOOL CombineTexture2DsAsMips(unsigned int width, unsigned int height, IMythosResource** textures, int numMips, MythosID& id);
		BOOL CombineTextureCubeMapAsMips(unsigned int width, unsigned int height, IMythosResource** textures, int numMips, MythosID& id);

		//GetResource will return a resource based off of the id given.
		IMythosResource* GetResource(MythosID&);
		void DeleteResource(MythosID&);

		ID3D10Blob* GetShaderBlob(MythosID&);
		void DeleteShaderBlob(MythosID&);
		
		MythosLight* GetLight(MythosID&);
		void DeleteLight(MythosID&);
	private:
		MythosCreator m_Creator;
		MythosContext m_Context;
		MythosSwapChain m_SwapChain;
		D3D11_VIEWPORT m_Viewport;

		//Helper Function
		UINT GetBindFlags(MythosBindFlags flag);
		UINT GetFormat(MythosFormat format);
		UINT GetFormatSize(MythosFormat format);

		IMythosResource* CombineAsMips2D(unsigned int width, unsigned int height, IMythosResource** textures, int numMips);
		IMythosResource* CombineAsMipsCube(unsigned int width, unsigned int height, IMythosResource** textures, int numMips);

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

		std::unordered_map<MythosID, unsigned int> m_IDsToIndex;
		std::vector<std::unordered_map<MythosID, IMythosResource*>> m_Resources;
		std::unordered_map<MythosID, ID3D10Blob*> m_ShaderBlobs;
		std::unordered_map<MythosID, MythosLight*> m_Lights;
	};
}