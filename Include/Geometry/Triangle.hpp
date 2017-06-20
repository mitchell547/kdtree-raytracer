#include "Include/Geometry/Vec.hpp"
#include "Include/Geometry/Ray.hpp"
#include "Include/Geometry/float3.h"
#include "assert.h"
#pragma once
#define EPSILON 0.000001

struct triangle
{
	//Vec a1, a2, a3, c, _normal;	// position, color 
	float3 p[3];
	float3 c;
	float3 _normal;
	float3 v_n[3];	// vertex normals

	//sphere
	//Vec o; 
	float3 o;
	float rad;	

	//double difra; 
	//double reflect;
	float diffuse, specular, reflect, refraction;

	inline   triangle (float3 a1_, float3 a2_, float3 a3_, float3 c_, double reflect_) : c (c_), reflect (reflect_) {
		p[0] = a1_;	p[1] = a2_;	p[2] = a3_;
		//difra = 1;
		//_normal = (a2 - a1).cross(a3 - a1);
		_normal = ((p[1] - p[0]).cross(p[2] - p[0])).norm();
		diffuse = 0.7;
		specular = 0;
		for (int v = 0; v < 3; ++v)
			v_n[v] = _normal;
		//makeMinimumBoundingSphere (p[0], p[1], p[2]);
	}

	inline   triangle (Vec a1_, Vec a2_, Vec a3_, Vec c_, double reflect_) : c (c_), reflect (reflect_) {
		p[0] = a1_;	p[1] = a2_;	p[2] = a3_;
		//difra = 1;
		//_normal = (a2 - a1).cross(a3 - a1);
		_normal = ((p[1] - p[0]).cross(p[2] - p[0])).norm();
		diffuse = 0.7;
		specular = 0;
		for (int v = 0; v < 3; ++v)
			v_n[v] = _normal;
		//makeMinimumBoundingSphere (p[0], p[1], p[2]);
	}

	void  makeMinimumBoundingSphere (const float3 &p1, const float3 &p2, const float3 &p3) {
		// Calculate relative distances
		float A = (p1  ).distance (p2);
		float B = (p2  ).distance (p3);
		float C = (p3 ).distance (p1);

		// Re-orient triangle (make A longest side)
		const float3 *a = &p3, *b = &p1, *c = &p2;
		if (B < C) swap (B, C), swap (b, c);
		if (A < B) swap (A, B), swap (a, b);

		// If obtuse, just use longest diameter, otherwise circumscribe
		if ((B*B) + (C*C) <= (A*A)) {
			rad = A / 2.f;
			o = (*b + *c)/ 2.f;
		}
		else {
			// http://en.wikipedia.org/wiki/Circumscribed_circle
			float  cos_a = (B*B + C*C - A*A) / (B*C * 2);
			rad = A / (sqrt (1 - cos_a*cos_a)*2.f);
			float3 alpha = *a - *c, beta = *b - *c;
			o = (beta * alpha.dot (alpha) - alpha * beta.dot (beta)).cross (alpha.cross (beta)) /
				(alpha.cross (beta).dot (alpha.cross (beta)) * 2.f) + *c;
		}
		
	}

	void swap (float & A, float & B) {
		float C = A;
		A = B;
		B = C;
	}

	void swap (const Vec * A, const Vec * B) {
		const Vec * C = A;
		A = B;
		B = C;
	}

	void swap (const float3 * A, const float3 * B) {
		const float3 * C = A;
		A = B;
		B = C;
	}

	inline   triangle (){}

	inline   triangle moveX (const double step) const {
		triangle newT = *this;
		newT.p[0].v[AXIS::X] += step;
		newT.p[1].v[AXIS::X] += step;
		newT.p[2].v[AXIS::X] += step;
		return newT;
	}

	inline   triangle moveY (const double step) const {
		triangle newT = *this;
		newT.p[0].v[AXIS::Y] += step;
		newT.p[1].v[AXIS::Y] += step;
		newT.p[2].v[AXIS::Y] += step;
		return newT;
	}

	inline   triangle moveZ (const double step) const {
		triangle newT = *this;
		newT.p[0].v[AXIS::Z] += step;
		newT.p[1].v[AXIS::Z] += step;
		newT.p[2].v[AXIS::Z] += step;
		return newT;
	}

	inline   float3 normal () const {
		return _normal;
	}

	bool intersectSpher (const Ray &r) const { // returns distance, 0 if nohit 
		//Vec op = o - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
		//double t,  b = op.dot (r.d), det = b*b - op.dot (op) + rad*rad;
		//if (det + EPSILON <0) return false; else det = sqrt (det);
		//return (t = b - det)>EPSILON ? t : ((t = b + det) > EPSILON ? t : 0);
		//
		
		
		//a == 1; // because rdir must be normalized
		float3 k = r.o - o;
		double b = k.dot(r.d);
		double c = k.dot(k) - rad*rad;
		double d = b*b - c;

		if (d-EPSILON >= 0)
		{
			double sqrtfd = sqrt (d);
			// t, a == 1
			double t1 = -b + sqrtfd;
			double t2 = -b - sqrtfd;
			
		    double min_t = t1<t2 ? t1 : t2;//min (t1, t2);
			double max_t = t1>t2 ? t1 : t2; //max (t1, t2);
			
			double t = (min_t >= 0) ? min_t : max_t;
			//tResult = t;
			return (t > 0);
		}
		else
		{
			return true;
		}
		//
	}

	

