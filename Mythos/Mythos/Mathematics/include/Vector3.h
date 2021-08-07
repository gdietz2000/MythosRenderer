#pragma once

#include "Mathematics.h"

namespace Math
{
	struct Vector3 
	{
		//Properties
		union {
			float comp[3];
			struct {
				float x, y, z;
			};
		};

		//Constructors
		Vector3();
		Vector3(float);
		Vector3(float, float, float);
		Vector3(float*);
		Vector3(const float*);
		Vector3(const Vector2&, float);
		Vector3(const Vector3&) = default;

		//Right hand side operators
		Vector3 operator+(float);
		Vector3 operator+(float) const;
		Vector3 operator-(float);
		Vector3 operator-(float) const;
		Vector3 operator*(float);
		Vector3 operator*(float) const;
		Vector3 operator/(float);
		Vector3 operator/(float) const;

		Vector3 operator+(const Vector3&);
		Vector3 operator+(const Vector3&) const;
		Vector3 operator-(const Vector3&);
		Vector3 operator-(const Vector3&) const;
		Vector3 operator*(const Vector3&);
		Vector3 operator*(const Vector3&) const;
		Vector3 operator/(const Vector3&);
		Vector3 operator/(const Vector3&) const;

		Vector3& operator=(const Vector3&) = default;
		float operator[](int index);
		float operator[](int index) const;

		bool operator==(float) const;
		bool operator!=(float) const;
		bool operator==(const Vector3&) const;
		bool operator!=(const Vector3&) const;

		Vector3 operator += (float);
		Vector3 operator += (const Vector3&);
		Vector3 operator -= (float);
		Vector3 operator -= (const Vector3&);
		Vector3 operator *= (float);
		Vector3 operator *= (const Vector3&);
		Vector3 operator /= (float);
		Vector3 operator /= (const Vector3&);

		//Negates the values of the affected vector
		Vector3 operator-() const;

		float* data();

		//Returns the length of the vector
		float Magnitude();

		//Returns the length of the vector squared
		float MagnitudeSq();

		/*Returns the length of the vector using the inv_sqrt function
		Faster than the normal Magnitude function and is good enough for most cases*/
		float MagnitudeFast();

		/*Returns one over the length of the vector using the inv_sqrt function
		Uses no division. Faster than the normal Magnitude function and is good enough for most cases*/
		float MagnitudeSuper();

		//Returns the largest value in the vector
		float Major();

		//Returns the smallest value in the vector
		float Minor();

		/*Returns the dot product of current and other vector
		@param other The other vector needed to product the dot product*/
		float Dot(const Vector3& other) const;

		/*Returns a normalized vector*/
		Vector3 Normalize();

		/*Returns a normalized vector using fast magnitude function*/
		Vector3 NormalizeFast();

		/*Returns a normalized vector using super magnitude function*/
		Vector3 NormalizeSuper();
		
		/*
		Returns the cross product of current and other vector
		
		@param other The other vector needed to determine cross product
		*/
		Vector3 Cross(const Vector3& other) const;

		/*
		Returns a reflection of this vector based off a normal

		Assumes this vector is the incident

		@param normal The normal vector to influence the reflection, must be normalized
		*/
		Vector3 Reflection(const Vector3& normal);

		/*
		Returns a refraction of this vector based off a normal,
		the incidents refraction value and the materials refraction value

		Assumes this vector is the incident

		@param normal The normal vector to influence the refraction, must be normalized
		@param n1 The incidents refraction value, if air just choose 1.0f
		@param n2 The materials refraction value
		*/
		Vector3 Refraction(const Vector3& normal, float n1, float n2);

		/*
		Returns the angle difference between the 2 vectors in radians

		@param other The other vector to influence the result
		*/
		float Angle(Vector3& other);

		//Static types
		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 Unit_X;
		static const Vector3 Unit_Y;
		static const Vector3 Unit_Z;
	};

	//Left hand side operators
	Vector3 operator+(float, const Vector3&);
	Vector3 operator*(float, const Vector3&);

	float Distance(const Vector3& a, const Vector3& b);

	Vector3 VectorMin(const Vector3& a, const Vector3& b);
	Vector3 VectorMax(const Vector3& a, const Vector3& b);
}