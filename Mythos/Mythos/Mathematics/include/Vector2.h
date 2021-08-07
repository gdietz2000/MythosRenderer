#pragma once

#include "Mathematics.h"

namespace Math
{
	struct Vector2 
	{
		//Properties
		union {
			float comp[2];
			struct {
				float x, y;
			};
		};

		//Constructors
		Vector2();
		Vector2(float);
		Vector2(float, float);
		Vector2(const Vector2&) = default;

		//Right hand side operators
		Vector2 operator+(float);
		Vector2 operator+(float) const;
		Vector2 operator-(float);
		Vector2 operator-(float) const;
		Vector2 operator*(float);
		Vector2 operator*(float) const;
		Vector2 operator/(float);
		Vector2 operator/(float) const;

		Vector2 operator+(const Vector2&);
		Vector2 operator+(const Vector2&) const;
		Vector2 operator-(const Vector2&);
		Vector2 operator-(const Vector2&) const;
		Vector2 operator*(const Vector2&);
		Vector2 operator*(const Vector2&) const;
		Vector2 operator/(const Vector2&);
		Vector2 operator/(const Vector2&) const;

		Vector2& operator=(const Vector2&) = default;
		float operator[](int index);
		float operator[](int index) const;

		bool operator==(float) const;
		bool operator!=(float) const;
		bool operator==(const Vector2&) const;
		bool operator!=(const Vector2&) const;

		Vector2 operator += (float);
		Vector2 operator += (const Vector2&);
		Vector2 operator -= (float);
		Vector2 operator -= (const Vector2&);
		Vector2 operator *= (float);
		Vector2 operator *= (const Vector2&);
		Vector2 operator /= (float);
		Vector2 operator /= (const Vector2&);

		//Negates the values of the affected Vector
		Vector2 operator-() const;

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

		/*Returns the angle of rotation of a vector. Only available for 2D vectors*/
		float Heading();

		/*
		Returns the dot product of current and other vector

		@param other The other vector needed to produce the dot product
		*/
		float Dot(const Vector2& other);

		/*
		Returns the determinant of current and other vector

		@param other The other vector needed to produce the determinant
		*/
		float Determinant(const Vector2& other);
		
		/*Returns a normalized vector*/
		Vector2 Normalize();

		/*Returns a normalized vector using fast magnitude function*/
		Vector2 NormalizeFast();

		/*Returns a normalized vector using super magnitude function*/
		Vector2 NormalizeSuper();

		/*
		Returns a reflection of this vector based off a normal
		
		Assumes this vector is the incident

		@param normal The normal vector to influence the reflection, must be normalized
		*/
		Vector2 Reflection(const Vector2& normal);

		/*
		Returns a refraction of this vector based off a normal,
		the incidents refraction value and the materials refraction value

		Assumes this vector is the incident

		@param normal The normal vector to influence the refraction, must be normalized
		@param n1 The incidents refraction value, if air just choose 1.0f
		@param n2 The materials refraction value
		*/
		Vector2 Refraction(const Vector2& normal, float n1, float n2);


		/*
		Returns the angle difference between the 2 vectors in radians

		@param other The other vector to influence the result
		*/
		float Angle(const Vector2 & other);

		//Static types
		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 Unit_X;
		static const Vector2 Unit_Y;
	};

	//Left hand side operators
	Vector2 operator+(float, const Vector2&);
	Vector2 operator*(float, const Vector2&);
}