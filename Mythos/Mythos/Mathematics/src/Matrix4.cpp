
#include "../include/Vector3.h"
#include "../include/Vector4.h"
#include "../include/Matrix3.h"
#include "../include/Matrix4.h"
#include "../include/Quaternion.h"

#include <vector>
#include <algorithm>

namespace Math 
{
	const Matrix4 Matrix4::Zero = Matrix4();
	const Matrix4 Matrix4::Identity = Matrix4(1);

	void Matrix4::Fill(float v) 
	{
		std::fill(mat, mat + sizeof(mat) / sizeof(float), v);
	}

	Matrix4::Matrix4() {
		Fill(0);
	}

	Matrix4::Matrix4(float diagonal) {
		Fill(0);
		a = f = k = p = diagonal;
	}

	Matrix4::Matrix4(
		float _a, float _b, float _c, float _d,
		float _e, float _f, float _g, float _h,
		float _i, float _j, float _k, float _l,
		float _m, float _n, float _o, float _p)
	{
		a = _a; b = _b;	c = _c;	d = _d;
		e = _e; f = _f; g = _g; h = _h;
		i = _i; j = _j; k = _k; l = _l;
		m = _m; n = _n; o = _o; p = _p;
	}

	Matrix4::Matrix4(float* arr) {
		*this = Matrix4(
			arr[0], arr[1], arr[2], arr[3],
			arr[4], arr[5],	arr[6], arr[7],
			arr[8], arr[9], arr[10], arr[11],
			arr[12], arr[13], arr[14], arr[15]);
	}

