#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")

namespace Mythos
{
	class MythosCreator
	{
	public:
		inline ID3D11Device*& GetCreator() { return m_Creator; }
		void SafeRelease() { if (m_Creator) m_Creator->Release(); }
	private:
		ID3D11Device* m_Creator = nullptr;
	};
}