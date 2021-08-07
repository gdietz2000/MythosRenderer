#pragma once

#include "Mathematics.h"

#include "Vector4.h"

namespace Math {

	struct Matrix4 {

	private:
		//Helper functions
		void Fill(float);
	public:

		//Properties
		union {
			float mat[4 * 4];
			struct {
				Vector4 row0, row1, row2, row3;
			};
			struct {
				float a, b, c, d;
				float e, f, g, h;
				float i, j, k, l;
				float m, n, o, p;
			};
		};

		//Constructors
		Matrix4();
		Matrix4(float diagonal);
		Matrix4(float, float, float, float,
				float, float, float, float,
				float, float, float, float,
				float, float, float, float);
		Matrix4(float*);
		Matrix4(const Vector4&, const Vector4&, const Vector4&, const Vector4&);
		Matrix4(const Matrix3&);
		Matrix4(const Matrix4&) = default;

		//Right hand side operators
		Matrix4 operator+(const Matrix4&);
		Matrix4 operator-(const Matrix4&);
		Matrix4 operator*(const Matrix4&);
		Matrix4 operator*(float);
		Vector4 operator*(const Vector4&);

		bool operator==(const Matrix4&);
		bool operator!=(const Matrix4&);

		//Attribute Functions

		/*Returns the determinant of the matrix*/
		float Determinant();

		/*Returns the summation of the diagonal values*/
		float Trace();

		/*Returns a transposed matrix based off the current matrix*/
		Matrix4 Transpose();

		/*Returns a matrix containing the current matrix's minors*/
		Matrix4 Minors();

		/*Returns a matrix containing the current matrix's cofactors*/
		Matrix4 Cofactors();

		/*Returns a matrix inverse to the current matrix*/
		Matrix4 Inverse();

		/*Returns a quaternion representing the current matrix*/
		Quaternion ToQuaternion();

		/*Returns a matrix3x3 representing the rotation of the current matrix*/
		Matrix3 ToMatrix3();

		/*Returns position of row major matrix in form of a vector*/
		Vector3 GetPosition();
		
		/*
		Sets Position of row major matrix

		@param pos Position of XYZ component;
		*/
		void SetPosition(const Vector3& pos);

		/*
		Sets Position of row major matrix
		
		@param x Position of X component
		@param y Position of Y component
		@param z Position of Z component
		*/
		void SetPosition(float x, float y, float z);

		//Static functions

		/*
		Returns a matrix4x4 based off of a quaternion

		@param q Quaternion to be converted into a matrix
		*/
		static Matrix4 FromQuaternion(Quaternion q);

		/*
		Returns a translated matrix
		
		@param x Translation along the X axis
		@param y Translation along the Y axis
		@param z Translation along the Z axis
		*/
		static Matrix4 Translate(float x, float y, float z);

		/*
		Returns a translated matrix

		@param translation Translation along the X, Y and Z axis in the form of a vector
		*/
		static Matrix4 Translate(const Vector3& translation);

		/*
		Returns a matrix rotated on the X axis

		@param angle Angle of rotation in radians
		*/
		static Matrix4 RotateX(float angle);

		/*
		Returns a matrix rotated on the Y axis

		@param angle Angle of rotation in radians
		*/
		static Matrix4 RotateY(float angle);

		/*
		Returns a matrix rotated on the Z axis

		@param angle Angle of rotation in radians
		*/
		static Matrix4 RotateZ(float angle);

		/*
		Returns a matrix rotated on a given axis

		@param axis Axis of rotation
		@param angle Angle of rotation in radians
		*/
		static Matrix4 Rotate(const Vector3& axis, float angle);

		/*
		Returns a matrix rotated on a given axis

		@param axisAngle Axis and angle of rotation in the form of a vector
		*/
		static Matrix4 Rotate(const Vector4& axisAngle);

		static Matrix4 RotatePitchYawRoll(float pitch, float yaw, float roll);

		/*
		Returns a scaled matrix

		@param scalar The scale value of the matrix
		*/
		static Matrix4 Scale(float scalar);

		/*
		Returns a scaled matrix

		@param x The scale value of the X component in the matrix
		@param y The scale value of the Y component in the matrix
		@param z The scale value of the Z component in the matrix
		*/
		static Matrix4 Scale(float x, float y, float z);

		/*
		Returns a left handed perspective matrix in column major

		@params FOV Vertical field of view in radians
		@param AspectRatio Aspect Ratio of the window
		@param NearPlane Near Plane of the Frustum
		@param FarPlane Far Plane of the Frustum
		*/
		static Matrix4 PerspectiveFovLH(float FOV, float AspectRatio, float NearPlane, float FarPlane);

		/*
		Returns a left handed column major matrix used for cameras

		@param eye Position of the camera
		@param at Target the camera is looking at
		@param up Up direction of the camera
		*/
		static Matrix4 LookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up);


		/*
		Returns a slerped Matrix using quaternions
		
		@param a First matrix, when ratio is zero, will return this matrix
		@param b Second matrix, when ratio is one, will return this matrix
		@param ratio, valued between zero and one, the main influencer in slerp
		*/
		static Matrix4 Slerp(Matrix4& a, Matrix4& b, float ratio);

		static const Matrix4 Zero;
		static const Matrix4 Identity;
	};

	Matrix4 operator*(float, Matrix4);
	Vector3 operator*(Vector3, Matrix4);
	Vector3 operator*(Vector4, Matrix4);

}