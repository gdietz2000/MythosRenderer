#pragma once

namespace Mythos
{
	//Required for usage type and accessability
	enum MythosAccessability
	{
		MYTHOS_DEFAULT_ACCESS,
		MYTHOS_DYNAMIC_ACCESS,
		MYTHOS_IMMUTABLE_ACCESS,
	};

	//Buffer Descriptor
	struct MythosBufferDescriptor
	{
		void* data;
		unsigned int byteSize;
		MythosAccessability cpuAccess;
	};

	//Depth Buffer Descriptor
	struct MythosDepthBufferDescriptor 
	{

	};

	//Render Target Descriptor
	struct MythosRenderTargetDescriptor
	{
		
	};

	//Input Layout Elements Descriptor
	struct MythosInputElement
	{
		const char* m_SemanticName;
		unsigned int m_SemanticIndex;
		unsigned int m_Format;
	};

	//Rasterizer Descriptor
	struct MythosRasterizerDescriptor
	{
		
	};

	//Texture Descriptor
	struct MythosTextureDescriptor
	{
		
	};

	//Shader Resource Descriptor
	struct MythosShaderResourceDescriptor
	{

	};

	//Texture Sampler Descriptor
	struct MythosSamplerDescriptor
	{
		
	};
}