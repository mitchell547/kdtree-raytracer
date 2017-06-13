#pragma once

//#define baricentTest //standartTest

//#define EPSILON 0.000001f//0.000000001
#define INF 1e20f
#define fast


/*float inline __declspec (naked) __fastcall sqrtf (float n)
{
	_asm fld qword ptr[esp + 4]
		_asm fsqrt
	_asm ret 8
}*/
float inline  sqrtP (float n)
{
	return pow (n, 0.5f);
}

#ifdef fast
#define sqrt   sqrtP

#endif


template<typename T>
struct Vec3
{
	T x, y, z;                  // position, also color (r,g,b) 
	__forceinline     Vec3<T> (T x_, T y_, T z_) : x (x_), y (y_), z (z_){}
	__forceinline     Vec3<T> () : x (0), y (0), z (0){	};
	__forceinline     Vec3<T>		operator+	(const Vec3<T> &b)	const
	{
		return Vec3<T> (this->x + b.x, this->y + b.y, this->z + b.z);
	}
	__forceinline     Vec3<T>		operator-	(const Vec3<T> &b)	const
	{
		return Vec3<T> (this->x - b.x, this->y - b.y, this->z - b.z);
	}
	__forceinline     Vec3<T>		operator*	(T b)				const
	{
		return Vec3<T> (x*b, y*b, z*b);
	}
	__forceinline     Vec3<T>		operator/	(T b)				const
	{
		T p = 1.f/ b;
		return Vec3<T> (x * p, y * p, z * p);
	}
	__forceinline     Vec3<T>		inv ()					const
	{
		return Vec3<T> (-x, -y, -z);
	}
	__forceinline     Vec3<T>		mult (const Vec3<T> &b)	const
	{
		return Vec3<T> (x*b.x, y*b.y, z*b.z);
	}
	__forceinline     T				distance (const Vec3<T> &b)	const
	{
		return  sqrt ((x - b.x)*(x - b.x) +
			(y - b.y)*(y - b.y) +
			(z - b.z)*(z - b.z));
	}
	__forceinline     Vec3<T>&		normalization ()
	{
		return *this = *this * (1.f/ sqrt (x*x + y*y + z*z));
	}
	__forceinline     T				dot (const Vec3<T> &b)	const
	{
		//__fmaf_rd ();
		return fma (this->x,b.x , fma (this->y, b.y, this->z*b.z));//this->x*b.x + this->y*b.y + this->z*b.z;
	} // cross: 
	__forceinline     Vec3<T>		cross		(const Vec3<T>&b)	const
	{
		return Vec3<T> (this->y*b.z - this->z*b.y, this->z*b.x - this->x*b.z, this->x*b.y - this->y*b.x);
		//return Vec3<T> (fma (this->y, b.z, -this->z*b.y), fma (this->z,b.x, - this->x*b.z), fma (this->x,b.y, - this->y*b.x));
	}
};

template<typename T>
struct RayT
{
	Vec3<T> o, d; //o-origin d-direction
	  RayT (Vec3<T> o_, Vec3<T> d_) : o (o_), d (d_) {}
};

template<typename T>
struct polygon3
{
	Vec3<T> a1, a2, a3, c, _normal;      // position, color
	Vec3<T> Na1, Na2, Na3;
	Vec3<T> o; T rad;//spher
	T diffuse; T specular;
	T refract;
	inline    polygon3 (Vec3<T> a1_, Vec3<T> a2_, Vec3<T> a3_, Vec3<T> c_, T specular_) :
							a1 (a1_), a2 (a2_), a3 (a3_), c (c_), specular (specular_)
	{
		Na1 = Na2 = Na3 = _normal = (a2 - a1).cross (a3 - a1).normalization ();
		//makeMinimumBoundingSphere (a1, a2, a3);
		diffuse = 0.5f;
		refract = 0.f;
	}
	inline    polygon3 (Vec3<T> a1_, Vec3<T> a2_, Vec3<T> a3_, Vec3<T> c_,
		Vec3<T> Na1_, Vec3<T> Na2_, Vec3<T> Na3_, T specular_) :
								a1 (a1_), a2 (a2_), a3 (a3_), c (c_),
								Na1 (Na1_), Na2 (Na2_), Na3 (Na3_), specular (specular_)
	{
		_normal = (a2 - a1).cross (a3 - a1).normalization ();
		//makeMinimumBoundingSphere (a1, a2, a3);
		diffuse = 0.5f;
		refract = 0.71f;
	}
	inline     polygon3 (){}
	//
	inline     polygon3<T> moveX (const T step)const
	{
		polygon3<T> newT = *this;
		newT.a1.x += step;
		newT.a2.x += step;
		newT.a3.x += step;
		return newT;
	}
	inline     polygon3<T> moveY (const T step)const
	{
		polygon3<T> newT = *this;
		newT.a1.y += step;
		newT.a2.y += step;
		newT.a3.y += step;
		return newT;
	}
	inline     polygon3<T> moveZ (const T step)const
	{
		polygon3<T> newT = *this;
		newT.a1.z += step;
		newT.a2.z += step;
		newT.a3.z += step;
		return newT;
	}


