#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos
{
	class MythosInputLayout : public IMythosResource
	{
	public:
		MythosInputLayout() = default;
		inline virtual void*& GetData() override { return (void*&)m_InputLayout; }
		inline virtual void SafeRelease() override { if (m_InputLayout) { m_InputLayout->Release(); m_InputLayout = nullptr; } }
	private:
		ID3D11InputLayout* m_InputLayout;
	};
}