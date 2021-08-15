#pragma once

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")
#include "IMythosResource.h"

namespace Mythos {

	class MythosPixelShader : public IMythosResource
	{
	public:
		MythosPixelShader() = default;
		MythosPixelShader(unsigned int id, const wchar_t* filePath, const char* entryPoint, const char* modelType)
		{
			m_ID = id;  m_ShaderFilePath = filePath; m_ShaderEntryPoint = entryPoint; m_ShaderModelType = modelType;
		}
		virtual void*& GetData() override { return (void*&)m_Shader; }
		inline unsigned int GetID() { return m_ID; }
		virtual void SafeRelease() { if (m_Shader) { m_Shader->Release(); m_Shader = nullptr; } }
	private:
		const wchar_t* m_ShaderFilePath;
		const char* m_ShaderEntryPoint, * m_ShaderModelType;
		ID3D11PixelShader* m_Shader = nullptr;
	};
}