#include "../include/Rng.h"

#include "../include/Vector2.h"
#include "../include/Vector3.h"

namespace Math
{
	Rng::Rng()
	{
		distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
		generator.seed(m_Seed);
	}

	Rng::Rng(unsigned int seed)
	{
		distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
		generator.seed(seed);
		m_Seed = seed;
	}

	void Rng::Reset()
	{
		generator.seed(++m_Seed);
	}

	void Rng::Seed(unsigned int seed)
	{
		generator.seed(seed);
		m_Seed = seed;
	}

	float Rng::RandomFloat()
	{
		return distribution(generator);
	}

	float Rng::RandomFloat(float min, float max) {
		return min + (max - min) * distribution(generator);
	}

	Vector2 Rng::RandomVector2()
	{
		return Vector2(RandomFloat(), RandomFloat());
	}

	Vector2 Rng::RandomVector2(float min, float max)
	{
		return Vector2(RandomFloat(min, max), RandomFloat(min, max));
	}

	Vector2 Rng::RandomVector2(const Vector2& min, const Vector2& max)
	{
		return Vector2(RandomFloat(min.x, max.x), RandomFloat(min.y, max.y));
	}

	Vector3 Rng::RandomVector3()
	{
		return Vector3(RandomFloat(), RandomFloat(), RandomFloat());
	}

	Vector3 Rng::RandomVector3(float min, float max)
	{
		return Vector3(RandomVector2(min, max), RandomFloat(min, max));
	}

	Vector3 Rng::RandomVector3(const Vector3& min, const Vector3& max)
	{
		return Vector3(RandomFloat(min.x, max.x), RandomFloat(min.y, max.y), RandomFloat(min.z, max.z));
	}

	Vector2 Rng::InUnitCircle()
	{
		while(true)
		{
			auto p = RandomVector2(-1, 1);
			if (p.MagnitudeSq() >= 1)
				continue;

			return p;
		}
	}

	Vector3 Rng::InUnitSphere()
	{
		while (true) 
		{
			auto p = RandomVector3(-1, 1);
			if (p.MagnitudeSq() >= 1)
				continue;

			return p;
		}
	}

	Vector3 Rng::InHemisphere(const Vector3& normal)
	{
		auto inUnitSphere = InUnitSphere();
		if (inUnitSphere.Dot(normal) > 0.0f)
			return inUnitSphere;
		else
			return -inUnitSphere;
	}
}