	Matrix4::Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d)
		: row0(a), row1(b), row2(c), row3(d) {}

	Matrix4::Matrix4(const Matrix3& other) 
	{
		Fill(0);

		for (int j = 0; j < 3; j++)
			for (int i = 0; i < 3; i++)
				mat[i + j * 4] = other.mat[i + j * 3];

		p = 1;
	}

	//Right hand side operators
	Matrix4 Matrix4::operator+(const Matrix4& rhs) {
		Matrix4 sum;
		for (int i = 0; i < 16; i++)
			sum.mat[i] = mat[i] + rhs.mat[i];

		return sum;
	}

	Matrix4 Matrix4::operator-(const Matrix4& rhs) {
		Matrix4 sub;
		for (int i = 0; i < 16; i++)
			sub.mat[i] = mat[i] - rhs.mat[i];

		return sub;
	}

	Matrix4 Matrix4::operator* (const Matrix4& rhs)
	{
		Matrix4 mult;
		mult.a = (a * rhs.a) + (b * rhs.e) + (c * rhs.i) + (d * rhs.m);
		mult.b = (a * rhs.b) + (b * rhs.f) + (c * rhs.j) + (d * rhs.n);
		mult.c = (a * rhs.c) + (b * rhs.g) + (c * rhs.k) + (d * rhs.o);
		mult.d = (a * rhs.d) + (b * rhs.h) + (c * rhs.l) + (d * rhs.p);

		mult.e = (e * rhs.a) + (f * rhs.e) + (g * rhs.i) + (h * rhs.m);
		mult.f = (e * rhs.b) + (f * rhs.f) + (g * rhs.j) + (h * rhs.n);
		mult.g = (e * rhs.c) + (f * rhs.g) + (g * rhs.k) + (h * rhs.o);
		mult.h = (e * rhs.d) + (f * rhs.h) + (g * rhs.l) + (h * rhs.p);

		mult.i = (i * rhs.a) + (j * rhs.e) + (k * rhs.i) + (l * rhs.m);
		mult.j = (i * rhs.b) + (j * rhs.f) + (k * rhs.j) + (l * rhs.n);
		mult.k = (i * rhs.c) + (j * rhs.g) + (k * rhs.k) + (l * rhs.o);
		mult.l = (i * rhs.d) + (j * rhs.h) + (k * rhs.l) + (l * rhs.p);

		mult.m = (m * rhs.a) + (n * rhs.e) + (o * rhs.i) + (p * rhs.m);
		mult.n = (m * rhs.b) + (n * rhs.f) + (o * rhs.j) + (p * rhs.n);
		mult.o = (m * rhs.c) + (n * rhs.g) + (o * rhs.k) + (p * rhs.o);
		mult.p = (m * rhs.d) + (n * rhs.h) + (o * rhs.l) + (p * rhs.p);

		return mult;
	}

	Matrix4 Matrix4::operator*(float scalar) {
		Matrix4 mult;
		for (int i = 0; i < 16; i++)
			mult.mat[i] = mat[i] * scalar;

		return mult;
	}

	Vector4 Matrix4::operator*(const Vector4& rhs) {
		return Vector4(
			rhs.x * row0.x + rhs.y * row0.y + rhs.z * row0.z + rhs.w * row0.w,
			rhs.x * row1.x + rhs.y * row1.y + rhs.z * row1.z + rhs.w * row1.w,
			rhs.x * row2.x + rhs.y * row2.y + rhs.z * row2.z + rhs.w * row2.w,
			rhs.x * row3.x + rhs.y * row3.y + rhs.z * row3.z + rhs.w * row3.w);
	}

	bool Matrix4::operator==(const Matrix4& other) {
		for (int i = 0; i < 16; i++)
			if (!almost_equal(mat[i], other.mat[i], 5))
				return false;

		return true;
	}

	bool Matrix4::operator!=(const Matrix4& other) {
		return !this->operator==(other);
	}

	float Matrix4::Determinant() {
		float d1, d2, d3, d4;

		d1 = Matrix3(Vector3(f, g, h), Vector3(j, k, l), Vector3(n, o, p)).Determinant();
		d2 = Matrix3(Vector3(e, g, h), Vector3(i, k, l), Vector3(m, o, p)).Determinant();
		d3 = Matrix3(Vector3(e, f, h), Vector3(i, j, l), Vector3(m, n, p)).Determinant();
		d4 = Matrix3(Vector3(e, f, g), Vector3(i, j, k), Vector3(m, n, o)).Determinant();

		return a * d1 - b * d2 + c * d3 - d * d4;
	}

	float Matrix4::Trace() {
		return a + f + k + p;
	}

	Matrix4 Matrix4::Transpose() {
		Matrix4 Transpose = Matrix4::Zero;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				Transpose.mat[i + j * 4] = this->mat[j + i * 4];
			}
		}
		return Transpose;
	}

	Matrix4 Matrix4::Minors() 
	{
		Matrix4 Minor = Matrix4::Zero;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				std::vector<float> points;
				for (int i = 0; i < 4; i++)
				{
					if (i == row)
						continue;
					for (int j = 0; j < 4; j++)
					{
						if (j == col)
							continue;
						points.push_back(mat[j + i * 4]);
					}
				}

				Minor.mat[col + row * 4] = Matrix3(points.data()).Determinant();
			}
		}

		return Minor;
	}

	Matrix4 Matrix4::Cofactors() {
		Matrix4 cof = Matrix4::Zero;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if ((even(row) && odd(col)) || (odd(row) && even(col)) && mat[col + row * 4] != 0)
				{
					if (mat[col + row * 4] == 0)
						cof.mat[col + row * 4] = mat[col + row * 4];
					else
						cof.mat[col + row * 4] = -mat[col + row * 4];
				}
				else
					cof.mat[col + row * 4] = mat[col + row * 4];
			}
		}

		return cof;
	}

	Matrix4 Matrix4::Inverse() {
		float Det = this->Determinant();

		if (Det == 0)
			return {};

		Matrix4 adjugate = this->Minors().Cofactors().Transpose();
		return (1 / Det) * adjugate;
	}

	Quaternion Matrix4::ToQuaternion()
	{
		return this->ToMatrix3().ToQuaternion();
	}

	Matrix3 Matrix4::ToMatrix3() {
		return Matrix3(
			a, b, c,
			e, f, g,
			i, j, k);
	}

	Vector3 Matrix4::GetPosition() {
		return row3.XYZ();
	}

	void Matrix4::SetPosition(const Vector3& pos)
	{
		SetPosition(pos.x, pos.y, pos.z);
	}

	void Matrix4::SetPosition(float x, float y, float z)
	{
		m = x;
		n = y;
		o = z;
	}

	Matrix4 Matrix4::FromQuaternion(Quaternion q){
		return Matrix4(Matrix3::FromQuaternion(q));
	}

	Matrix4 Matrix4::Translate(float x, float y, float z) {
		Matrix4 trans = Matrix4::Identity;
		trans.m = x; trans.n = y; trans.o = z;
		return trans;
	}

	Matrix4 Matrix4::Translate(const Vector3& translate) {
		return Translate(translate.x, translate.y, translate.z);
	}

	Matrix4 Matrix4::RotateX(float theta) {
		return Matrix4(Matrix3(
			1,			0,			 0,
			0, cos(theta), -sin(theta),
			0, sin(theta),  cos(theta)).Transpose());
	}

	Matrix4 Matrix4::RotateY(float theta) {
		return Matrix4(Matrix3(
			 cos(theta), 0, sin(theta),
			0,			 1,			 0,
			-sin(theta), 0, cos(theta)).Transpose());
			
	}

	Matrix4 Matrix4::RotateZ(float theta) {
		return Matrix4(Matrix3(
			cos(theta), -sin(theta), 0,
			sin(theta),  cos(theta), 0,
					 0,			  0, 1).Transpose());
	}

	Matrix4 Matrix4::Rotate(const Vector3& axis, float theta) {
		return Matrix4(
		(cosf(theta) + SQ(axis.x)) * (1 - cosf(theta)),						(axis.x * axis.y) * (1 - cosf(theta) - (axis.z * sinf(theta))), (axis.x * axis.z) * (1 - cosf(theta) + (axis.y * sinf(theta))), 0.0f,
		  (axis.y * axis.x) * (1 - cosf(theta) + (axis.z * sinf(theta))), (cosf(theta) + SQ(axis.y)) * (1 - cosf(theta)),					  (axis.y * axis.z) * (1 - cosf(theta) - (axis.x * sinf(theta))), 0.0f,
		  (axis.z * axis.x) * (1 - cosf(theta) - (axis.y * sinf(theta))), (axis.z * axis.y) * (1 - cosf(theta) + (axis.x * sinf(theta))), (cosf(theta) + SQ(axis.z)) * (1 - cosf(theta)),						0.0f,
		  0.0f,																	0.0f,																0.0f,																	1.0f).Transpose();
	}

	Matrix4 Matrix4::Rotate(const Vector4& axisAngle) {
		return Rotate(axisAngle.XYZ(), axisAngle.w);
	}

	Matrix4 Matrix4::RotatePitchYawRoll(float pitch, float yaw, float roll) {
		Quaternion q;
		return q.FromEulerAngles(yaw, roll, pitch).ToMatrix4();
	}

	Matrix4 Matrix4::Scale(float x, float y, float z) {
		Matrix4 temp = Matrix4::Identity;
		temp.a = x;
		temp.f = y;
		temp.k = z;
		return temp;
	}

	Matrix4 Matrix4::Scale(float scalar) {
		return Scale(scalar, scalar, scalar);
	}

	Matrix4 Matrix4::PerspectiveFovLH(float FOV, float ar, float n, float f)
	{
		Matrix4 per = Matrix4::Zero;
		float range = n - f;

		per.a = 1 / (ar * tanf(FOV / 2));
		per.f = 1 / tanf(FOV / 2);
		per.k = (-n - f) / range;
		per.l = 1;
		per.o = (2 * f * n) / range;

		return per;
	}

	Matrix4 Matrix4::LookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up)
	{
		Vector3 fwd = (at - eye).Normalize();
		Vector3 side = (up.Cross(fwd)).Normalize();
		Vector3 newUp = (fwd.Cross(side)).Normalize();

		//fwd = -fwd;

		float sDot = side.Dot(eye);
		float uDot = newUp.Dot(eye);
		float fDot = fwd.Dot(eye);

		return Matrix4(
			side.x,		side.y,		side.z,		sDot == 0 ? 0 : -sDot,
			newUp.x,	newUp.y,	newUp.z,	uDot == 0 ? 0 : -uDot,
			fwd.x,		fwd.y,		fwd.z,		fDot == 0 ? 0 : -fDot,
			0,			0,			0,			 1).Transpose();
	}

	Matrix4 Matrix4::Slerp(Matrix4& a, Matrix4& b, float ratio) {
		Quaternion qa = a.ToQuaternion();
		Quaternion qb = b.ToQuaternion();

		return Quaternion::Slerp(qa, qb, ratio).ToMatrix4();
	}

	Matrix4 Math::operator*(float lhs, Matrix4 rhs) {
		return rhs * lhs;
	}

	Vector3 Math::operator*(Vector3 lhs, Matrix4 rhs) {
		Vector3 res;
		res.x = rhs.a * lhs.x + rhs.e * lhs.y + rhs.i * lhs.z + rhs.m * 1;
		res.y = rhs.b * lhs.x + rhs.f * lhs.y + rhs.j * lhs.z + rhs.n * 1;
		res.z = rhs.c * lhs.x + rhs.g * lhs.y + rhs.k * lhs.z + rhs.o * 1;
		return res;
	}

	Vector3 Math::operator*(Vector4 lhs, Matrix4 rhs) {
		Vector3 res;
		res.x = rhs.a * lhs.x + rhs.e * lhs.y + rhs.i * lhs.z + rhs.m * lhs.w;
		res.y = rhs.b * lhs.x + rhs.f * lhs.y + rhs.j * lhs.z + rhs.n * lhs.w;
		res.z = rhs.c * lhs.x + rhs.g * lhs.y + rhs.k * lhs.z + rhs.o * lhs.w;
		return res;
	}
}