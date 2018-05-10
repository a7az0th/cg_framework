#pragma once

#include <math.h> //sqrtf

// Class represents a standard vector (or point) in 3D space.
// The vector class provides all the necessary basic functionality
// for calculations with vectors.
struct Vector {
	float x, y, z;

	Vector() { /*blank on purpose*/ }
	Vector(float x, float y, float z): x(x), y(y), z(z) {}
	Vector(const Vector& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	float lengthSqr() const { return x*x + y*y + z*z; }
	float length() const { return sqrtf(x*x + y*y + z*z); }
	Vector normalize() {
		const float factor = 1.0f / sqrtf(x*x + y*y + z*z);
		x *= factor;
		y *= factor;
		z *= factor;
		return *this;
	}

	Vector operator - (const Vector& rhs) const { return Vector(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector operator - () const { return Vector(-x, -y, -z); }
	Vector operator + (const Vector& rhs) const { return Vector(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector operator * (float scalar) const { return Vector(x * scalar, y * scalar, z * scalar); }
	Vector operator / (float scalar) const {
		scalar = 1.0f / scalar;
		return Vector(x * scalar, y * scalar, z * scalar);
	}
	
	Vector& operator = (const Vector& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	Vector& operator += (const Vector& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	
	Vector& operator += (float scalar) {
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}
	
	Vector& operator -= (const Vector& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector& operator -= (float scalar) {
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}

	Vector& operator /= (float scalar) {
		scalar = 1.0f / scalar;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	
	void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

	float& operator[] (int index) {
		switch (index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return x;
		}
	}
	const float& operator[] (int index) const {
		switch (index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return x;
		}
	}

	friend Vector operator*(float lhs, const Vector & rhs) { return rhs * lhs; }
};


/// Returns the value of the dot product of vectors 'a' and 'b'
inline float dot(const Vector& a, const Vector& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// Returns the vector resulting from the cross product of vectors 'a' and 'b'
inline Vector cross(const Vector& a, const Vector& b) {
	const float x = a.y * b.z - a.z * b.y;
	const float y = a.x * b.z - a.z * b.x;
	const float z = a.x * b.y - a.y * b.x;
	return Vector(x, -y, z);
}