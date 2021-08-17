#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{

	class MythosShaderResource : public IMythosResource
	{
	public:
		MythosShaderResource() = default;
		inline virtual void*& GetData() override { return (void*&)m_ShaderResource; }
		inline virtual void SafeRelease() override { if (m_ShaderResource) { m_ShaderResource->Release(); m_ShaderResource = nullptr; } }
	private:
		ID3D11ShaderResourceView* m_ShaderResource = nullptr;
	};
}