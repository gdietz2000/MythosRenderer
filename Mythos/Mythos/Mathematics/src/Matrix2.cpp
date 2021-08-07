#include "../include/Mathematics.h"
#include "../include/Matrix2.h"

namespace Math {

	const Matrix2 Matrix2::Zero = Matrix2();
	const Matrix2 Matrix2::Identity = Matrix2(1);

	void Matrix2::Fill(float v) {
		for (int i = 0; i < 4; i++)
			this->mat[i] = v;
	}

	Matrix2::Matrix2() {
		Fill(0);
	}

	Matrix2::Matrix2(float diagonal) {
		Fill(0);
		a = d = diagonal;
	}

	Matrix2::Matrix2(float x1, float y1, float x2, float y2) {
		a = x1;
		b = y1;
		c = x2; 
		d = y2;
	}

	Matrix2::Matrix2(float* arr) {
		*this = Matrix2(
			arr[0], arr[1], arr[2],
			arr[3]);
	}

	Matrix2::Matrix2(const Vector2& a, const Vector2& b)
		: row0(a), row1(b) {}

	Matrix2 Matrix2::operator+(const Matrix2& rhs) {
		Matrix2 sum;
		for (int i = 0; i < 4; i++)
			sum.mat[i] = mat[i] + rhs.mat[i];

		return sum;
	}
	
	Matrix2 Matrix2::operator-(const Matrix2& rhs) {
		Matrix2 sub;
		for (int i = 0; i < 4; i++)
			sub.mat[i] = mat[i] - rhs.mat[i];

		return sub;
	}

	Matrix2 Matrix2::operator*(const Matrix2& rhs) {
		return Matrix2(
			a * rhs.a + b * rhs.c,
			a * rhs.b + b * rhs.d,
			c * rhs.a + d * rhs.c,
			c * rhs.b + d * rhs.d);
	}

	bool Matrix2::operator==(const Matrix2& other) {
		for (int i = 0; i < 4; i++)
			if (!almost_equal(mat[i], other.mat[i], 5))
				return false;

		return true;
	}

	bool Matrix2::operator!=(const Matrix2& other) {
		return !this->operator==(other);
	}

	float Matrix2::Determinant() {
		float det = a * d - b * c;
		if (det == -0)
			return 0;

		return det;
	}

	float Matrix2::Trace() {
		return a + d;
	}

	Matrix2 Matrix2::Inverse() {
		float Det = Determinant();
		if (Det == 0)
			return {};

		Matrix2 mat = *this;

		float temp = mat.a;
		mat.a = mat.d;
		mat.d = temp;

		mat.b = -mat.b;
		mat.c = -mat.c;

		return (1 / Det) * mat;
	}

	Matrix2 Matrix2::Transpose() {
		Matrix2 mat = *this;
		float temp = mat.c;
		mat.c = mat.b;
		mat.b = temp;
		return mat;
	}

	Matrix2 Math::operator*(float scalar, Matrix2 rhs) {
		Matrix2 mult;
		for (int i = 0; i < 4; i++) {
			mult.mat[i] = rhs.mat[i] * scalar;
		}
		return mult;
	}
}