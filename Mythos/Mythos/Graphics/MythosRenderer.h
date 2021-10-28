#pragma once

#include "Vector4.h"
#include "MythosContext.h"
#include "MythosLight.h"
#include "IMythosResource.h"
#include "MythosRendererSettings.h"

#include <unordered_map>
#include <vector>

namespace Mythos
{
	class MythosRenderer
	{
	public:
		MythosRenderer();
		MythosRenderer(MythosContext*);

		void CleanUp();

		//Setters
		void SetDepthBufferClearValue(float clearValue);
		void SetRenderTargetClearColor(Math::Vector4 clearColor);
		void SetRendererSettings(MythosRendererSettings settings);

		//Getters
		inline float GetDepthBufferClearValue() { return m_DepthBufferClearValue; }
		inline Math::Vector4 GetRenderTargetClearColor() { return m_RenderTargetClearColor; }
		inline MythosRendererSettings GetRendererSettings() { return m_Settings; }

		void AddMythosResource(MythosID id, unsigned int index, IMythosResource* resource);
		void AddShaderBlob(MythosID id, ID3D10Blob*);
		void AddLight(MythosID id, MythosLight* light);

		IMythosResource* GetResource(MythosID& id);
		void DeleteResource(MythosID& id);
		ID3D10Blob* GetShaderBlob(MythosID& id);
		void DeleteShaderBlob(MythosID& id);
		MythosLight* GetLight(MythosID& id);
		void DeleteLight(MythosID& id);

		//Rendering
		void ApplySettings();
	private:
		void SetTopology(MythosTopology topo);


		MythosContext* m_Context;

		MythosRendererSettings m_Settings;

		Math::Vector4 m_RenderTargetClearColor;
		float m_DepthBufferClearValue;

		//Shaders
		std::unordered_map<MythosID, ID3D10Blob*> m_ShaderBlobs;
		std::unordered_map<MythosID, MythosLight*> m_Lights;

		//Resources
		std::unordered_map<MythosID, unsigned int> m_IDsToIndex;
		std::vector<std::unordered_map<MythosID, IMythosResource*>> m_Resources;
	};

}