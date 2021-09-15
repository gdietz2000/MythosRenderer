#include "../include/Vector2.h"
#include "../include/Vector3.h"

#include <iterator>

namespace Math
{
	//Static types
	const Vector3 Vector3::Zero		= Vector3(0, 0, 0);
	const Vector3 Vector3::One		= Vector3(1, 1, 1);
	const Vector3 Vector3::Unit_X	= Vector3(1, 0, 0);
	const Vector3 Vector3::Unit_Y	= Vector3(0, 1, 0);
	const Vector3 Vector3::Unit_Z	= Vector3(0, 0, 1);

	Vector3::Vector3()
		:x(0), y(0), z(0) {}

	Vector3::Vector3(float scalar)
		:x(scalar), y(scalar), z(scalar) {}

	Vector3::Vector3(float _x, float _y, float _z)
		:x(_x), y(_y), z(_z) {}

	Vector3::Vector3(float* xyz)
		: x(xyz[0]), y(xyz[1]), z(xyz[2]) {}

	Vector3::Vector3(const float* xyz)
		: x(xyz[0]), y(xyz[1]), z(xyz[2]) {}

	Vector3::Vector3(const Vector2& vec2, float _z)
		:x(vec2.x), y(vec2.y), z(_z) {}

	//Right hand side operators
	Vector3 Vector3::operator+(float scalar) {
		return Vector3(x + scalar, y + scalar, z + scalar);
	}

	Vector3 Vector3::operator+(float scalar) const {
		return Vector3(x + scalar, y + scalar, z + scalar);
	}

	Vector3 Vector3::operator-(float scalar) {
		return Vector3(x - scalar, y - scalar, z - scalar);
	}

	Vector3 Vector3::operator-(float scalar) const {
		return Vector3(x - scalar, y - scalar, z - scalar);
	}

	Vector3 Vector3::operator*(float scalar) {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 Vector3::operator*(float scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 Vector3::operator/(float scalar) {
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3 Vector3::operator/(float scalar) const {
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3 Vector3::operator+(const Vector3& rhs) {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 Vector3::operator+(const Vector3& rhs) const {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 Vector3::operator-(const Vector3& rhs) {
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 Vector3::operator-(const Vector3& rhs) const {
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 Vector3::operator*(const Vector3& rhs) {
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	Vector3 Vector3::operator*(const Vector3& rhs) const {
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	Vector3 Vector3::operator/(const Vector3& rhs) {
		return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	Vector3 Vector3::operator/(const Vector3& rhs) const {
		return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	Vector3& Vector3::operator=(const Vector3& other)
	{
		memcpy(this->comp, other.comp, sizeof(Vector3));
		return *this;
	}

	float Vector3::operator[](int index) {
		return comp[index];
	}

	float Vector3::operator[](int index) const {
		return comp[index];
	}

	bool Vector3::operator==(float scalar) const {
		return x == scalar && y == scalar && z == scalar ? true : false;
	}

	bool Vector3::operator!=(float scalar) const {
		return !this->operator==(scalar);
	}

	bool Vector3::operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z ? true : false;
	}

	bool Vector3::operator!=(const Vector3& other) const {
		return !this->operator==(other);
	}

	Vector3 Vector3::operator+=(float scalar) {
		return *this = *this + scalar;
	}

	Vector3 Vector3::operator+=(const Vector3& rhs) {
		return *this = *this + rhs;
	}

	Vector3 Vector3::operator-=(float scalar) {
		return *this = *this - scalar;
	}

	Vector3 Vector3::operator-=(const Vector3& rhs) {
		return *this = *this - rhs;
	}

	Vector3 Vector3::operator*=(float scalar) {
		return *this = *this * scalar;
	}

	Vector3 Vector3::operator*=(const Vector3& rhs) {
		return *this = *this * rhs;
	}

	Vector3 Vector3::operator/=(float scalar) {
		return *this = *this / scalar;
	}

	Vector3 Vector3::operator/=(const Vector3& rhs) {
		return *this = *this / rhs;
	}

	Vector3 Vector3::operator-() const {
		return Vector3(x != 0 ? -x : 0, y != 0 ? -y : 0, z != 0 ? -z : 0);
	}

	float* Vector3::data() { return &comp[0]; }

	float Vector3::Magnitude() {
		return sqrtf(SQ(x) + SQ(y) + SQ(z));
	}

	float Vector3::MagnitudeSq() {
		return SQ(x) + SQ(y) + SQ(z);
	}

	float Vector3::MagnitudeFast() {
		return 1 / fast_inv_sqrt(SQ(x) + SQ(y) + SQ(z));
	}

	float Vector3::MagnitudeSuper() {
		return fast_inv_sqrt(SQ(x) + SQ(y) + SQ(z));
	}

	float Vector3::Major() {
		float maximum = std::fmaxf(x, y);
		maximum = std::fmaxf(maximum, z);
		return maximum;
	}

	float Vector3::Minor() {
		float minor = std::fminf(x, y);
		minor = std::fminf(minor, z);
		return minor;
	}

	float Vector3::Dot(const Vector3& other) const{
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 Vector3::Normalize() {
		float mag = this->Magnitude();
		if (mag == 0)
			return {};

		return Vector3(x / mag, y / mag, z / mag);
	}

	Vector3 Vector3::NormalizeFast() {
		float magFast = this->MagnitudeFast();
		if (magFast == 0)
			return {};

		return Vector3(x / magFast, y / magFast, z / magFast);
	}

	Vector3 Vector3::NormalizeSuper() {
		float magSuper = this->MagnitudeSuper();
		if (magSuper == 0)
			return {};

		return Vector3(x * magSuper, y * magSuper, z * magSuper);
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		float tempX = y * other.z - z * other.y;
		float tempY = z * other.x - x * other.z;
		float tempZ = x * other.y - y * other.x;

		return Vector3(tempX, tempY, tempZ);
	}

	Vector3 Vector3::Reflection(const Vector3& normal)
	{
		//r = d - 2(d * n) * n
		return (*this - 2 * (this->Dot(normal) * normal));
	}

	Vector3 Vector3::Refraction(const Vector3& normal, float n1, float n2) 
	{
		const float n = n1 / n2;
		float dot = Dot(normal);
		float k = 1.0f - SQ(n) * (1.0f - SQ(dot));
		if (k < 0.0f)
			return Vector3::Zero;

		return n * (*this) - (n * dot + sqrtf(k)) * normal;
	}

	float Vector3::Angle(Vector3& other)
	{
		float dot = Dot(other);
		float L1 = Magnitude();
		float L2 = other.Magnitude();

		return acosf(dot / (L1 * L2));
	}

	Vector3 Math::operator+(float scalar, const Vector3& rhs) {
		return rhs + scalar;
	}

	Vector3 Math::operator*(float scalar, const Vector3& rhs) {
		return rhs * scalar;
	}

	float Math::Distance(const Vector3& a, const Vector3& b) {
		return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2));
	}

	Vector3 Math::VectorMin(const Vector3& a, const Vector3& b)
	{
		Vector3 minimum;
		minimum.x = mmin(a.x, b.x);
		minimum.y = mmin(a.y, b.y);
		minimum.z = mmin(a.z, b.z);
		return minimum;
	}

	Vector3 Math::VectorMax(const Vector3& a, const Vector3& b)
	{
		Vector3 maximum;
		maximum.x = mmax(a.x, b.x);
		maximum.y = mmax(a.y, b.y);
		maximum.z = mmax(a.z, b.z);
		return maximum;
	}
}