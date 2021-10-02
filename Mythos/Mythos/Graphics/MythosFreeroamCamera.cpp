#include <Windows.h>
#include "MythosFreeroamCamera.h"
namespace Mythos
{
	void MythosFreeroamCamera::SetCamera(Math::Vector3 position, Math::Vector3 target, Math::Vector3 up) {
		m_CameraPosition = position;
		m_TargetDirection = target.Normalize();
		m_UpDirection = up.Normalize();
		m_Camera = Math::Matrix4::LookAtLH(position, target, up);
	}

	void MythosFreeroamCamera::SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		m_FieldOfView = fov;
		m_AspectRatio = aspectRatio;
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;
		m_Projection = Math::Matrix4::PerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	}

	void MythosFreeroamCamera::GetCameraInput(float deltaTime)
	{
		float LocalX = 0, LocalZ = 0, GlobalY = 0, camYaw = 0, camPitch = 0;
		//Movement Inputs
		if (GetAsyncKeyState(0x41))
		{
			LocalX -= 0.1f;
		}

		if (GetAsyncKeyState(0x44))
		{
			LocalX += 0.1f;
		}

		if (GetAsyncKeyState(VK_SHIFT))
		{
			GlobalY -= 0.1f;
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			GlobalY += 0.1f;
		}

		if (GetAsyncKeyState(0x53))
		{
			LocalZ -= 0.1f;
		}

		if (GetAsyncKeyState(0x57))
		{
			LocalZ += 0.1f;
		}

		if (GetAsyncKeyState(VK_UP))
		{
			if (m_PitchClamp > -1.4f)
			{
				camPitch -= 0.005f;
				m_PitchClamp -= 0.005f;
			}
		}

		if (GetAsyncKeyState(VK_DOWN))
		{
			if (m_PitchClamp < 1.4f)
			{
				camPitch += 0.002f;
				m_PitchClamp += 0.002f;
			}
		}

		if (GetAsyncKeyState(VK_LEFT))
		{
			camYaw -= 0.002f;
		}

		if (GetAsyncKeyState(VK_RIGHT))
		{
			camYaw += 0.002f;
		}

		camPitch *= deltaTime;
		camYaw *= deltaTime;

		m_Camera = m_Camera.Inverse();

		Math::Matrix4 temp = Math::Matrix4::Translate(LocalX, 0, LocalZ);
		m_Camera = temp * m_Camera;
		temp = Math::Matrix4::Translate(0, GlobalY, 0);
		m_Camera = m_Camera * temp;

		Math::Vector3 tempPosition = m_Camera.GetPosition();

		m_Camera.SetPosition({ 0,0,0 });

		temp = Math::Matrix4::RotateY(camYaw);
		m_Camera = m_Camera * temp;
		m_Camera.SetPosition(tempPosition);
		m_Camera = Math::Matrix4::RotateX(camPitch) * m_Camera;

		m_Camera.SetPosition(tempPosition);

		m_CameraPosition = tempPosition;

		m_Camera = m_Camera.Inverse();
	}

	Math::Vector3 MythosFreeroamCamera::GetPosition() 
	{
		return m_CameraPosition;
	}

	void MythosFreeroamCamera::SetPosition(Math::Vector3 newPosition)
	{
		m_CameraPosition = newPosition;
		UpdateCamera();
	}

	Math::Vector3 MythosFreeroamCamera::GetTargetDirection()
	{
		return m_TargetDirection;
	}

	void MythosFreeroamCamera::SetTargetDirection(Math::Vector3 targetDirection)
	{
		m_TargetDirection = targetDirection;
		UpdateCamera();
	}

	Math::Vector3 MythosFreeroamCamera::GetUpDirection()
	{
		return m_UpDirection;
	}

	void MythosFreeroamCamera::SetUpDirection(Math::Vector3 updirection)
	{
		m_UpDirection = updirection;
		UpdateCamera();
	}

	float MythosFreeroamCamera::GetFieldOfView()
	{
		return m_FieldOfView;
	}

	void MythosFreeroamCamera::SetFieldOfView(float fov)
	{
		m_FieldOfView = fov;
		UpdateProjection();
	}

	float MythosFreeroamCamera::GetAspectRatio() 
	{
		return m_AspectRatio;
	}

	void MythosFreeroamCamera::SetAspectRatio(float aspectRatio) 
	{
		m_AspectRatio = aspectRatio;
		UpdateProjection();
	}

	float MythosFreeroamCamera::GetNearPlane() 
	{
		return m_NearPlane;
	}

	void MythosFreeroamCamera::SetNearPlane(float nearPlane)
	{
		m_NearPlane = nearPlane;
		UpdateProjection();
	}

	float MythosFreeroamCamera::GetFarPlane()
	{
		return m_FarPlane;
	}

	void MythosFreeroamCamera::SetFarPlane(float farPlane)
	{
		m_FarPlane = farPlane;
		UpdateProjection();
	}

	void MythosFreeroamCamera::UpdateCamera()
	{
		m_Camera = Math::Matrix4::LookAtLH(m_CameraPosition, m_TargetDirection, m_UpDirection);
	}

	void MythosFreeroamCamera::UpdateProjection()
	{
		m_Projection = Math::Matrix4::PerspectiveFovLH(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
	}

}