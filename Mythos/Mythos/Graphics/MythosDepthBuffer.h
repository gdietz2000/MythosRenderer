#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosDepthBuffer : public IMythosResource
	{
	public:
		MythosDepthBuffer() = default;
		virtual void*& GetData() override { return (void*&)m_DepthBuffer; }
		virtual void SafeRelease() override { if (m_DepthBuffer) { m_DepthBuffer->Release(); m_DepthBuffer = nullptr; } }
	private:
		ID3D11DepthStencilView* m_DepthBuffer;
	};
}