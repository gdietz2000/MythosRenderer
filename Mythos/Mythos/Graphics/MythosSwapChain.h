#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")

namespace Mythos
{
	class MythosSwapChain
	{
	public:
		inline IDXGISwapChain*& GetSwapChain() { return m_SwapChain; }
		void SafeRelease() { if (m_SwapChain) m_SwapChain->Release(); }
	private:
		IDXGISwapChain* m_SwapChain = nullptr;
	};
}