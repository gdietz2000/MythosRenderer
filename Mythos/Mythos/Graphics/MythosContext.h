#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")

namespace Mythos
{
	class MythosContext
	{
	public:
		inline ID3D11DeviceContext*& GetContext() { return m_Context; }
		void SafeRelease() { if (m_Context) m_Context->Release(); }
	private:
		ID3D11DeviceContext* m_Context = nullptr;
	};
}