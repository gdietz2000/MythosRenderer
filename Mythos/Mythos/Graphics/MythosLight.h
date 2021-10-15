#pragma once

#include "Vector3.h"

namespace Mythos
{
	enum class MythosLightType : int
	{
		MYTHOS_DIRECTIONAL_LIGHT = 1,
		MYTHOS_POINT_LIGHT,
		MYTHOS_SPOT_LIGHT
	};

	class MythosLight 
	{
	public:
		MythosLight(MythosLightType type, Math::Vector3& direction, Math::Vector3& color, float intensity)
			: lightType(type), lightDirection(direction), lightColor(color), lightIntensity(intensity) {}
	private:
		//Variables that will be available for all lights
		MythosLightType lightType;
		Math::Vector3 lightColor;
		float lightIntensity;
		
		//Directional Lights only
		Math::Vector3 lightDirection;
	};
}