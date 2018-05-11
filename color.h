#pragma once

#include "defs.h"
inline float clamp(float val, float minVal, float maxVal) { return Min(Max(val, minVal), maxVal); }

struct Color {

	float r, g, b;
	Color() {}
	Color(float r, float g, float b): r(r), b(b), g(g) {}

	Color operator * (float scalar) const {
		return Color(r * scalar, g * scalar, b * scalar);
	}

	Color operator * (const Color& rhs) const {
		return Color(r * rhs.r, g * rhs.g, b * rhs.b);
	}

	Color& operator += (float scalar) {
		r += scalar;
		g += scalar;
		b += scalar;
		return *this;
	}

	Color& operator += (const Color& rhs) {
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		return *this;
	}

	Color& operator -= (float scalar) {
		r -= scalar;
		g -= scalar;
		b -= scalar;
		return *this;
	}

	Color& operator -= (const Color& rhs) {
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		return *this;
	}

	Color& operator /= (float scalar) {
		scalar = 1.0f/scalar;
		r *= scalar;
		g *= scalar;
		b *= scalar;
		return *this;
	}

	Color& operator=(const Color& rhs) {
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		return *this;
	}

	Color(const Color& rhs) {
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
	}

	Color operator / (float scalar) const {
		scalar = 1.0f / scalar;
		return Color(r * scalar, g * scalar, b * scalar);
	}

	Color operator - (const Color& rhs) const {
		return Color(r - rhs.r, g - rhs.g, b - rhs.b);
	}

	Color operator + (const Color& rhs) const {
		return Color(r + rhs.r, g + rhs.g, b + rhs.b);
	}

	Color operator - () const {
		return Color(-r, -g, -b);
	}

	uint8 getRedUINT8()   const { return uint8(clamp(r,0.0f,1.0f)*255.0f); }
	uint8 getGreenUINT8() const { return uint8(clamp(g,0.0f,1.0f)*255.0f); }
	uint8 getBlueUINT8()  const { return uint8(clamp(b,0.0f,1.0f)*255.0f); }

	float intensity() const { return (r+g+b)/3.0f; }

	void makeZero() {
		r = g = b = 0.0f;
	}
};

//inline Color operator*(const Color &a, const Color &b) {
//	return Color(a.r*b.r, a.b*b.b, a.g*b.g);
//}
