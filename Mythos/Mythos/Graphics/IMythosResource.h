#pragma once

namespace Mythos 
{
	struct IMythosResource
	{
	public:
		IMythosResource() = default;
		virtual void*& GetData() = 0;
		virtual void SafeRelease() = 0;
	protected:
	};
}