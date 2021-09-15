#include "../include/Vector3.h"
#include "../include/Matrix2.h"
#include "../include/Matrix3.h"
#include "../include/Matrix4.h"
#include "../include/Quaternion.h"

#include <algorithm>

namespace Math 
{
	//Static types
	const Matrix3 Matrix3::Zero = Matrix3();
	const Matrix3 Matrix3::Identity = Matrix3(1);

	//Helper functions
	void Matrix3::Fill(float v) {
		std::fill(mat, mat + sizeof(mat) / sizeof(float), v);
	}

	//Constructors
	Matrix3::Matrix3() {
		this->Fill(0);
	}

	Matrix3::Matrix3(float diagonal) {
		this->Fill(0);
		a = diagonal;
		e = diagonal;
		i = diagonal;
	}

	Matrix3::Matrix3(
		float _a, float _b, float _c,
		float _d, float _e, float _f,
		float _g, float _h, float _i) {
		a = _a;	b = _b;	c = _c;
		d = _d;	e = _e;	f = _f;
		g = _g;	h = _h;	i = _i;
	}

	Matrix3::Matrix3(float* arr) {
		*this = Matrix3(
			arr[0], arr[1], arr[2],
			arr[3], arr[4], arr[5],
			arr[6], arr[7], arr[8]);
	}

	Matrix3::Matrix3(const Vector3& a, const Vector3& b, const Vector3& c)
		: row0(a), row1(b), row2(c) {}

	//Right hand side operators
	Matrix3 Matrix3::operator+(const Matrix3& rhs) {
		Matrix3 sum;
		for (int i = 0; i < 9; i++)
			sum.mat[i] = mat[i] + rhs.mat[i];

		return sum;
	}

	Matrix3 Matrix3::operator-(const Matrix3& rhs) {
		Matrix3 sub;
		for (int i = 0; i < 9; i++)
			sub.mat[i] = mat[i] - rhs.mat[i];

		return sub;
	}

	Matrix3 Matrix3::operator*(const Matrix3& rhs) {
		Matrix3 mult;
		mult.a = (a * rhs.a) + (b * rhs.d) + (c * rhs.g);
		mult.b = (a * rhs.b) + (b * rhs.e) + (c * rhs.h);
		mult.c = (a * rhs.c) + (b * rhs.f) + (c * rhs.i);

		mult.d = (d * rhs.a) + (e * rhs.d) + (f * rhs.g);
		mult.e = (d * rhs.b) + (e * rhs.e) + (f * rhs.h);
		mult.f = (d * rhs.c) + (e * rhs.f) + (f * rhs.i);

		mult.g = (g * rhs.a) + (h * rhs.d) + (i * rhs.g);
		mult.h = (g * rhs.b) + (h * rhs.e) + (i * rhs.h);
		mult.i = (g * rhs.c) + (h * rhs.f) + (i * rhs.i);

		return mult;
	}

	Matrix3 Matrix3::operator*(float scalar) {
		Matrix3 mult;
		for (int i = 0; i < 9; i++)
			mult.mat[i] = mat[i] * scalar;

		return mult;
	}

	Vector3 Matrix3::operator*(const Vector3& rhs) {
		return Vector3(
			rhs.x * mat[0] + rhs.y * mat[1] + rhs.z * mat[2],
			rhs.x * mat[3] + rhs.y * mat[4] + rhs.z * mat[5],
			rhs.x * mat[6] + rhs.y * mat[7] + rhs.z * mat[8]);
	}

	Matrix3& Matrix3::operator=(const Matrix3& other)
	{
		memcpy(this->mat, other.mat, sizeof(Matrix3));
		return *this;
	}

	bool Matrix3::operator==(const Matrix3& other) {
		for (int i = 0; i < 9; i++)
			if (!almost_equal(mat[i], other.mat[i], 5))
				return false;

		return true;
	}

	bool Matrix3::operator!=(const Matrix3& other) {
		return !this->operator==(other);
	}

	//Attribute Functions

	float Matrix3::Determinant() {
		float d1, d2, d3;

		d1 = Matrix2(e, f, h, i).Determinant();
		d2 = Matrix2(d, f, g, i).Determinant();
		d3 = Matrix2(d, e, g, h).Determinant();

		return mat[0] * d1 - mat[1] * d2 + mat[2] * d3;
	}

	float Matrix3::Trace() {
		return a + e + i;
	}

	Matrix3 Matrix3::Transpose() 
	{
		Matrix3 m;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m.mat[i + j * 3] = this->mat[j + i * 3];

		return m;
	}

	Matrix3 Matrix3::Minors() 
	{
		return Matrix3(
			Matrix2(e, f, h, i).Determinant(),
			Matrix2(d, f, g, i).Determinant(),
			Matrix2(d, e, g, h).Determinant(),
			Matrix2(b, c, h, i).Determinant(),
			Matrix2(a, c, g, i).Determinant(),
			Matrix2(a, b, g, h).Determinant(),
			Matrix2(b, c, e, f).Determinant(),
			Matrix2(a, c, d, f).Determinant(),
			Matrix2(a, b, d, e).Determinant());
	}

	Matrix3 Matrix3::Cofactors() {
		Matrix3 cof = *this;

		for (int i = 1; i < 9; i += 2) {
			if (cof.mat[i] != 0)
				cof.mat[i] *= -1;
		}

		return cof;
	}

	Matrix3 Matrix3::Inverse() {
		float Det = this->Determinant();

		if (Det == 0)
			return {};

		Matrix3 adjugate = this->Minors().Cofactors().Transpose();
		return (1 / Det) * adjugate;
	}

	Quaternion Matrix3::ToQuaternion() {
		Quaternion q;

		float trace = this->Trace();

		if (trace > 0) {
			float S = sqrt(1 + trace) * 2;
			q.w = 0.25f * S;
			q.x = (h - f) / S;
			q.y = (c - g) / S;
			q.z = (d - b) / S;
		}
		else if ((a > e) && a > i) {
			float S = sqrt(1 + a - e - i) * 2;
			q.w = (h - f) / S;
			q.x = 0.25f * S;
			q.y = (d + b) / S;
			q.z = (g + c) / S;
		}
		else if (e > i) {
			float S = sqrt(1 + e - a - i) * 2;
			q.w = (c - g) / S;
			q.x = (d + b) / S;
			q.y = 0.25f * S;
			q.z = (h + f) / S;
		}
		else {
			float S = sqrt(1.0f + i - a - e) * 2;
			q.w = (d - b) / S;
			q.x = (g + c) / S;
			q.y = (h + f) / S;
			q.z = 0.25f * S;
		}

		return q;
	}

	Matrix3 Matrix3::FromQuaternion(Quaternion q) {
		return q.ToMatrix3();
	}

	Matrix4 Matrix3::ToMatrix4() {
		return Matrix4(*this);
	}

	Matrix3 Math::operator*(float scalar, Matrix3 mat) {
		return mat * scalar;
	}

	Vector3 Math::operator*(Vector3 lhs, Matrix3 rhs) {
		return Vector3(
			lhs.x * rhs.mat[0] + lhs.y * rhs.mat[3] + lhs.z * rhs.mat[6],
			lhs.x * rhs.mat[1] + lhs.y * rhs.mat[4] + lhs.z * rhs.mat[7],
			lhs.x * rhs.mat[2] + lhs.y * rhs.mat[5] + lhs.z * rhs.mat[8]);
	}
}