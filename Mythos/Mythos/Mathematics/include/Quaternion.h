#pragma once

#include "Mathematics.h"

namespace Math
{
	struct Quaternion 
	{
		//Properties
		//a + bi + cj + dk
		union {
			float comp[4];
			struct {
				float w, x, y, z;
			};
		};

		//Constructors
		Quaternion();
		Quaternion(float, float, float, float);
		Quaternion(const Vector3&, float);
		Quaternion(const Vector4&);

		//Right hand side operators

		Quaternion operator+(const Quaternion& rhs) const;
		Quaternion operator-(const Quaternion& rhs) const;
		Quaternion operator*(const Quaternion& rhs) const;
		Quaternion operator+=(const Quaternion& rhs);
		Quaternion operator-=(const Quaternion& rhs);
		Quaternion operator*=(const Quaternion& rhs);
		
		Quaternion operator*(float rhs) const;
		Quaternion operator*=(float rhs);

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;


		//Attribute Functions

		/*Returns the length/magnitude of a vector*/
		float Magnitude();

		/*Returns the length/magnitude of a vector squared*/
		float MagnitudeSq();

		/*
		Returns the length/magnitude of a vector using the fast inverse sqrt function
		It is generally accurate enough for most cases
		*/
		float MagnitudeFast();

		/*
		Returns the length/magnitude of a vector using the fast inverse sqrt function
		It is generally accurate enough for most cases.
		Uses no division
		*/
		float MagnitudeSuper();

		/*Returns a normalized quaternion*/
		Quaternion Normalize();

		/*Returns a normalized quaternion using fast magnitude function*/
		Quaternion NormalizeFast();

		/*Returns a normalized quaternion using the super magnitude function*/
		Quaternion NormalizeSuper();

		/*Returns a quaternion inverse to the current values*/
		Quaternion Inverse();

		/*Returns a conjugated quaternion based off the current values*/
		Quaternion Conjugate();

		/*Returns angles in radians in Vector form from Quaternion*/
		Vector3 EulerAngles();

		/*
		Returns the axis and the angle of the quaternion
		X, Y, Z, is the axis and W is the angle
		*/
		Vector4 AxisAngle();


		/*
		Returns the rotation matrix of the quaternion as a matrix3x3.
		*/
		Matrix3 ToMatrix3();

		/*
		Returns the rotation matrix of the quaternion as a matrix4x4.
		*/
		Matrix4 ToMatrix4();

		//Static Functions

		/*
		Returns Quaternion from Euler Angles

		@param angles Vector containing the heading, attitude and bank in a vector
		*/
		static Quaternion FromEulerAngles(const Vector3& angles);

		/* 
		Returns Quaternion from Euler Angles

		@param heading Rotation around the Y axis in radians
		@param attitude Rotation around the Z axis in radians
		@param bank Rotation around the X axis in radians
		*/
		static Quaternion FromEulerAngles(float heading, float attitude, float bank);

		/*
		Returns Quaternion from Axis Angle

		@param axisAngle Axis and angle of rotation in radians in one vector
		*/
		static Quaternion FromAxisAngle(const Vector4& axisAngle);

		/*
		Returns Quaternion from Axis Angle

		@param axis Axis Vector
		@param angle Angle of rotation in radians around the axis
		*/
		static Quaternion FromAxisAngle(const Vector3& axis, float angle);

		/*
		Returns Quaternion from Axis Angle with its W component in radians

		@param x X component of the axis vector
		@param y Y component of the axis vector
		@param z Z component of the axis vector
		@param angle Angle of rotation in radians around the axis
		*/
		static Quaternion FromAxisAngle(float x, float y, float z, float angle);

		/*
		Returns an interpolated Quaternion from a to b given a ratio

		@param qa The first Quaternion
		@param qb The second Quaternion
		@param t The ratio of interpolation

		*/
		static Quaternion Slerp(Quaternion qa, Quaternion qb, float t);

		//Static types
		static const Quaternion Identity;
	};

	//Left hand side operators
	Quaternion operator*(float, const Quaternion&);
}