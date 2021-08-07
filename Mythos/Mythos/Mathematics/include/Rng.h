#pragma once

#include "Mathematics.h"
#include <random>

namespace Math
{
	struct Rng
	{
	public:
		Rng();
		Rng(unsigned int seed);

		void Reset();
		void Seed(unsigned int seed);

		/*Returns a random float between 0.0f and 1.0f*/
		inline float RandomFloat();

		/*Returns a random float between the min and the max*/
		inline float RandomFloat(float min, float max);

		/*Returns a random vector*/
		inline Vector2 RandomVector2();

		/*Returns a random vector with the values ranging from min and max*/
		inline Vector2 RandomVector2(float min, float max);

		/*
		Returns a random vector with the values ranging from the min and max
		
		@param min The min X and Y the random value is allowed
		@param max the max X and Y the random value is allowed
		*/
		inline Vector2 RandomVector2(const Vector2& min, const Vector2& max);

		/*Returns a random vector*/
		inline Vector3 RandomVector3();

		/*Returns a random vector with the values ranging from min and max*/
		inline Vector3 RandomVector3(float min, float max);

		/*
		Returns a random vector with the values ranging from min and max
		
		@param min The min X, Y and Z the random value is allowed
		@param max The max X, Y and Z the random value is allowed
		*/
		inline Vector3 RandomVector3(const Vector3& min, const Vector3& max);

		/*Returns a random value within the Unit Circle*/
		inline Vector2 InUnitCircle();

		/*Returns a random value withing the Unit Sphere*/
		inline Vector3 InUnitSphere();

		/*
		Returns a random value within a hemisphere based off the normal

		@param normal The normal that influences the area where random will land
		*/
		inline Vector3 InHemisphere(const Vector3& normal);

	private:
		int m_Seed = 0;
		std::mt19937 generator;
		std::uniform_real_distribution<float> distribution;
	};
}
