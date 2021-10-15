#pragma once

#include "Vector3.h"

namespace Mythos
{
	enum class MythosLightType : int
	{
		MYTHOS_NONE = 0, 
		MYTHOS_DIRECTIONAL_LIGHT,
		MYTHOS_POINT_LIGHT,
		MYTHOS_SPOT_LIGHT
	};

	class MythosLight 
	{
	public:
		MythosLight(MythosLightType type, Math::Vector3& direction, Math::Vector3& color, float intensity)
			: lightType(type), lightDirection(direction), lightColor(color), lightIntensity(intensity) 
		{
			lightPosition = 0.0f;
			lightRadius = 0.0f;
		}

		MythosLight(MythosLightType type, Math::Vector3& position, Math::Vector3& color, float intensity, float radius)
			: lightType(type), lightPosition(position), lightColor(color), lightIntensity(intensity), lightRadius(radius) 
		{
			lightDirection = 0.0f;
		}
	private:
		//Variables that will be available for all lights
		MythosLightType lightType;
		Math::Vector3 lightColor;
		float lightIntensity;
		
		//Directional Lights only
		Math::Vector3 lightDirection;

		//Point Lights only
		Math::Vector3 lightPosition;
		float lightRadius;
	};
}