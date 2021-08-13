#pragma once

namespace Mythos 
{
	struct IMythosResource
	{
	public:
		IMythosResource() = default;
		virtual void*& GetData() = 0;
		inline unsigned int GetID() { return m_ID; }
		virtual void SafeRelease() = 0;
	protected:
		unsigned int m_ID = 0;
	};
}