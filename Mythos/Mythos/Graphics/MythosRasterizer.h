#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosRasterizer : public IMythosResource
	{
	public:
		MythosRasterizer() = default;
		inline virtual void*& GetData() override { return (void*&)m_RasterState; }
		inline virtual void SafeRelease() override { if (m_RasterState) { m_RasterState->Release(); m_RasterState = nullptr; } }
	private:
		ID3D11RasterizerState* m_RasterState;
	};
}