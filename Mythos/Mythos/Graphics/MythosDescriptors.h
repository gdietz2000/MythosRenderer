#pragma once

namespace Mythos
{
#define BIT(x) 1 << x

	//All enums that will need to created
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
		MYTHOS_FORMAT_32_INT1,
		MYTHOS_FORMAT_32_INT2,
		MYTHOS_FORMAT_32_INT3,
		MYTHOS_FORMAT_32_INT4,
		MYTHOS_FORMAT_32_UNKNOWN1,
		MYTHOS_FORMAT_32_UNKNOWN2,
		MYTHOS_FORMAT_32_UNKNOWN3,
		MYTHOS_FORMAT_32_UNKNOWN4,
		MYTHOS_FORMAT_32_DEPTH
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

	//Shader Resource Descriptor
	struct MythosShaderResourceDescriptor
	{
		MythosFormat Format;
		unsigned int MipLevels;
		unsigned int MostDetailedMip;
	};

	//Texture Sampler Descriptor
	struct MythosSamplerDescriptor
	{
		MythosAddressMode AddressMode;
		Math::Vector4 BorderColor;
		unsigned int MaxAnisotropy;
		unsigned int MaxLOD;
		unsigned int MinLOD;
		float MipLODBias;
	};
}