	inline bool mollerTrumboreIntersect(const Ray &r, float3 & hit, float3 & uv) const {
		//float3 uv;
		float3 p0p1 = p[1] - p[0];
		float3 p0p2 = p[2] - p[0];
		float3 pvec = r.d.cross(p0p2);
		float det = p0p1.dot(pvec);

		//if (det < EPSILON) return false;	// culling
		if (fabs(det) < EPSILON) return false;	//not culling

		float invDet = 1 / det;
		float3 tvec = r.o - p[0];

		uv.v[0] = tvec.dot(pvec) * invDet;
		if (uv.v[0] < 0 || uv.v[0] > 1) return false;

		float3 qvec = tvec.cross(p0p1);
		uv.v[1] = r.d.dot(qvec) * invDet;
		if (uv.v[1] < 0 || uv.v[0] + uv.v[1] > 1) return false;

		//float t = p0p2.dot(qvec) * invDet; // distance from ray origin to intersection point
		if (p0p2.dot(qvec) * invDet < 0) return false;

		//float3 dnorm = r.d;
		//hit = r.o + dnorm.normalization() * p0p2.dot(qvec) * invDet;
		hit = p[0] + uv.v[0] * p0p1 + uv.v[1] * p0p2;
		return true;
	}

	#ifndef  baricentTest
	inline   bool		intersect (const Ray &r, float3 & hit) const {
		double k = 0;								//plane coefficient

		float3 pl = this->normal ().normalization();	//normal of flat for this object
		double D = -pl.dot (p[0]);					//point's projection on normal 
		double nu = pl.dot (r.o) + D;				//numerator 
		double den = pl.dot (r.d);					//denominator
		if (abs (den) - EPSILON<0) return false;
		k = -(nu) / (den);							
		hit = r.d*k + r.o;							//hit point
		//verifications
		float3 hitor = hit - r.o;						//verification that the
		double heor = hitor.dot (r.d) - EPSILON;	//point along the ray path

		//bool t = PointInTriangle (hit, a1, a2, a3); //verification that the
		bool t = PointInTriangle (hit, p[0], p[1], p[2]); //verification that the
		//////////////////////////////////////////////point in triangle

		if (t && heor > 0)return true;				//verification access
		else			return false;

	}

	inline   bool		SameSide (const Vec p1, const Vec p2, const Vec a, const Vec b) const {
		Vec cp1 = (b - a).cross (p1 - a);
		Vec cp2 = (b - a).cross (p2 - a);
		double d = cp1.dot (cp2);
		if (d + EPSILON >= 0) return true;
		else return false;
	}

	inline   bool		SameSide (const float3 p1, const float3 p2, const float3 a, const float3 b) const {
		float3 cp1 = (b - a).cross (p1 - a);
		float3 cp2 = (b - a).cross (p2 - a);
		double d = cp1.dot (cp2);
		if (d + EPSILON >= 0) return true;
		else return false;
	}

	inline   bool		PointInTriangle (const Vec p, const Vec a, const Vec b, const Vec c) const {
		if (SameSide (p, a, b, c) && 
			SameSide (p, b, a, c) && 
			SameSide (p, c, a, b))  
			 return true;
		else return false;
	}

	inline   bool		PointInTriangle (const float3 p, const float3 a, const float3 b, const float3 c) const {
		if (SameSide (p, a, b, c) && 
			SameSide (p, b, a, c) && 
			SameSide (p, c, a, b))  
			 return true;
		else return false;
	}

	#else
	// ifdef baricentTest
	/* code rewritten to do tests on the sign of the determinant */
	/* the division is before the test of the sign of the det    */
	/* and one CROSS has been moved out from the if-else if-else */
	inline   bool intersect (const Ray &r, Vec & hit) const {
		Vec edge1, edge2, tvec, pvec, qvec;
		double det, inv_det;
		double u, v, t;
		/* find vectors for two edges sharing vert0 */
		edge1 = a2 - a1;
		edge2 = a3 - a1;

		/* begin calculating determinant - also used to calculate U parameter */
		pvec = r.d.cross(edge2);

		/* if determinant is near zero, ray lies in plane of triangle */
		det = edge1.dot (pvec);

		/* calculate distance from vert0 to ray origin */
		tvec = r.o - a1;
		inv_det = 1.0 / det;

		qvec = tvec.cross(edge1);

		if (det > EPSILON)
		{
			u = tvec.dot (pvec);
			if (u - EPSILON < 0.0 || u > det - EPSILON)
				return false;

			/* calculate V parameter and test bounds */
			v = r.d.dot (qvec);
			if (v - EPSILON < 0.0 || u + v > det - EPSILON)
				return false;

		}
		else if (det < -EPSILON)
		{
			/* calculate U parameter and test bounds */
			u = tvec.dot (pvec);
			if (u + EPSILON > 0.0 || u - EPSILON < det)
				return false;

			/* calculate V parameter and test bounds */
			v = r.d.dot (qvec);
			if (v + EPSILON > 0.0 || u + v - EPSILON < det)
				return false;
		}
		else return false;  /* ray is parallell to the plane of the triangle */

		t = edge2.dot (qvec)*inv_det;
		u = inv_det;
		v = inv_det;
		hit = r.o + r.d*t;
		return true;
	}
	#endif
};