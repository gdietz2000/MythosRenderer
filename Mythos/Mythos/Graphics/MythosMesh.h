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
	};

	struct MythosMesh 
	{
		std::vector<MythosVertex> m_Vertices;
		std::vector<int> m_Indices;
	};

	class MythosObject
	{
		
	};
}