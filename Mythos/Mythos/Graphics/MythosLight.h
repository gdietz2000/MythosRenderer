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
		MythosLight() { ZeroMemory(this, sizeof(MythosLight)); }

		MythosLight(Math::Vector3& direction, Math::Vector3& color, float intensity)
			: lightType(MythosLightType::MYTHOS_DIRECTIONAL_LIGHT), lightDirection(direction), lightColor(color), lightIntensity(intensity) 
		{
			lightPosition = 0.0f;
			lightRadius = 0.0f;
			lightConeRatio = 0.0f;
			lightInnerConeRatio = 0.0f;
			lightOuterConeRatio = 0.0f;
		}

		MythosLight(Math::Vector3& position, Math::Vector3& color, float intensity, float radius)
			: lightType(MythosLightType::MYTHOS_POINT_LIGHT), lightPosition(position), lightColor(color), lightIntensity(intensity), lightRadius(radius)
		{
			lightDirection = 0.0f;
			lightConeRatio = 0.0f;
			lightInnerConeRatio = 0.0f;
			lightOuterConeRatio = 0.0f;
		}

		MythosLight(Math::Vector3& position, Math::Vector3& direction, Math::Vector3& color, float intensity, float coneRatio, float innerConeRatio, float outerConeRatio)
			: lightType(MythosLightType::MYTHOS_SPOT_LIGHT), lightPosition(position), lightDirection(direction), lightColor(color), lightIntensity(intensity), lightConeRatio(coneRatio), lightInnerConeRatio(innerConeRatio), lightOuterConeRatio(outerConeRatio)
		{
			lightRadius = 0.0f;
		}
	private:
		//Variables that will be available for all lights
		MythosLightType lightType;
		Math::Vector3 lightColor;
		float lightIntensity;
		
		//Directional Lights and Spot Lights
		Math::Vector3 lightDirection;

		//Point Lights only
		Math::Vector3 lightPosition;
		float lightRadius;

		//Spot Lights only
		float lightConeRatio;
		float lightInnerConeRatio;
		float lightOuterConeRatio;
		float buffer = 0.0;
	};
}