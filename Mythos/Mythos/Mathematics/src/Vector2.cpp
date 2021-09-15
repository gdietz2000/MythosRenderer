#include "../include/Vector2.h"

#include <algorithm>

namespace Math
{
	//Static types
	const Vector2 Vector2::Zero		= Vector2(0, 0);
	const Vector2 Vector2::One		= Vector2(1, 1);
	const Vector2 Vector2::Unit_X	= Vector2(1, 0);
	const Vector2 Vector2::Unit_Y	= Vector2(0, 1);

	//Constructors
	Vector2::Vector2()
		: x(0), y(0) {}

	Vector2::Vector2(float scalar)
		: x(scalar), y(scalar) {}

	Vector2::Vector2(float _x, float _y)
		: x(_x), y(_y) {}

	//Right hand side operators
	Vector2 Vector2::operator+(float scalar) {
		return Vector2(x + scalar, y + scalar);
	}

	Vector2 Vector2::operator+(float scalar) const {
		return Vector2(x + scalar, y + scalar);
	}

	Vector2 Vector2::operator-(float scalar) {
		return Vector2(x - scalar, y - scalar);
	}

	Vector2 Vector2::operator-(float scalar) const {
		return Vector2(x - scalar, y - scalar);
	}

	Vector2 Vector2::operator*(float scalar) {
		return Vector2(x * scalar, y * scalar);
	}

	Vector2 Vector2::operator*(float scalar) const {
		return Vector2(x * scalar, y * scalar);
	}

	Vector2 Vector2::operator/(float scalar) {
		return Vector2(x / scalar, y / scalar);
	}

	Vector2 Vector2::operator/(float scalar) const {
		return Vector2(x / scalar, y / scalar);
	}

	Vector2 Vector2::operator+(const Vector2& rhs) {
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const {
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 Vector2::operator-(const Vector2& rhs) {
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 Vector2::operator-(const Vector2& rhs) const {
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 Vector2::operator*(const Vector2& rhs) {
		return Vector2(x * rhs.x, y * rhs.y);
	}

	Vector2 Vector2::operator*(const Vector2& rhs) const {
		return Vector2(x * rhs.x, y * rhs.y);
	}

	Vector2 Vector2::operator/(const Vector2& rhs) {
		return Vector2(x / rhs.x, y / rhs.y);
	}

	Vector2 Vector2::operator/(const Vector2& rhs) const {
		return Vector2(x / rhs.x, y / rhs.y);
	}

	float Vector2::operator[](int index) {
		return comp[index];
	}

	float Vector2::operator[](int index) const {
		return comp[index];
	}

	Vector2& Vector2::operator=(const Vector2& other)
	{
		memcpy(this->comp, other.comp, sizeof(Vector2));
		return *this;
	}

	bool Vector2::operator==(float value) const {
		return x == value && y == value ? true : false;
	}

	bool Vector2::operator!=(float value) const {
		return !this->operator==(value);
	}

	bool Vector2::operator==(const Vector2& other) const {
		return x == other.x && y == other.y ? true : false;
	}

	bool Vector2::operator!=(const Vector2& other) const {
		return !this->operator==(other);
	}

	Vector2 Vector2::operator+=(float scalar) {
		return *this = *this + scalar;
	}

	Vector2 Vector2::operator+=(const Vector2& rhs) {
		return *this = *this + rhs;
	}

	Vector2 Vector2::operator-=(float scalar) {
		return *this = *this - scalar;
	}

	Vector2 Vector2::operator-=(const Vector2& rhs) {
		return *this = *this - rhs;
	}

	Vector2 Vector2::operator*=(float scalar) {
		return *this = *this * scalar;
	}

	Vector2 Vector2::operator*=(const Vector2& rhs) {
		return *this = *this * rhs;
	}

	Vector2 Vector2::operator/=(float scalar) {
		return *this = *this / scalar;
	}

	Vector2 Vector2::operator/=(const Vector2& rhs) {
		return *this = *this / rhs;
	}

	Vector2 Vector2::operator-() const {
		return Vector2(-x, -y);
	}

	//Attribute Functions
	float Vector2::Magnitude() {
		return sqrt(SQ(x) + SQ(y));
	}

	float Vector2::MagnitudeSq() {
		return SQ(x) + SQ(y);
	}

	float Vector2::MagnitudeFast() {
		return 1 / fast_inv_sqrt(SQ(x) + SQ(y));
	}

	float Vector2::MagnitudeSuper() {
		return fast_inv_sqrt(SQ(x) + SQ(y));
	}

	float Vector2::Major() {
		return x > y ? x : y;
	}

	float Vector2::Minor() {
		return x < y ? x : y;
	}

	float Vector2::Heading() {
		return atan2f(this->y, this->x);
	}

	float Vector2::Dot(const Vector2& other) {
		return this->x * other.x + this->y * other.y;
	}

	float Vector2::Determinant(const Vector2& other) {
		return x * other.y - y * other.x;
	}

	Vector2 Vector2::Normalize() {
		float mag = this->Magnitude();

		if (mag == 0)
			return {};

		return Vector2(x / mag, y / mag);
	}

	Vector2 Vector2::NormalizeFast() {
		float magFast = this->MagnitudeFast();

		if (magFast == 0)
			return {};

		return Vector2(x / magFast, y / magFast);
	}

	Vector2 Vector2::NormalizeSuper() {
		float magSuper = this->MagnitudeSuper();
		if (magSuper == 0)
			return {};

		return Vector2(x * magSuper, y * magSuper);
	}

	Vector2 Vector2::Reflection(const Vector2& normal)
	{
		//r = d - 2(d * n) * n
		return (*this - 2 * (this->Dot(normal) * normal));
	}

	Vector2 Vector2::Refraction(const Vector2& normal, float n1, float n2)
	{
		const float n = n1 / n2;
		float dot = Dot(normal);
		float k = 1.0f - SQ(n) * (1.0f - SQ(dot));
		if (k < 0.0f)
			return Vector2::Zero;

		return n * (*this) - (n * dot + sqrtf(k)) * normal;
	}

	float Vector2::Angle(const Vector2& other)
	{
		float dot = Dot(other);
		float det = Determinant(other);
		return atan2(det, dot);
	}

	Vector2 Math::operator+(float scalar, const Vector2& rhs) {
		return rhs + scalar;
	}

	Vector2 Math::operator*(float scalar, const Vector2& rhs) {
		return rhs * scalar;
	}
}