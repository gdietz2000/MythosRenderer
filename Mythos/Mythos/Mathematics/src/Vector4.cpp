#include "../include/Vector2.h"
#include "../include/Vector3.h"
#include "../include/Vector4.h"

#include <iterator>

namespace Math
{
	const Vector4 Vector4::Zero		= Vector4(0, 0, 0, 0);
	const Vector4 Vector4::One		= Vector4(1, 1, 1, 1);
	const Vector4 Vector4::Unit_X	= Vector4(1, 0, 0, 0);
	const Vector4 Vector4::Unit_Y	= Vector4(0, 1, 0, 0);
	const Vector4 Vector4::Unit_Z	= Vector4(0, 0, 1, 0);
	const Vector4 Vector4::Unit_W	= Vector4(0, 0, 0, 1);

	Vector4::Vector4()
		:x(0), y(0), z(0), w(0) {}

	Vector4::Vector4(float scalar)
		:x(scalar), y(scalar), z(scalar), w(scalar) {}

	Vector4::Vector4(float _x, float _y, float _z, float _w)
		:x(_x), y(_y), z(_z), w(_w) {}

	Vector4::Vector4(const Vector2& a, const Vector2& b)
		:x(a.x), y(a.y), z(b.x), w(b.y) {}

	Vector4::Vector4(const Vector3& vec, float scalar)
		:x(vec.x), y(vec.y), z(vec.z), w(scalar) {}

	Vector4 Vector4::operator+(float scalar) {
		return Vector4(x + scalar, y + scalar, z + scalar, w + scalar);
	}

	Vector4 Vector4::operator+(float scalar) const {
		return Vector4(x + scalar, y + scalar, z + scalar, w + scalar);
	}

	Vector4 Vector4::operator-(float scalar) {
		return Vector4(x - scalar, y - scalar, z - scalar, w - scalar);
	}

	Vector4 Vector4::operator-(float scalar) const {
		return Vector4(x - scalar, y - scalar, z - scalar, w - scalar);
	}

	Vector4 Vector4::operator*(float scalar) {
		return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	Vector4 Vector4::operator*(float scalar) const {
		return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	Vector4 Vector4::operator/(float scalar) {
		return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	Vector4 Vector4::operator/(float scalar) const {
		return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	Vector4 Vector4::operator+(const Vector4& rhs) {
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.z);
	}

	Vector4 Vector4::operator+(const Vector4& rhs) const {
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.z);
	}

	Vector4 Vector4::operator-(const Vector4& rhs) {
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.z);
	}

	Vector4 Vector4::operator-(const Vector4& rhs) const {
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.z);
	}

	Vector4 Vector4::operator*(const Vector4& rhs) {
		return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.z);
	}

	Vector4 Vector4::operator*(const Vector4& rhs) const {
		return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.z);
	}

	Vector4 Vector4::operator/(const Vector4& rhs) {
		return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.z);
	}

	Vector4 Vector4::operator/(const Vector4& rhs) const {
		return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.z);
	}

	float Vector4::operator[](int index) {
		return comp[index];
	}

	float Vector4::operator[](int index) const {
		return comp[index];
	}

	Vector4& Vector4::operator=(const Vector4& other)
	{
		memcpy(this->comp, other.comp, sizeof(Vector4));
		return *this;
	}

	bool Vector4::operator==(float scalar) const {
		return x == scalar && y == scalar && z == scalar && w == scalar ? true : false;
	}

	bool Vector4::operator!=(float scalar) const {
		return !this->operator==(scalar);
	}

	bool Vector4::operator==(const Vector4& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w ? true : false;
	}

	bool Vector4::operator!=(const Vector4& other) const {
		return !this->operator==(other);
	}

	Vector4 Vector4::operator+=(float scalar) {
		return *this  = *this + scalar;
	}

	Vector4 Vector4::operator+=(const Vector4& other) {
		return *this = *this + other;
	}

	Vector4 Vector4::operator-=(float scalar) {
		return *this = *this - scalar;
	}

	Vector4 Vector4::operator-=(const Vector4& other) {
		return *this = *this - other;
	}

	Vector4 Vector4::operator*=(float scalar) {
		return *this = *this * scalar;
	}

	Vector4 Vector4::operator*=(const Vector4& other) {
		return *this = *this * other;
	}

	Vector4 Vector4::operator/=(float scalar) {
		return *this = *this / scalar;
	}

	Vector4 Vector4::operator/=(const Vector4& other) {
		return *this = *this / other;
	}

	Vector4 Vector4::operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

	float Vector4::Magnitude() {
		return sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
	}

	float Vector4::MagnitudeSq() {
		return SQ(x) + SQ(y) + SQ(z) + SQ(w);
	}

	float Vector4::MagnitudeFast() {
		return 1 / fast_inv_sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
	}

	float Vector4::MagnitudeSuper() {
		return fast_inv_sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
	}

	float Vector4::Major() {
		float a = std::fmaxf(x, y);
		float b = std::fmaxf(z, w);
		return std::fmaxf(a, b);
	}

	float Vector4::Minor() {
		float a = std::fminf(x, y);
		float b = std::fminf(z, w);
		return std::fminf(a, b);
	}

	float Vector4::Dot(const Vector4& other) {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	Vector4 Vector4::Normalize() {
		float mag = this->Magnitude();
		if (mag == 0)
			return {};

		return Vector4(x / mag, y / mag, z / mag, w / mag);
	}

	Vector4 Vector4::NormalizeFast() {
		float magFast = this->MagnitudeFast();
		if (magFast == 0)
			return {};

		return Vector4(x / magFast, y / magFast, z / magFast, w / magFast);
	}

	Vector4 Vector4::NormalizeSuper() {
		float magSuper = this->MagnitudeSuper();
		if (magSuper == 0)
			return {};

		return Vector4(x * magSuper, y * magSuper, z * magSuper, w * magSuper);
	}

	Vector3 Vector4::XYZ() const {
		return Vector3(x, y, z);
	}

	void Vector4::XYZ(float _x, float _y, float _z) {
		x = _x; y = _y; z = _z;
	}

	void Vector4::XYZ(const Vector3& vec) {
		*this = Vector4(vec, this->w);
	}

	Vector4 Vector4::Reflection(const Vector4& normal)
	{
		//r = d - 2(d * n) * n
		return (*this - 2 * (this->Dot(normal) * normal));
	}

	Vector4 Vector4::Refraction(const Vector4& normal, float n1, float n2)
	{
		const float n = n1 / n2;
		float dot = Dot(normal);
		float k = 1.0f - SQ(n) * (1.0f - SQ(dot));
		if (k < 0.0f)
			return Vector4::Zero;

		return n * (*this) - (n * dot + sqrtf(k)) * normal;
	}


	Vector4 Math::operator+(float scalar, const Vector4& rhs) {
		return rhs + scalar;
	}

	Vector4 Math::operator*(float scalar, const Vector4& rhs) {
		return rhs * scalar;
	}
}