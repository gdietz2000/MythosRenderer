#include "MythosRenderer.h"
namespace Mythos {

	MythosRenderer::MythosRenderer()
	{
		m_Context = nullptr;
		m_RenderTargetClearColor = Math::Vector4(0);
		m_DepthBufferClearValue = 1.0f;
	}

	MythosRenderer::MythosRenderer(MythosContext* context)
	{
		m_Context = context;
		m_RenderTargetClearColor = Math::Vector4(0);
		m_DepthBufferClearValue = 1.0f;

		for (int i = 0; i < MYTHOS_RESOURCE_COUNT; ++i)
		{
			m_Resources.push_back(std::unordered_map<MythosID, IMythosResource*>());
		}
	}

	void MythosRenderer::CleanUp()
	{
		for (int i = 0; i < MYTHOS_RESOURCE_COUNT; ++i)
		{
			for (auto iter : m_Resources[i])
			{
				//Ensuring Memory is Deallocated Properly
				iter.second->SafeRelease();
				delete iter.second;
			}
		}

		for (auto blob : m_ShaderBlobs)
		{
			blob.second->Release();
		}

		for (auto light : m_Lights)
		{
			delete light.second;
		}

		//Clearing these containers
		m_IDsToIndex.clear();
		m_Resources.clear();
		m_ShaderBlobs.clear();
		m_Lights.clear();
	}

	void MythosRenderer::SetDepthBufferClearValue(float clearValue)
	{
		m_DepthBufferClearValue = clearValue;
	}

	void MythosRenderer::SetRenderTargetClearColor(Math::Vector4 clearColor)
	{
		m_RenderTargetClearColor = clearColor;
	}

	void MythosRenderer::SetRendererSettings(MythosRendererSettings settings)
	{
		m_Settings = settings;
	}

	void MythosRenderer::AddMythosResource(MythosID id, unsigned int index, IMythosResource* resource)
	{
		m_IDsToIndex.insert(std::make_pair(id, index));
		m_Resources[index].insert(std::make_pair(id, resource));
	}

	void MythosRenderer::AddShaderBlob(MythosID id, ID3D10Blob* blob)
	{
		m_ShaderBlobs.insert(std::make_pair(id, blob));
	}

	void MythosRenderer::AddLight(MythosID id, MythosLight* light)
	{
		m_Lights.insert(std::make_pair(id, light));
	}

	IMythosResource* MythosRenderer::GetResource(MythosID& id)
	{
		if (id == NULL)
			return nullptr;

		auto num = m_IDsToIndex.find(id);
		if (num != m_IDsToIndex.end())
		{
			unsigned int arrayIndex = num->second;
			auto iter = m_Resources[arrayIndex].find(id);
			if (iter != m_Resources[arrayIndex].end())
			{
				return iter->second;
			}
		}

		return nullptr;
	}

	void MythosRenderer::DeleteResource(MythosID& id)
	{
		auto num = m_IDsToIndex.find(id);
		if (num != m_IDsToIndex.end())
		{
			unsigned int arrayIndex = num->second;
			auto iter = m_Resources[arrayIndex].find(id);
			if (iter != m_Resources[arrayIndex].end())
			{
				iter->second->SafeRelease();
				delete iter->second;
				m_Resources[arrayIndex].erase(id);
				m_IDsToIndex.erase(id);
			}
		}
	}

	ID3D10Blob* MythosRenderer::GetShaderBlob(MythosID& id)
	{
		auto iter = m_ShaderBlobs.find(id);
		if (iter != m_ShaderBlobs.end())
			return iter->second;

		return nullptr;
	}

	void MythosRenderer::DeleteShaderBlob(MythosID& id)
	{
		auto iter = m_ShaderBlobs.find(id);
		if (iter != m_ShaderBlobs.end())
		{
			iter->second->Release();
			m_ShaderBlobs.erase(id);
		}
	}

	MythosLight* MythosRenderer::GetLight(MythosID& id)
	{
		auto iter = m_Lights.find(id);
		if (iter != m_Lights.end())
			return iter->second;

		return nullptr;
	}

	void MythosRenderer::DeleteLight(MythosID& id)
	{
		auto iter = m_Lights.find(id);
		if (iter != m_Lights.end())
		{
			delete iter->second;
			m_Lights.erase(id);
		}
	}

	void MythosRenderer::ApplySettings()
	{
		SetTopology(m_Settings.m_Topology);
		if (m_Settings.m_InputLayout)
			m_Context->GetContext()->IASetInputLayout((ID3D11InputLayout*)GetResource(m_Settings.m_InputLayout)->GetData());
		if (m_Settings.m_RasterState)
			m_Context->GetContext()->RSSetState((ID3D11RasterizerState*)GetResource(m_Settings.m_RasterState)->GetData());
		if (m_Settings.m_TextureSampler)
			m_Context->GetContext()->PSSetSamplers(0, 1, (ID3D11SamplerState**)&GetResource(m_Settings.m_TextureSampler)->GetData());
	}

	//Private Methods
	void MythosRenderer::SetTopology(MythosTopology topo)
	{
		if (topo == MYTHOS_LINE_LIST)
			m_Context->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		if (topo == MYTHOS_TRIANGLE_LIST)
			m_Context->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

}