#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosBuffer : public IMythosResource
	{
	public:
		MythosBuffer() = default;
		inline virtual void*& GetData() { return (void*&)m_Buffer; }
		inline virtual void SafeRelease() { if (m_Buffer) m_Buffer->Release(); }
	private:
		ID3D11Buffer* m_Buffer = nullptr;
	};
}