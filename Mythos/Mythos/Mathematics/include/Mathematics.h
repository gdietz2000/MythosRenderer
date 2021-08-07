#pragma once

#include <cmath>

namespace Math 
{

//Defines
#define PI 3.1415926535f
#define TAU 6.2831853071f
#define SQ(x) x * x

	//Helper Functions

	template<typename T>
	inline void swap(T& a, T& b) { T t = a; a = b; b = t; }

	inline float radians(float degrees) {return degrees * (PI / 180.0f);}
	inline float degrees(float radians) {return radians * (180.0f / PI);}

	template<typename T>
	inline T mmax(T a, T b) { return a > b ? a : b; }
	template<typename T>
	inline T mmin(T a, T b) { return a < b ? a : b; }

	template<typename T>
	inline T mabs(T a) { return a < 0 ? -a : a; }

	inline bool even(int number) { return number % 2 == 0 ? true : false; };
	inline bool odd(int number) { return number % 2 == 0 ? false : true; };

	inline int sign(int number) { return number > 0 ? 1 : number < 0 ? -1 : 0; }
	inline int sign(float number) { return number > 0.0f ? 1 : number < 0.0f ? -1 : 0; }

	inline int clamp(int value, int minimum, int maximum) { return value < minimum ? minimum : (value > maximum ? maximum : value); }
	inline float clamp(float value, float minimum, float maximum) { return value < minimum ? minimum : (value > maximum ? maximum : value); }


	inline float fast_inv_sqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck? 
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}

	inline bool almost_equal(float a, float b, float tolerance) 
	{
		return mabs(a - b) * pow(10, tolerance) <= 1;
	}

	//Declarations

		//Vectors
	struct Vector2;
	struct Vector3;
	struct Vector4;
		
		//Matrices
	struct Matrix2;
	struct Matrix3;
	struct Matrix4;
		
		//Quaternion
	struct Quaternion;
}