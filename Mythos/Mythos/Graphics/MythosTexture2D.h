#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosTexture2D : public IMythosResource
	{
	public:
		MythosTexture2D() = default;
		MythosTexture2D(void* data, unsigned int sizeInBytes) { m_Data = data; m_ByteSize = sizeInBytes; }
		virtual void*& GetData() override { return (void*&)m_Texture; }
		virtual void SafeRelease() override { if (m_Texture) { m_Texture->Release(); m_Texture = nullptr; } }
	private:
		void* m_Data = nullptr;
		unsigned int m_ByteSize = 0;
		ID3D11Texture2D* m_Texture = nullptr;
	};
}