	void  makeMinimumBoundingSphere (const Vec3<T> &p1, const Vec3<T> &p2, const Vec3<T> &p3) {

		// Calculate relative distances
		float A = (p1).distance (p2);
		float B = (p2).distance (p3);
		float C = (p3).distance (p1);

		// Re-orient triangle (make A longest side)
		const Vec3<T> *a = &p3, *b = &p1, *c = &p2;
		if (B < C) swap (B, C), swap (b, c);
		if (A < B) swap (A, B), swap (a, b);

		// If obtuse, just use longest diameter, otherwise circumscribe
		if ((B*B) + (C*C) <= (A*A)) {
			rad = A / 2.f;
			o = (*b + *c) / 2.f;
		}
		else {
			// http://en.wikipedia.org/wiki/Circumscribed_circle
			float  cos_a = (B*B + C*C - A*A) / (B*C * 2);
			rad = A / (sqrt (1 - cos_a*cos_a)*2.f);
			Vec3<T> alpha = *a - *c, beta = *b - *c;
			o = (beta * alpha.dot (alpha) - alpha * beta.dot (beta)).cross (alpha.cross (beta)) /
				(alpha.cross (beta).dot (alpha.cross (beta)) * 2.f) + *c;
		}

	}
	
	void swap (float & A, float & B)
	{
		float C = A;
		A = B;
		B = C;
	}
	void swap (const Vec3<T> * A, const Vec3<T> * B)
	{
		const Vec3<T> * C = A;
		A = B;
		B = C;
	}




	__forceinline     Vec3<T> normal ()const
	{
		return _normal;
	}
	/*
	__forceinline bool intersectSpher (const RayT<T> &r) const
	{
		Vec3<T> k = r.o - o;
		float b = r.d.dot (k);
		float c = k.dot (k) - rad*rad;
		float d = b*b - c;

		if (d - EPSILON >= 0.f)
		{
			float t1 = -b + d;
			float t2 = -b - d;
			if (t2 >= 0) return (t2 > 0);
			else  return (t1 > 0);
		}
		else
		{
			return true;
		}
	}

	bool ray_sphereIntersect (const RayT<T> &r)const
	{
		// Intersections++;
		Vec3<T> L = o - r.o;
		float LdotR = L.dot (r.d);
		if (LdotR > 0.0f)
		{
			float D2 = Vec3<T> ().distance (L) - LdotR * LdotR;
			if (D2 < rad*rad)
			{
				float Distance = LdotR - sqrt (rad*rad - D2);
				if (Distance >= 0.0f && Distance < INF)
				{
					return true;
				}
			}
		}
		return false;
	}

	bool ray_sphereIntersection (const RayT<T> &r)const {
		Vec3<T> k = r.o - o;
		float b = r.d.dot (k);
		float c = k.dot (k) - (rad * rad);
		float d = b * b - c;

		if (d  < 0.0f) {
			return false;
		}
		return true;
	}

	*/

	/* code rewritten to do tests on the sign of the determinant */
	/* the division is before the test of the sign of the det    */
	/* and one CROSS has been moved out from the if-else if-else */

	/*__forceinline bool intersect (const RayT<T> &r, T &t, T &u, T &v, Vec3<T> & P)const
	{

		Vec3<T> e1, e2;
		Vec3<T>  Q, TL;
		T det, inv_det;
		//Find Vec3<T>tors for two edges sharing a1
		e1 = a2 - a1;
		e2 = a3 - a1;
		//Begin calculating determinant - also used to calculate u parameter
		P = r.d.cross (e2);
		//if determinant is near zero, ray lies in plane of triangle
		det = e1.dot (P);
		//NOT CULLING
		if (det > -EPSILON && det < EPSILON) return false;
		inv_det = 1.0 / det;

		//calculate distance from a1 to ray origin
		TL = r.o - a1;

		//Calculate u parameter and test bound
		u = TL.dot (P) * inv_det;
		//The intersection lies outside of the triangle
		if (u < 0.0 || u > 1.0) return false;

		//Prepare to test v parameter
		Q = TL.cross (e1);

		//Calculate V parameter and test bound
		v = r.d.dot (Q) * inv_det;
		//The intersection lies outside of the triangle
		if (v < 0.0 || u + v  > 1.0) return false;

		t = e2.dot (Q) * inv_det;

		if (t > EPSILON) { //ray intersection
			P = r.o + r.d*t;
			return true;
		}
		// No hit, no win
		return false;
	}*/
};


typedef Vec3<float> Vec;
typedef Vec3<float> Vecf;
typedef RayT<float> Ray;
//typedef polygon3<float> triangle;


inline     double	clamp (double x) { return x < 0 ? 0 : x>1 ? 1 : x; }
inline     float	clamp (float x) { return x < 0.f ? 0.f : x>1.f ? 1.f : x; }
inline     int		toInt (double x) { return int (pow (clamp (x), 1 / 2.2) * 255 + .5); }
inline     char		toChar (double x) { return char (pow (clamp (x), 1 / 2.2) * 255); }
inline     char		toChar (float x) { return char (pow (clamp (x), 1.f/ 2.2f) * 255); }