
#pragma once
#include "Vec.hpp"
#include <math.h>


enum AXIS {	X = 0, 
			Y = 1, 
			Z = 2 };

struct float3 {
	float v[3];

	/*inline float3() {
		v[0] = 0; v[1] = 0; v[2] = 0;
	}*/

	inline float3(float x=0, float y=0, float z=0) { v[X] = x; v[Y] = y; v[Z] = z; }

	//inline float3(const float3&b) { v[0] = b.v[0]; v[1] = b.v[1]; v[2] = b.v[2]; }

	inline float3(const Vec&b) { v[0] = b.x; v[1] = b.y; v[2] = b.z; }

	inline   float3		operator+	  (const float3 &b) const {
		return float3 (v[X] + b.v[X], v[Y] + b.v[Y], v[Z] + b.v[Z]);
	}

	inline   float3		operator-	  (const float3 &b) const {
		return float3 (v[X] - b.v[X], v[Y] - b.v[Y], v[Z] - b.v[Z]);
	}

	inline   float3		operator*	  (float b)	 const {
		return float3 (v[X] * b, v[Y] * b, v[Z] * b);
	}

	inline   float3		operator*	  (float3 &b)	 const {
		return float3 (v[X] * b.v[X], v[Y] * b.v[Y], v[Z] * b.v[Z]);
	}

	inline friend float3 operator* (float a, const float3& b) {
		return float3 (b.v[X] * a, b.v[Y] * a, b.v[Z] * a);
	}

	inline friend   float3		operator/	  (const float3 &a, float b)	 {
		return float3 (a.v[X] / b, a.v[Y] / b, a.v[Z] / b);
	}

	inline friend  float3		operator/	  (float a, const float3 &b)	 {
		return float3 (a/b.v[X], a/b.v[Y], a/b.v[Z]);
	}

	inline   float3&		operator+=	  (const float3 &b) {
		v[X] = v[X] + b.v[X]; v[Y] = v[Y] + b.v[Y]; v[Z] = v[Z] + b.v[Z];
		return *this;
	}

	inline bool operator>(const float3 &b) {
		return v[X] > b.v[X] && v[Y] > b.v[Y] && v[Z] > b.v[Z];
	}
	inline bool operator<(const float3 &b) {
		return v[X] < b.v[X] && v[Y] < b.v[Y] && v[Z] < b.v[Z];
	}
	inline bool operator==(const float3 &b) {
		return v[X] == b.v[X] && v[Y] == b.v[Y] && v[Z] == b.v[Z];
	}
	inline bool operator!=(const float3 &b) {
		return v[X] != b.v[X] && v[Y] != b.v[Y] && v[Z] != b.v[Z];
	}

	inline   float3		inv			  ()			 const {
		return float3 (-v[X], -v[Y], -v[Z]);
	}

	inline   float	distance	  (const float3 &b) const {
		return  sqrt((v[X] - b.v[X])*(v[X] - b.v[X]) +
			(v[Y] - b.v[Y])*(v[Y] - b.v[Y]) +
			(v[Z] - b.v[Z])*(v[Z] - b.v[Z]));
	}

	inline   float sqr_dist(const float3 &b) const {
		return  (v[X] - b.v[X])*(v[X] - b.v[X]) +
			(v[Y] - b.v[Y])*(v[Y] - b.v[Y]) +
			(v[Z] - b.v[Z])*(v[Z] - b.v[Z]);
	}

	inline   float3&		normalization () {
		return *this = *this * (1 / sqrt (v[X]*v[X] + v[Y]*v[Y] + v[Z]*v[Z]));
	}

	inline   float	dot			  (const float3 &b) const {
		return v[X] * b.v[X] + v[Y] * b.v[Y] + v[Z] * b.v[Z];
	} 

	inline   float3		cross		  (const float3&b)  const {
		return float3 (v[Y]*b.v[Z] - v[Z]*b.v[Y], v[Z]*b.v[X] - v[X]*b.v[Z], v[X]*b.v[Y] - v[Y]*b.v[X]);
	}

	
	// for compability
	float3& operator= (const Vec& b) {
		v[0] = b.x;
		v[1] = b.y;
		v[2] = b.z;
		return *this;
	}
};

Vec::Vec(const float3 & b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
}

Vec& Vec::operator=(const float3& b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
	return *this;
}