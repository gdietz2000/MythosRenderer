#pragma once

#include "Mathematics.h"

#include "Vector3.h"

namespace Math 
{
	struct Matrix3 {

	private:
		//Helper functions
		void Fill(float);
	public:

		//Properties
		union {
			float mat[3 * 3];
			struct {
				Vector3 row0, row1, row2;
			};
			struct {
				float a, b, c;
				float d, e, f;
				float g, h, i;
			};
		};

		//Constructors
		Matrix3();
		Matrix3(float diagonal);
		Matrix3(float, float, float, 
				float, float, float,
				float, float, float);
		Matrix3(float*);
		Matrix3(const Vector3&, const Vector3&, const Vector3&);
		Matrix3(const Matrix3&) = default;

		//Right hand side operators
		Matrix3 operator+(const Matrix3&);
		Matrix3 operator-(const Matrix3&);
		Matrix3 operator*(const Matrix3&);
		Matrix3 operator*(float);
		Vector3 operator*(const Vector3&);

		Matrix3& operator=(const Matrix3&);

		bool operator==(const Matrix3&);
		bool operator!=(const Matrix3&);

		//Attribute Functions

		/*Returns the determinant of the matrix*/
		float Determinant();

		/*Returns the summation of the diagonal values*/
		float Trace();

		/*Returns a transposed matrix based off the current matrix*/
		Matrix3 Transpose();

		/*Returns a matrix containing the current matrix's minors*/
		Matrix3 Minors();

		/*Returns a matrix containing the current matrix's cofactors*/
		Matrix3 Cofactors();

		/*Returns a matrix inverse to the current matrix*/
		Matrix3 Inverse();

		/*Returns a quaternion representing the current matrix*/
		Quaternion ToQuaternion();

		/*Returns a matrix4x4 representing the current matrix*/
		Matrix4 ToMatrix4();

		
		//Static functions

		/*
		Returns a matrix3x3 based off of a quaternion

		@param q Quaternion to be converted into a matrix
		*/
		static Matrix3 FromQuaternion(Quaternion q);

		//Static types
		static const Matrix3 Zero;
		static const Matrix3 Identity;
	};

	Matrix3 operator*(float, Matrix3);
	Vector3 operator*(Vector3, Matrix3);
}