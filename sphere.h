#pragma once
#include "vector.h"
#include "defs.h"

struct Sphere {
public:
	Sphere() { O = Vector(0, 0, 0); }
	int intersect(const Ray& ray, IntersectionInfo& info) const;
	void setRadius(float r) { radius = r; }
	void setPos(const Vector& newPos) { O = newPos;}
private:
	float radius;
	Vector O;
};

// Intersect a given ray with a Sphere
// If an intersection point exists, then it can be expressed by the following equation:
// Let S = ray.start and D = ray.dir, then :
// (1): P = S + D*x  (for some scalar x)
// We also know that the point P is a point lying on the surface of the sphere.
// Therefore we have :
// (2): (P - O)*(P - O) = r*r  (where r is the radius of the sphere)
// Now if we substitute P in (2) with its equivalent from (1) we get
// (3): (S + D*x - O)^2 = r*r and if H = S - O we get:
// (4): (D*x + H)^2 = r*r. Expanded this yields:
// (5): (D*D)*x^2 + 2*D*H*x + H*H - r*r = 0
// This function solves the quadratic equation in (5) and returns information about the intersection
// If no intersection is found we return infinite distance to mark
int Sphere::intersect(const Ray &ray, IntersectionInfo &info) const {
	const Vector &S = ray.origin;
	const Vector &D = ray.dir;
	const Vector &H = S - O; // When sphere is located at 0,0,0, then H == S
	const float A = dot(D,D);
	const float B = 2 * dot(H, D);
	const float C = dot(H,H) - radius*radius;
	const float Dscr = B*B - 4*A*C;
	if (Dscr < 0) return false; // no solutions to the quadratic equation - then we don't have an intersection.
	const float x1 = (-B + sqrtf(Dscr)) / (2*A);
	const float x2 = (-B - sqrtf(Dscr)) / (2*A);
	float sol = x2; // get the closer of the two solutions...
	if (sol < 0) sol = x1; // ... but if it's behind us, opt for the other one
	if (sol < 0) return false; // ... still behind? Then the whole sphere is behind us - no intersection.

	float distSqr = sol*sol;
	if (info.distSq < distSqr) return false;

	info.distSq = distSqr;

	info.intersectionPoint = S + D * sol;
	info.normal = info.intersectionPoint/radius;

	const Vector& P = info.normal;
	info.u = 0.5f + atan2f(P.y, P.x)/(2.0f*pi());
	info.v = 0.5f + asinf(P.z)/pi();

	return true;
}
