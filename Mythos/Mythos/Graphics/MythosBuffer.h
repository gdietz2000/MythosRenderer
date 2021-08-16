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
		MythosBuffer(void* data, unsigned int byteSize, MythosContext* context) { m_Data = data; m_ByteSize = byteSize; }
		virtual void*& GetData() { return (void*&)m_Buffer; }
		virtual void SafeRelease() { if (m_Buffer) m_Buffer->Release(); }
	private:
		void* m_Data = nullptr;
		unsigned int m_ByteSize = 0;
		ID3D11Buffer* m_Buffer = nullptr;
	};
}