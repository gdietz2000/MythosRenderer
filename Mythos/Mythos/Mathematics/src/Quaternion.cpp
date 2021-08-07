#include "../include/Quaternion.h"

#include "../include/Vector3.h"
#include "../include/Vector4.h"
#include "../include/Matrix3.h"
#include "../include/Matrix4.h"

namespace Math
{
	const Quaternion Quaternion::Identity = Quaternion(0, 0, 0, 1);

	Quaternion::Quaternion()
		: w(1), x(0), y(0), z(0) {}

	Quaternion::Quaternion(float _w, float _x, float _y, float _z)
		: w(_w), x(_x), y(_y), z(_z) {}

	Quaternion::Quaternion(const Vector3& xyz, float _w)
		: w(_w), x(xyz.x), y(xyz.y), z(xyz.z) {}

	Quaternion::Quaternion(const Vector4& xyzw)
		: w(xyzw.w), x(xyzw.x), y(xyzw.y), z(xyzw.z) {}

	//Right hand side operators

	Quaternion Quaternion::operator+(const Quaternion& rhs) const {
		return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Quaternion Quaternion::operator-(const Quaternion& rhs) const {
		return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Quaternion Quaternion::operator*(const Quaternion& rhs) const {
		return Quaternion(
			rhs.w * w - rhs.x * x - rhs.y * y - rhs.z * z,
			rhs.w * x + rhs.x * w - rhs.y * z + rhs.z * y,
			rhs.w * y + rhs.x * z + rhs.y * w - rhs.z * x,
			rhs.w * z - rhs.x * y + rhs.y * x + rhs.z * w);
	}

	Quaternion Quaternion::operator+=(const Quaternion& rhs) {
		return *this = *this + rhs;
	}

	Quaternion Quaternion::operator-=(const Quaternion& rhs) {
		return *this = *this - rhs;
	}

	Quaternion Quaternion::operator*=(const Quaternion& rhs) {
		return *this = *this * rhs;
	}

	Quaternion Quaternion::operator*(float scalar) const {
		return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
	}

	Quaternion Quaternion::operator*=(float scalar) {
		return *this = *this * scalar;
	}

	bool Quaternion::operator==(const Quaternion& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Quaternion::operator!=(const Quaternion& other) const {
		return !this->operator==(other);
	}

	//Attribute Functions

	float Quaternion::Magnitude() {
		return std::sqrtf(SQ(w) + SQ(x) + SQ(y) + SQ(z));
	}

	float Quaternion::MagnitudeSq() {
		return SQ(w) + SQ(x) + SQ(y) + SQ(z);
	}

	float Quaternion::MagnitudeFast() {
		return 1 / fast_inv_sqrt(SQ(w) + SQ(x) + SQ(y) + SQ(z));
	}

	float Quaternion::MagnitudeSuper() {
		return fast_inv_sqrt(SQ(w) + SQ(x) + SQ(y) + SQ(z));
	}

	Quaternion Quaternion::Normalize() {
		float mag = this->Magnitude();
		if (mag == 0)
			return {};

		return Quaternion(w / mag, x / mag, y / mag, z / mag);
	}

	Quaternion Quaternion::NormalizeFast() {
		float magFast = this->MagnitudeFast();
		if (magFast == 0)
			return {};

		return Quaternion(w / magFast, x / magFast, y / magFast, z / magFast);
	}

	Quaternion Quaternion::NormalizeSuper() {
		float magSuper = this->MagnitudeSuper();
		if (magSuper == 0)
			return {};

		return Quaternion(w * magSuper, x * magSuper, y * magSuper, z * magSuper);
	}

	Quaternion Quaternion::Inverse() {
		float magSq = MagnitudeSq();
		if (almost_equal(magSq, 0.0f, 5))
			return {};

		return Quaternion(w / magSq, -x / magSq, -y / magSq, -z / magSq);
	}

	Quaternion Quaternion::Conjugate() {
		return Quaternion(w, -x, -y, -z);
	}

	Vector3 Quaternion::EulerAngles() {
		float heading, attitude, bank;


		float wSq = SQ(this->w);
		float xSq = SQ(this->x);
		float ySq = SQ(this->y);
		float zSq = SQ(this->z);

		float unit = wSq + xSq + ySq + zSq;
		float test = this->x * this->y + this->z * this->w;

		if (test > 0.499f * unit) //At north pole
		{
			heading = 2 * atan2(this->x, this->w);
			attitude = PI / 2;
			bank = 0;
			return Vector3(bank, heading, attitude);
		}

		if (test < -0.499f * unit) //At south pole
		{
			heading = -2 * atan2(this->x, this->w);
			attitude = -PI / 2;
			bank = 0;
			return Vector3(bank, heading, attitude);
		}

		heading = atan2(2 * this->y * this->w - 2 * this->x * this->z, xSq - ySq - zSq + wSq);
		attitude = asinf(2 * test / unit);
		bank = atan2(2 * this->x * this->w - 2 * this->y * this->z, -xSq + ySq - zSq + wSq);

		return Vector3(bank, heading, attitude);
	}

	Vector4 Quaternion::AxisAngle() {
		if (this->w > 1)
			this->Normalize();

		Vector4 axisAngle;

		axisAngle.w = 2 * acos(this->w);
		float denom = sqrt(1 - SQ(this->w));

		if (denom < 0.001f) {
			axisAngle.x = this->x;
			axisAngle.y = this->y;
			axisAngle.z = this->z;
		}
		else {
			axisAngle.x = this->x / denom;
			axisAngle.y = this->y / denom;
			axisAngle.z = this->z / denom;
		}

		return axisAngle;
	}

	Matrix3 Quaternion::ToMatrix3() {
		Quaternion q = *this;

		float qw = q.w;
		float qx = q.x;
		float qy = q.y;
		float qz = q.z;

		const float n = 1.0f / sqrtf(SQ(qw) + SQ(qx) + SQ(qy) + SQ(qz));
		qw *= n;
		qx *= n;
		qy *= n;
		qz *= n;

		float a = 1.0f - 2.0f * SQ(qy) - 2.0f * SQ(qz);
		float b = (2.0f * qx * qy) - (2.0f * qw * qz);
		float c = (2.0f * qx * qz) + (2.0f * qw * qy);
		float d = (2.0f * qx * qy) + (2.0f * qw * qz);
		float e = 1.0f - 2.0f * SQ(qx) - 2.0f * SQ(qz);
		float f = (2.0f * qy * qz) - (2.0f * qw * qx);
		float g = (2.0f * qx * qz) - (2.0f * qw * qy);
		float h = (2.0f * qy * qz) + (2.0f * qw * qx);
		float i = 1.0f - 2.0f * SQ(qx) - 2.0f * SQ(qy);
		return Matrix3
		(a, b, c,
			d, e, f,
			g, h, i);
	}

	Matrix4 Quaternion::ToMatrix4() {
		return Matrix4(ToMatrix3());
	}

	//Static Functions

	Quaternion Quaternion::FromEulerAngles(const Vector3& angles) {
		return FromEulerAngles(angles.y, angles.z, angles.x);
	}

	Quaternion Quaternion::FromEulerAngles(float heading, float attitude, float bank) {
		float c1 = cos(heading * 0.5f);
		float c2 = cos(attitude * 0.5f);
		float c3 = cos(bank * 0.5f);
		float s1 = sin(heading * 0.5f);
		float s2 = sin(attitude * 0.5f);
		float s3 = sin(bank * 0.5f);

		return Quaternion(
			c1 * c2 * c3 - s1 * s2 * s3,
			s1 * s2 * c3 + c1 * c2 * s3,
			s1 * c2 * c3 + c1 * s2 * s3,
			c1 * s2 * c3 - s1 * c2 * s3);
	}

	Quaternion Quaternion::FromAxisAngle(const Vector4& axisAngle) {
		return FromAxisAngle(axisAngle.XYZ(), axisAngle.w);
	}

	Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle) {
		return FromAxisAngle(axis.x, axis.y, axis.z, angle);
	}

	Quaternion Quaternion::FromAxisAngle(float x, float y, float z, float angle) {
		return Quaternion(cos(angle * 0.5f), x * sin(angle * 0.5f), y * sin(angle * 0.5f), z * sin(angle * 0.5f));
	}

	Quaternion Quaternion::Slerp(Quaternion qa, Quaternion qb, float ratio) {
		Quaternion qm;
		float cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;

		//If qa == qb then theta is 0 and we can return either
		if (abs(cosHalfTheta) >= 1.0f) {
			return qa;
		}
		
		float halfTheta = acos(cosHalfTheta);
		float sinHalfTheta = sqrt(1.0f - SQ(cosHalfTheta));
		//if theta is 180 degrees the result isn't fully defined
		//we could rotate around any axis normal to qa or qb
		if (fabsf(sinHalfTheta) < 0.001f) {
			qm.w = (qa.w * 0.5f + qb.w * 0.5f);
			qm.x = (qa.x * 0.5f + qb.x * 0.5f);
			qm.y = (qa.y * 0.5f + qb.y * 0.5f);
			qm.z = (qa.z * 0.5f + qb.z * 0.5f);
			return qm;
		}

		float ratioA = sin((1 - ratio) * halfTheta) / sinHalfTheta;
		float ratioB = sin(ratio * halfTheta) / sinHalfTheta;

		qm.w = (qa.w * ratioA + qb.w * ratioB);
		qm.x = (qa.x * ratioA + qb.x * ratioB);
		qm.y = (qa.y * ratioA + qb.y * ratioB);
		qm.z = (qa.z * ratioA + qb.z * ratioB);
		return qm;
	}

	Quaternion Math::operator*(float value, const Quaternion& rhs) {
		return rhs * value;
	}
}