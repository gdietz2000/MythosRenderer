#pragma once

#include <vector>
#include "Vector2.h"
#include "Vector3.h"

namespace Mythos
{
	struct MythosVertex
	{
		Math::Vector3 position;
		Math::Vector2 uv;
		Math::Vector3 normals;
	};

	struct MythosMesh 
	{
		std::vector<MythosVertex> m_Vertices;
		std::vector<int> m_Indices;
	};

	struct MythosModel
	{
		~MythosModel() { for (int i = 0; i < m_Meshes.size(); ++i) { delete m_Meshes[i]; } m_Meshes.clear(); }
		std::vector<MythosMesh*> m_Meshes;
	};
}