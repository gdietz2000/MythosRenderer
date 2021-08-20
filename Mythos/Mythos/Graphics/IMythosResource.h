#pragma once

namespace Mythos 
{
	struct IMythosResource
	{
	public:
		IMythosResource() = default;
		inline virtual void*& GetData() = 0;
		inline virtual void SafeRelease() = 0;
	protected:
	};
}