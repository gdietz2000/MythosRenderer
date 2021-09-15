#pragma once

#include "Mathematics.h"

namespace Math 
{
	struct Vector4 
	{
		//Properties
		union {
			float comp[4];
			struct {
				float x, y, z, w;
			};
		};

		//Constructors
		Vector4();
		Vector4(float);
		Vector4(float, float, float, float);
		Vector4(const Vector2&, const Vector2&);
		Vector4(const Vector3&, float);
		Vector4(const Vector4&) = default;

		//Right hand side operators
		Vector4 operator+(float);
		Vector4 operator+(float) const;
		Vector4 operator-(float);
		Vector4 operator-(float) const;
		Vector4 operator*(float);
		Vector4 operator*(float) const;
		Vector4 operator/(float);
		Vector4 operator/(float) const;

		Vector4 operator+(const Vector4&);
		Vector4 operator+(const Vector4&) const;
		Vector4 operator-(const Vector4&);
		Vector4 operator-(const Vector4&) const;
		Vector4 operator*(const Vector4&);
		Vector4 operator*(const Vector4&) const;
		Vector4 operator/(const Vector4&);
		Vector4 operator/(const Vector4&) const;

		float operator[](int index);
		float operator[](int index) const;

		Vector4& operator=(const Vector4&);

		bool operator==(float) const;
		bool operator!=(float) const;
		bool operator==(const Vector4&) const;
		bool operator!=(const Vector4&) const;

		Vector4 operator += (float);
		Vector4 operator += (const Vector4&);
		Vector4 operator -= (float);
		Vector4 operator -= (const Vector4&);
		Vector4 operator *= (float);
		Vector4 operator *= (const Vector4&);
		Vector4 operator /= (float);
		Vector4 operator /= (const Vector4&);

		//Negates the values of the affected vector
		Vector4 operator-() const;

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

		/*
		Returns the dot product of current and other vector

		@param other The other vector needed to product the dot product
		*/
		float Dot(const Vector4& other);

		/*Returns a normalized vector*/
		Vector4 Normalize();

		/*Returns a normalized vector using fast magnitude function*/
		Vector4 NormalizeFast();

		/*Returns a normalized vector using super magnitude function*/
		Vector4 NormalizeSuper();

		//Returns the first 3 values in the form of a vector
		Vector3 XYZ() const;

		//Sets the first 3 values of this vector based on values given
		void XYZ(float, float, float);

		//Sets the first 3 values of this vector based on vector given
		void XYZ(const Vector3&);

		/*
		Returns a reflection of this vector based off a normal

		Assumes this vector is the incident

		@param normal The normal vector to influence the reflection, must be normalized
		*/
		Vector4 Reflection(const Vector4&);

		/*
		Returns a refraction of this vector based off a normal,
		the incidents refraction value and the materials refraction value

		Assumes this vector is the incident

		@param normal The normal vector to influence the refraction, must be normalized
		@param n1 The incidents refraction value, if air just choose 1.0f
		@param n2 The materials refraction value
		*/
		Vector4 Refraction(const Vector4 & normal, float n1, float n2);

		static const Vector4 Zero;
		static const Vector4 One;
		static const Vector4 Unit_X;
		static const Vector4 Unit_Y;
		static const Vector4 Unit_Z;
		static const Vector4 Unit_W;
	};

	//Left hand side operators
	Vector4 operator+(float, const Vector4&);
	Vector4 operator*(float, const Vector4&);
}