#pragma once

#include "vector.h"
#include "string.h" //memcpy and memset
#include "defs.h"

/// Class representing a 3x3 matrix and some basic operations for working with matrices.
struct Matrix {
	Matrix() {}
	Matrix(float diag) { makeDiagonal(diag); }

	void makeDiagonal(float diag) {
		m[0][0] = m[1][1] = m[2][2] = diag;
		m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0.f;
	}

	Matrix(const Matrix &rhs) {
		memcpy(m, rhs.m, sizeof(float)*9);
	}

	float determinant() const {
		return  m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0]
		      + m[0][2]*m[1][0]*m[2][1] - m[2][0]*m[1][1]*m[0][2]
		      - m[2][1]*m[1][2]*m[0][0] - m[2][2]*m[1][0]*m[0][1];
	}

	Matrix inverse() {
		const float D = determinant();
		if (D < 1e-12) {
			return *this;
		}

		Matrix inv;
		const float rD = 1.0f / D;

		inv.m[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * rD;
		inv.m[0][1] =-(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * rD;
		inv.m[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * rD;

		inv.m[1][0] =-(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * rD;
		inv.m[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * rD;
		inv.m[1][2] =-(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * rD;

		inv.m[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * rD;
		inv.m[2][1] =-(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * rD;
		inv.m[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * rD;
		
		return inv;
	}

	Matrix transpose() {
		Matrix t;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				t.m[i][j] = m[j][i];
			}
		}
		return t;
	}


	Matrix& operator = (const Matrix& rhs) {
		if (this != &rhs) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					m[i][j] = rhs.m[i][j];
				}
			}
		}
		return *this;
	}

	Matrix& operator *= (const Matrix& rhs) { return *this = *this * rhs; }
	Matrix& operator += (const Matrix& rhs) { return *this = *this + rhs; }
	Matrix& operator -= (const Matrix& rhs) { return *this = *this - rhs; }



	Matrix operator * (const Matrix& rhs) const {
		Matrix res;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				res.m[i][j] = m[i][0] * rhs.m[0][j] +
				              m[i][1] * rhs.m[1][j] +
				              m[i][2] * rhs.m[2][j];
			}
		}
		return res;
	}
	Matrix operator+ (const Matrix& rhs) const {
		Matrix res;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				res.m[i][j] = m[i][j] + rhs.m[i][j];
			}
		}
		return res;
	}

	Matrix operator- (const Matrix& rhs) const {
		Matrix res;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				res.m[i][j] = m[i][j] - rhs.m[i][j];
			}
		}
		return res;
	}

	void makeZero() { memset(m, 0, sizeof(float)*9); }
	void makeIdentity() { makeDiagonal(1.f); }

	float m[3][3];
};


inline Vector operator* (const Vector& v, const Matrix& a) {
	return Vector(v.x * a.m[0][0] + v.y * a.m[1][0] + v.z * a.m[2][0] ,
	              v.x * a.m[0][1] + v.y * a.m[1][1] + v.z * a.m[2][1] ,
	              v.x * a.m[0][2] + v.y * a.m[1][2] + v.z * a.m[2][2]);
}

inline Vector& operator *= (Vector& v, const Matrix& a) {
	return v = v * a;
}


inline Matrix rotateAroundX(float angle) {
	Matrix a;
	a.makeZero();
	const float cosTheta = cosf(toRadians(angle));
	const float sinTheta = sinf(toRadians(angle));

	a.m[0][0] =  1;
	a.m[1][1] =  cosTheta;
	a.m[1][2] = -sinTheta;
	a.m[2][1] =  sinTheta;
	a.m[2][2] =  cosTheta;
	return a;
}

inline Matrix rotateAroundY(float angle) {
	Matrix a;
	a.makeZero();

	const float cosTheta = cosf(toRadians(angle));
	const float sinTheta = sinf(toRadians(angle));

	a.m[0][0] =  cosTheta;
	a.m[0][2] =  sinTheta;
	a.m[1][1] =  1;
	a.m[2][0] = -sinTheta;
	a.m[2][2] =  cosTheta;
	return a;
}

inline Matrix rotateAroundZ(float angle) {
	Matrix a;
	a.makeZero();

	const float cosTheta = cosf(toRadians(angle));
	const float sinTheta = sinf(toRadians(angle));

	a.m[0][0] =  cosTheta;
	a.m[0][1] = -sinTheta;
	a.m[1][0] =  sinTheta;
	a.m[1][1] =  cosTheta;
	a.m[2][2] =  1;
	return a;
}