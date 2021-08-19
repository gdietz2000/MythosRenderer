#pragma once

namespace Mythos 
{
	//All enums that will need to created
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

	struct IMythosResource
	{
	public:
		IMythosResource() = default;
		inline virtual void*& GetData() = 0;
		inline virtual void SafeRelease() = 0;
	protected:
	};
}