#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos {

	class MythosVertexShader : public IMythosResource
	{
	public:
		MythosVertexShader() = default;
		virtual void*& GetData() override { return (void*&)m_Shader; }
		virtual void SafeRelease() { if (m_Shader) { m_Shader->Release(); m_Shader = nullptr; } }
	private:
		ID3D11VertexShader* m_Shader = nullptr;
	};
}