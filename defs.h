#pragma once

#include <stdio.h>
#include <stdlib.h> //rand and RAND_MAX
#include "vector.h"


// PREDEFINED COLORS
#define RED         Color(1.0f, 0.0f, 0.0f)
#define GREEN       Color(0.0f, 1.0f, 0.0f)
#define BLUE        Color(0.0f, 0.0f, 1.0f)

#define CYAN        Color(0.0f, 1.0f, 1.0f)
#define MAGENTA     Color(1.0f, 0.0f, 1.0f)
#define YELLOW      Color(1.0f, 1.0f, 0.0f)

#define WHITE       Color(1.0f, 1.0f, 1.0f)
#define BLACK       Color(0.0f, 0.0f, 0.0f)

// MACROS
#define max(a,b) (((a) > (b))? (a) : (b))
#define min(a,b) (((a) < (b))? (a) : (b))

//TYPEDEFS
typedef unsigned char       uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

typedef char                 int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

// UTILITY FUNCTIONS
inline float pi() { return 3.141592653589793f; }
inline float toRadians(float degrees) { return (degrees / 180.0f) * pi(); }
inline float toDegrees(float radians) { return (radians / pi()) * 180.0f; }
inline float sqr(float x) { return x * x; }
inline float getRandomInRange(float min, float max) { return min + (max-min)*(float(rand())/RAND_MAX); }
inline Vector faceforward(const Vector &v, const Vector &n) { return dot(v, n) < 0.0f ? n : -n; }

struct Ray {
	Vector origin;
	Vector dir;
	int depth;
};

struct IntersectionInfo {
	Vector intersectionPoint;
	Vector normal;
	float distSq;
	float u;
	float v;

	bool isValid() { return distSq < 1e9f; }
	IntersectionInfo(): distSq(1e9f), u(0.0f), v(0.0f) {}
};
