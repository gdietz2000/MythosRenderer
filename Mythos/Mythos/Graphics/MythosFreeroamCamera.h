#pragma once

#include "IMythosCamera.h"

namespace Mythos
{
	class MythosFreeroamCamera : public IMythosCamera
	{
	public:
		MythosFreeroamCamera() = default;

		//Camera Methods
		void SetCamera(Math::Vector3 position, Math::Vector3 target, Math::Vector3 up);
		void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
		
		void GetCameraInput();

		Math::Vector3 GetPosition();
		void SetPosition(Math::Vector3 newPosition);
		Math::Vector3 GetTargetDirection();
		void SetTargetDirection(Math::Vector3 newTargetDirection);
		Math::Vector3 GetUpDirection();
		void SetUpDirection(Math::Vector3 upDirection);

		//Projection Methods
		float GetFieldOfView();
		void SetFieldOfView(float fov);
		float GetAspectRatio();
		void SetAspectRatio(float aspectRatio);
		float GetNearPlane();
		void SetNearPlane(float nearPlane);
		float GetFarPlane();
		void SetFarPlane(float farPlane);
	private:
		void UpdateCamera();
		void UpdateProjection();

		//Camera Matrix
		float m_PitchClamp = 0;

		Math::Vector3 m_CameraPosition = Math::Vector3(0.0f);
		Math::Vector3 m_TargetDirection = Math::Vector3(0.0f);
		Math::Vector3 m_UpDirection = Math::Vector3(0.0f);

		//Perspective Projection Matrix
		float m_FieldOfView = 0;
		float m_AspectRatio = 0;
		float m_NearPlane = 0;
		float m_FarPlane = 0;
	};
}