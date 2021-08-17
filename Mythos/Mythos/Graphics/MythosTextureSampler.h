#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosTextureSampler : public IMythosResource
	{
	public:
		inline virtual void*& GetData() override { return (void*&)m_Sampler; }
		inline virtual void SafeRelease() override { if (m_Sampler) { m_Sampler->Release(); m_Sampler = nullptr; } }
	private:
		ID3D11SamplerState* m_Sampler = nullptr;
	};
}