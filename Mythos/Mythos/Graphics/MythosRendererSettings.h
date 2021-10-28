#pragma once

#include "MythosDescriptors.h"
#include "MythosID.h"

namespace Mythos
{
	struct MythosRendererSettings
	{
		MythosTopology m_Topology = MYTHOS_TRIANGLE_LIST;
		MythosID m_InputLayout = 0;
		MythosID m_RasterState = 0;
		MythosID m_TextureSampler = 0;
	};
}