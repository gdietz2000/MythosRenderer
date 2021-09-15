#pragma once

#include "Mathematics.h"

#include "Vector2.h"

namespace Math {

	struct Matrix2 {

	private:
		//Helper functions
		void Fill(float);
	public:

		//Properties
		union {
			float mat[2 * 2];
			struct {
				Vector2 row0, row1;
			};
			struct {
				float a, b;
				float c, d;
			};
		};

		//Constructors
		Matrix2();
		Matrix2(float diagonal);
		Matrix2(float, float, float, float);
		Matrix2(float*);
		Matrix2(const Vector2&, const Vector2&);
		Matrix2(const Matrix2&) = default;

		//Right hand side operators
		Matrix2 operator+(const Matrix2&);
		Matrix2 operator-(const Matrix2&);
		Matrix2 operator*(const Matrix2&);

		Matrix2& operator=(const Matrix2&);

		bool operator==(const Matrix2&);
		bool operator!=(const Matrix2&);

		//Attribute Functions

		/*Returns the determinant of the matrix*/
		float Determinant();

		/*Returns the summation of the diagonal values*/
		float Trace();

		/*Returns an inverse matrix based off the current matrix*/
		Matrix2 Inverse();

		/*Returns a transposed matrix based off the current matrix*/
		Matrix2 Transpose();

		//Static types
		static const Matrix2 Zero;
		static const Matrix2 Identity;
	};

	Matrix2 operator*(float, Matrix2);
}