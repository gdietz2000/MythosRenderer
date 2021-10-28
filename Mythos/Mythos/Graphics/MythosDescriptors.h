#pragma once

namespace Mythos
{
#define BIT(x) 1 << x

	//All enums that will need to created
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

	enum MythosTopology
	{
		MYTHOS_LINE_LIST,
		MYTHOS_TRIANGLE_LIST
	};

	enum MythosFillMode
	{
		MYTHOS_FILL_WIREFRAME = 0,
		MYTHOS_FILL_SOLID
	};

	enum MythosCullMode
	{
		MYTHOS_CULL_NONE = 0,
		MYTHOS_CULL_FRONT,
		MYTHOS_CULL_BACK,
	};

	enum MythosAddressMode {
		MYTHOS_TEXTURE_ADDRESS_WRAP,
		MYTHOS_TEXTURE_ADDRESS_BORDER,
		MYTHOS_TEXTURE_ADDRESS_CLAMP,
		MYTHOS_TEXTURE_ADDRESS_MIRROR
	};

	enum MythosFormat
	{
		MYTHOS_FORMAT_32_FLOAT1,
		MYTHOS_FORMAT_32_FLOAT2,
		MYTHOS_FORMAT_32_FLOAT3,
		MYTHOS_FORMAT_32_FLOAT4,
		MYTHOS_FORMAT_32_SINT1,
		MYTHOS_FORMAT_32_SINT2,
		MYTHOS_FORMAT_32_SINT3,
		MYTHOS_FORMAT_32_SINT4,
		MYTHOS_FORMAT_32_UINT1,
		MYTHOS_FORMAT_32_UINT2,
		MYTHOS_FORMAT_32_UINT3,
		MYTHOS_FORMAT_32_UINT4,
		MYTHOS_FORMAT_32_UNKNOWN1,
		MYTHOS_FORMAT_32_UNKNOWN2,
		MYTHOS_FORMAT_32_UNKNOWN3,
		MYTHOS_FORMAT_32_UNKNOWN4,
		MYTHOS_FORMAT_32_DEPTH,
		MYTHOS_FORMAT_8_FLOAT4,
	};

	//Required for usage type and accessability
	enum MythosAccessability
	{
		MYTHOS_DEFAULT_ACCESS,
		MYTHOS_DYNAMIC_ACCESS,
		MYTHOS_IMMUTABLE_ACCESS,
	};

	enum MythosBindFlags
	{
		MYTHOS_BIND_NONE = BIT(0),
		MYTHOS_BIND_DEPTH = BIT(1),
		MYTHOS_BIND_RENDER_TARGET = BIT(2),
		MYTHOS_BIND_SHADER_RESOURCE = BIT(3),
	};

	//Buffer Descriptor
	struct MythosBufferDescriptor
	{
		void* data;
		unsigned int byteSize;
		MythosAccessability cpuAccess;
	};

	//Render Target Descriptor
	struct MythosRenderTargetDescriptor
	{
		unsigned int MipSlice;
		MythosFormat format;
	};

	//Input Layout Elements Descriptor
	struct MythosInputElement
	{
		const char* SemanticName;
		unsigned int SemanticIndex;
		MythosFormat Format;
	};

	//Rasterizer Descriptor
	struct MythosRasterizerDescriptor
	{
		bool AntialiasLineEnabled;
		MythosCullMode CullMode;
		unsigned int DepthBias;
		unsigned int DepthBiasClamp;
		bool DepthClipEnable;
		MythosFillMode FillMode;
		int FrontCounterClockwise;
		bool MultiSampleEnable;
		bool ScissorEnable;
		float SlopeScaledDepthBias;
	};

	//Texture Descriptor
	struct MythosTextureDescriptor
	{
		unsigned int width;
		unsigned int height;
		unsigned int mipLevels;
		unsigned int sampleCount;
		unsigned int sampleQuality;
		unsigned int bindFlags;
		MythosFormat format;
		MythosAccessability cpuAccess;
	};

	//Texture Sampler Descriptor
	struct MythosSamplerDescriptor
	{
		MythosAddressMode AddressMode;
		Math::Vector4 BorderColor;
		unsigned int MaxAnisotropy;
		float MaxLOD;
		float MinLOD;
		float MipLODBias;
	};
}