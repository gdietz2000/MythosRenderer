#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos 
{
	class MythosRenderTarget : public IMythosResource
	{
	public:
		MythosRenderTarget() = default;
		inline virtual void*& GetData() override { return (void*&)m_RenderTarget; }
		inline virtual void SafeRelease() override { if (m_RenderTarget) { m_RenderTarget->Release(); m_RenderTarget = nullptr; } }
	private:
		ID3D11RenderTargetView* m_RenderTarget = nullptr;
	};
}