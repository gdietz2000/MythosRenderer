#pragma once

#include "MythosID.h"

namespace Mythos
{
	struct MythosMaterial 
	{
		MythosMaterial()
			: m_Diffuse(NULL), m_AmbientOcculusion(NULL), m_Normal(NULL), m_Metallic(NULL), m_Roughness(NULL), m_Emissive(NULL) {};

		MythosMaterial(MythosID diffuse, MythosID ambientOcculusion, MythosID normal, MythosID metallic, MythosID roughness, MythosID emissive)
			: m_Diffuse(diffuse), m_AmbientOcculusion(ambientOcculusion), m_Normal(normal), m_Metallic(metallic), m_Roughness(roughness), m_Emissive(emissive) {};
		
		MythosID m_Diffuse;
		MythosID m_AmbientOcculusion;
		MythosID m_Normal;
		MythosID m_Metallic;
		MythosID m_Roughness;
		MythosID m_Emissive;
	};
}