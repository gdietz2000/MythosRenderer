#pragma once

#include "Vector3.h"
#include "Matrix4.h"

namespace Mythos
{

	class IMythosCamera
	{
	public:
		IMythosCamera() { m_Camera = Math::Matrix4::Identity; m_Projection = Math::Matrix4::Identity; }
		IMythosCamera(Math::Matrix4 newCamera, Math::Matrix4 newProjection) { m_Camera = newCamera; m_Projection = newProjection; }

		inline const Math::Matrix4& GetCamera() { return m_Camera; }
		inline const Math::Matrix4& GetProjection() { return m_Projection; }

		inline void SetCamera(Math::Matrix4 newMatrix) { m_Camera = newMatrix; }
		inline void SetProjection(Math::Matrix4 newMatrix) { m_Projection = newMatrix; }
	protected:
		Math::Matrix4 m_Camera;
		Math::Matrix4 m_Projection;
	};

}